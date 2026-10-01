#include "MiningPointGaugeUI.h"
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>
#include <algorithm>

namespace
{
	// DotGothic16の、行の上端からベースラインまでの割合(ascent / (ascent - descent) = 1160 / 1448)
	constexpr float kFontAscentRatio = 1160.0f / 1448.0f;
	// レベルの数字の下端を、ゲージの上端からどれだけ重ねるか(ゲージの絵の1px = 4px)
	constexpr float kLevelTextOverlap = 4.0f;
	// 縁取りの太さ(文字サイズ46のときのフォント1ドット分)
	constexpr float kLevelOutlineWidth = 2.0f;
}


MiningPointGaugeUI::MiningPointGaugeUI()
{
	// sprites_[0] : 経験値ゲージ
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/UI/MiningPointGauge.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/Minecraft/UI/MiningPointGauge/MiningPointGauge.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(float(textureData->metadata.width) * 0.5f, float(textureData->metadata.height) * 0.5f, 1.0f);
	sprites_[0].transforms.translate = Vector3(640.0f, 610.0f, 1.0f);

	gaugeParams_.textureIndex = sprites_[0].textureID;
}

MiningPointGaugeUI::~MiningPointGaugeUI()
{}

void MiningPointGaugeUI::Initialize()
{}

void MiningPointGaugeUI::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	if (!miningGauge_) return;

	const int32_t level = miningGauge_->GetLevel();
	const float ratio = miningGauge_->GetGaugeRatio();

	gaugeParams_.fillRatio = std::clamp(ratio, 0.0f, 1.0f);

	// ptが増えた
	const float levelProgress = static_cast<float>(level) + ratio;
	if (levelProgress > prevLevelProgress_)
	{
		Wave& wave = waves_[nextWaveIndex_];
		wave.distance = gaugeParams_.fillRatio + gaugeParams_.waveWidth;
		wave.counter.Initialize(wave.distance / waveSpeed_);
		wave.isActive = true;
		nextWaveIndex_ = (nextWaveIndex_ + 1) % kMaxWaveCount;
	}
	prevLevelProgress_ = levelProgress;

	// 波の先頭の位置
	for (int32_t i = 0; i < kMaxWaveCount; ++i)
	{
		Wave& wave = waves_[i];
		gaugeParams_.waveHeads[i] = -1.0f;
		if (!wave.isActive) continue;

		const float progress = wave.counter.GetProgress();
		if (progress >= 1.0f)
		{
			wave.isActive = false;
			continue;
		}
		gaugeParams_.waveHeads[i] = progress * wave.distance;
	}

	// レベルの文字(0のときは出さない)
	levelText_ = std::to_string(level);
	if (!levelText_.empty())
	{
		const Vector2 textSize = Game::Asset::Font::MeasureJustTextureSize(levelText_, levelCharSize_, Vector2{ 0.0f, 0.0f }, 0.0f);
		const Vector3& gaugePos = sprites_[0].transforms.translate;
		const float gaugeTop = gaugePos.y - sprites_[0].transforms.scale.y;
		const float baselineY = gaugeTop + kLevelTextOverlap;
		levelTextPos_ = Vector2{ gaugePos.x - textSize.x * 0.5f, baselineY - static_cast<float>(levelCharSize_) * kFontAscentRatio };
	}
}

void MiningPointGaugeUI::Draw(int32_t rt_ID)
{
	// ゲージ
	const auto& gauge = sprites_[0];
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(gauge.transforms.scale, gauge.transforms.rotate, gauge.transforms.translate);
	Matrix4x4 wvp = world * orthographic_;

	gauge.render->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
	gauge.render->SetBRegisterData(1, ShaderType::VertexShader, &world);
	gauge.render->SetBRegisterData(0, ShaderType::PixelShader, &gaugeParams_);
	gauge.render->Draw(rt_ID);


	// レベル
	if (levelText_.empty()) return;

	const Vector4 outlineColor = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
	const Vector2 outlineOffsets[] =
	{
		Vector2{ -kLevelOutlineWidth, 0.0f },
		Vector2{ kLevelOutlineWidth, 0.0f },
		Vector2{ 0.0f, -kLevelOutlineWidth },
		Vector2{ 0.0f, kLevelOutlineWidth },
	};
	for (const Vector2& offset : outlineOffsets)
	{
		Game::Asset::Font::DrawString(rt_ID, levelText_, levelCharSize_, levelTextPos_ + offset, outlineColor);
	}
	Game::Asset::Font::DrawString(rt_ID, levelText_, levelCharSize_, levelTextPos_, gaugeParams_.fillColor);
}

void MiningPointGaugeUI::DrawImGui()
{
	ImGui::Begin("MiningPointGaugeUI");

	ImGui::ColorEdit4("Fill Color", &gaugeParams_.fillColor.x);
	ImGui::ColorEdit4("Wave Color", &gaugeParams_.waveColor.x);
	ImGui::DragFloat("Wave Width", &gaugeParams_.waveWidth, 0.01f, 0.01f, 1.0f);
	ImGui::DragFloat("Wave Speed", &waveSpeed_, 0.05f, 0.1f, 10.0f);
	ImGui::DragInt("Level Char Size", &levelCharSize_, 1.0f, 8, 128);

	ImGui::End();
}
