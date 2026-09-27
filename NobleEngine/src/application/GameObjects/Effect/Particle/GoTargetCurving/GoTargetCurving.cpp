// GoTargetCurving.cpp
#include "GoTargetCurving.h"
#include <algorithm>

GoTargetCurving::GoTargetCurving()
{
	render_.modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	render_.psoConfig_.vs = "assets/shaders/Particle/GoTargetCurving/GoTargetCurving.VS.hlsl";
	render_.psoConfig_.ps = "assets/shaders/Particle/GoTargetCurving/GoTargetCurving.PS.hlsl";
	render_.psoConfig_.blendID = BlendStateID::Alpha;
	render_.psoConfig_.depthStencilID = DepthStencilID::TestOnly;
	render_.SetupFromShaders();

	instanceBufferID_ = Game::Resource::CreateDynamic();

	t_default_ = Game::Asset::Texture::Load("assets/engine/texture/particle/circle2.png");
}

GoTargetCurving::~GoTargetCurving()
{}

void GoTargetCurving::Emit(const Params& params)
{
	const float spread = std::max(params.spread, 0.0f);

	Particle particle;
	particle.params = params;
	particle.position = params.start;
	// 弧の頂点を粒ごとに少しずらす
	particle.controlOffset = Vector3(
		Game::Math::Rand::RandFloat(-spread, spread, 3),
		params.height * Game::Math::Rand::RandFloat(0.7f, 1.3f, 3),
		Game::Math::Rand::RandFloat(-spread, spread, 3));
	particles_.push_back(particle);
}

void GoTargetCurving::Update(int32_t c_ID)
{
	const float deltaTime = Game::Time::GetScaledDeltaTimeMs() * 0.001f;

	for (size_t i = 0; i < particles_.size();)
	{
		Particle& particle = particles_[i];
		const Params& params = particle.params;

		// 始点/制御点/終点
		const Vector3 start = params.start;
		const Vector3 end = GetTargetPosition(params);
		const Vector3 control = (start + end) * 0.5f + particle.controlOffset;

		// 曲線のおおよその長さ(弦と、制御点を通る折れ線の平均)
		const float length = ((end - start).Length() + (control - start).Length() + (end - control).Length()) * 0.5f;

		// speed(距離/秒)ぶん進める。目標が逃げ続けても、最低でもkMaxFlightSeconds秒で届く速さにする
		const float step = std::max(params.speed * deltaTime / std::max(length, 0.0001f), deltaTime / kMaxFlightSeconds);
		particle.progress = std::min(particle.progress + step, 1.0f);

		// 2次ベジェ曲線
		const float t = particle.progress;
		const float u = 1.0f - t;
		particle.position = start * (u * u) + control * (2.0f * u * t) + end * (t * t);

		// 届いた
		if (particle.progress >= 1.0f)
		{
			if (eventBus_ && params.arriveEventData.type != EventType::MAX)
			{
				eventBus_->Notify(params.arriveEventData);
			}

			// 末尾と入れ替えて消す(順番は気にしない)
			particles_[i] = particles_.back();
			particles_.pop_back();
			continue;
		}
		++i;
	}

	// Drawで使う行列(Chunkと同じく、Updateで作ってDrawでセットする)
	perView_.viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(c_ID);
	perView_.billboard = Game::Camera::Getter::GetBillboardMatrix(c_ID);
}

void GoTargetCurving::Draw(int32_t rt_ID)
{
	if (particles_.empty()) return;

	instances_.clear();
	for (const Particle& particle : particles_)
	{
		Instance instance;
		instance.position = particle.position;
		instance.scale = particle.params.scale * std::min((1.0f - particle.progress) * 5.0f, 1.0f);
		instance.color = particle.params.color;
		instance.textureID = (particle.params.textureID >= 0) ? particle.params.textureID : t_default_;
		instances_.push_back(instance);
	}
	Game::Resource::UpdateData(instanceBufferID_, instances_);
	render_.instanceNum_ = static_cast<uint32_t>(instances_.size());

	render_.SetBRegisterData(0, ShaderType::VertexShader, &perView_);
	render_.SetTRegisterData(0, ShaderType::VertexShader, Game::Resource::GetSRV(instanceBufferID_));
	render_.Draw(rt_ID);
}

Vector3 GoTargetCurving::GetTargetPosition(const Params& params) const
{
	// targetが無いときはtargetOffsetをそのまま目標座標として使う
	if (!params.target) return params.targetOffset;
	return *params.target + params.targetOffset;
}