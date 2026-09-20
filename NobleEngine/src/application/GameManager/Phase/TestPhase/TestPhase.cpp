#include "TestPhase.h"
#include <Utilities/Json/JsonManager.h>
#include <Utilities/functions.h>
#include <externals/MagicEnum/magic_enum.hpp>

namespace
{
	/// <summary>
	/// グラス内液体のメッシュを生成する。
	/// 天面は同心リング状に密に分割し、側面は波の追従範囲が滑らかになるよう縦にも分割する。
	/// サイズは単位(半径1.0・高さ1.0)で作り、実寸へはワールド行列のScaleで合わせる。
	/// </summary>
	/// <param name="bottomRadius">底の半径。テーパーしたグラスならtopRadiusより小さくする</param>
	/// <param name="topRadius">液面の半径</param>
	/// <param name="height">底(y=0)から液面までの高さ</param>
	/// <param name="angularSegments">円周方向の分割数</param>
	/// <param name="radialSegments">天面の半径方向の分割数</param>
	/// <param name="heightSegments">側面の縦方向の分割数</param>
	std::vector<VertexData> CreateLiquidMeshVertices(
		float bottomRadius,
		float topRadius,
		float height,
		uint32_t angularSegments,
		uint32_t radialSegments,
		uint32_t heightSegments)
	{
		const float kPi = 3.14159265358979323846f;
		const float topY = height;

		std::vector<VertexData> vertices;
		vertices.reserve(size_t(angularSegments) * (radialSegments * 6 + heightSegments * 6 + 3));

		auto MakeVertex = [](const Vector3& position, const Vector2& texcoord, const Vector3& normal)
			{
				VertexData v{};
				v.position = Vector4{ position.x, position.y, position.z, 1.0f };
				v.texcoord = texcoord;
				v.normal = normal;
				return v;
			};

		// ------------------------------------------------------------
		// 天面
		// 波で一番動く面なので、ここだけ密に分割する。法線は +Y 固定。
		// ------------------------------------------------------------
		{
			const Vector3 topNormal{ 0.0f, 1.0f, 0.0f };

			auto TopUV = [&](const Vector3& p)
				{
					return Vector2{ p.x / (topRadius * 2.0f) + 0.5f, p.z / (topRadius * 2.0f) + 0.5f };
				};

			for (uint32_t ring = 0; ring < radialSegments; ++ring)
			{
				const float innerRadius = topRadius * (float(ring) / float(radialSegments));
				const float outerRadius = topRadius * (float(ring + 1) / float(radialSegments));

				for (uint32_t seg = 0; seg < angularSegments; ++seg)
				{
					const float theta0 = 2.0f * kPi * (float(seg) / float(angularSegments));
					const float theta1 = 2.0f * kPi * (float(seg + 1) / float(angularSegments));

					const float c0 = std::cos(theta0), s0 = std::sin(theta0);
					const float c1 = std::cos(theta1), s1 = std::sin(theta1);

					const Vector3 innerA{ c0 * innerRadius, topY, s0 * innerRadius };
					const Vector3 innerB{ c1 * innerRadius, topY, s1 * innerRadius };
					const Vector3 outerB{ c1 * outerRadius, topY, s1 * outerRadius };
					const Vector3 outerA{ c0 * outerRadius, topY, s0 * outerRadius };

					// 最内リングは innerA と innerB が中心点に退化するので三角形1枚だけ出す
					if (ring > 0)
					{
						vertices.push_back(MakeVertex(innerA, TopUV(innerA), topNormal));
						vertices.push_back(MakeVertex(innerB, TopUV(innerB), topNormal));
						vertices.push_back(MakeVertex(outerB, TopUV(outerB), topNormal));
					}

					vertices.push_back(MakeVertex(innerA, TopUV(innerA), topNormal));
					vertices.push_back(MakeVertex(outerB, TopUV(outerB), topNormal));
					vertices.push_back(MakeVertex(outerA, TopUV(outerA), topNormal));
				}
			}
		}

		// ------------------------------------------------------------
		// 側面
		// 天面直下が SideWaveDepth の範囲で波に追従するので、縦にも割っておく。
		// ------------------------------------------------------------
		{
			const float deltaRadius = topRadius - bottomRadius;

			// テーパーを考慮した外向き法線
			auto SideNormal = [&](float c, float s)
				{
					Vector3 n{ c * height, -deltaRadius, s * height };
					const float length = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
					return Vector3{ n.x / length, n.y / length, n.z / length };
				};

			for (uint32_t level = 0; level < heightSegments; ++level)
			{
				const float v0 = float(level) / float(heightSegments);
				const float v1 = float(level + 1) / float(heightSegments);

				const float y0 = height * v0;
				const float y1 = height * v1;
				const float r0 = bottomRadius + deltaRadius * v0;
				const float r1 = bottomRadius + deltaRadius * v1;

				for (uint32_t seg = 0; seg < angularSegments; ++seg)
				{
					const float u0 = float(seg) / float(angularSegments);
					const float u1 = float(seg + 1) / float(angularSegments);

					const float theta0 = 2.0f * kPi * u0;
					const float theta1 = 2.0f * kPi * u1;

					const float c0 = std::cos(theta0), s0 = std::sin(theta0);
					const float c1 = std::cos(theta1), s1 = std::sin(theta1);

					const Vector3 normal0 = SideNormal(c0, s0);
					const Vector3 normal1 = SideNormal(c1, s1);

					const Vector3 upper0{ c0 * r1, y1, s0 * r1 };
					const Vector3 upper1{ c1 * r1, y1, s1 * r1 };
					const Vector3 lower0{ c0 * r0, y0, s0 * r0 };
					const Vector3 lower1{ c1 * r0, y0, s1 * r0 };

					vertices.push_back(MakeVertex(upper0, Vector2{ u0, 1.0f - v1 }, normal0));
					vertices.push_back(MakeVertex(lower1, Vector2{ u1, 1.0f - v0 }, normal1));
					vertices.push_back(MakeVertex(lower0, Vector2{ u0, 1.0f - v0 }, normal0));

					vertices.push_back(MakeVertex(upper0, Vector2{ u0, 1.0f - v1 }, normal0));
					vertices.push_back(MakeVertex(upper1, Vector2{ u1, 1.0f - v1 }, normal1));
					vertices.push_back(MakeVertex(lower1, Vector2{ u1, 1.0f - v0 }, normal1));
				}
			}
		}

		// ------------------------------------------------------------
		// 底面
		// 半透明なので下から覗くと抜けて見える。粗くていいので塞いでおく。
		// ------------------------------------------------------------
		{
			const Vector3 bottomNormal{ 0.0f, -1.0f, 0.0f };
			const Vector3 center{ 0.0f, 0.0f, 0.0f };

			for (uint32_t seg = 0; seg < angularSegments; ++seg)
			{
				const float theta0 = 2.0f * kPi * (float(seg) / float(angularSegments));
				const float theta1 = 2.0f * kPi * (float(seg + 1) / float(angularSegments));

				const Vector3 p0{ std::cos(theta0) * bottomRadius, 0.0f, std::sin(theta0) * bottomRadius };
				const Vector3 p1{ std::cos(theta1) * bottomRadius, 0.0f, std::sin(theta1) * bottomRadius };

				vertices.push_back(MakeVertex(center, Vector2{ 0.5f, 0.5f }, bottomNormal));
				vertices.push_back(MakeVertex(p0, Vector2{ 0.0f, 0.0f }, bottomNormal));
				vertices.push_back(MakeVertex(p1, Vector2{ 1.0f, 0.0f }, bottomNormal));
			}
		}

		return vertices;
	}
}

TestPhase::TestPhase()
{
	// カメラ
	c_main_ = Game::Camera::AddCamera("SimpleModels");
	// モデル
	m_cube_ = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");
	m_plane_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	m_human_ = Game::Asset::Model::Load("assets/engine/model/human/sneakWalk.gltf");
	m_water_ = Game::Asset::Model::Create(CreateLiquidMeshVertices(0.5f, 0.5f, 1.0f, 32, 32, 32), "Water");
	// テクスチャ
	t_monsterBall_ = Game::Asset::Texture::Load("assets/engine/texture/monsterBall.png");
	t_uvChecker_ = Game::Asset::Texture::Load("assets/engine/texture/uvChecker.png");
	// アニメーション
	a_sneakWalk_ = Game::Asset::Animation::Load("assets/engine/model/human/sneakWalk.gltf", "sneakWalk");
	// サウンド
	s_aura_ = Game::Asset::Audio::Load("assets/application/audio/SE/よく聞くやつ/歓声と拍手.mp3");
}

TestPhase::~TestPhase()
{}

void TestPhase::Initialize()
{
	// フェーズ初期化
	nextPhase_ = Phase::Phase_None;

	//Initialize_SimpleModels();
	Initialize_Sprite();
	//Initialize_Animation();
	Initialize_Water();

	particle_.Initialize();
}

void TestPhase::Update()
{
	Game::Camera::Update(c_main_);

	//Update_SimpleModels();
	Update_Sprite();
	//Update_Animation();
	Update_Water();

	particle_.Update(c_main_);
}

void TestPhase::Draw()
{
	//Draw_SimpleModels();
	Draw_Sprite();
	//Draw_Animation();
	Draw_Water();
	particle_.Draw();
}

void TestPhase::DrawImGui()
{
	DrawImGui_SimpleModels();
	DrawImGui_Sprite();
	DrawImGui_Animation();
	DrawImGui_Water();


	if (ImGui::Button("PlaySound"))
	{
		Game::Asset::Audio::PlayAudio(s_aura_, false);
	}
}



void TestPhase::Initialize_SimpleModels()
{
	simpleModels_ = std::make_unique<RenderObject>();
	simpleModels_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModels.VS.hlsl";
	simpleModels_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModels.PS.hlsl";
	simpleModels_->SetupFromShaders();
	simpleModels_->modelID_ = m_cube_;
	simpleModels_->instanceNum_ = instanceCount_;

	worldMatrixHeapSlot_ = Game::Resource::CreateDynamic();
	colorHeapSlot_ = Game::Resource::CreateDynamic();
	textureIndexHeapSlot_ = Game::Resource::CreateDynamic();

	transforms_.resize(instanceCount_, EulerTransforms());
	worldMatrices_.resize(instanceCount_, Matrix4x4());
	colors_.resize(instanceCount_, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	textureIndices_.resize(instanceCount_, t_uvChecker_);
}

void TestPhase::Update_SimpleModels()
{
	for (int i = 0; i < instanceCount_; i++)
	{
		worldMatrices_[i] = transforms_[i].GetWorldMatrix();
	}

	Game::Resource::UpdateData(worldMatrixHeapSlot_, worldMatrices_);
	Game::Resource::UpdateData(colorHeapSlot_, colors_);
	Game::Resource::UpdateData(textureIndexHeapSlot_, textureIndices_);

	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(c_main_);
	int32_t vsHeapSlot = Game::Resource::GetSRV(worldMatrixHeapSlot_);
	Vector2uint psHeapSlot{ Game::Resource::GetSRV(colorHeapSlot_), Game::Resource::GetSRV(textureIndexHeapSlot_) };

	simpleModels_->SetBRegisterData(0, ShaderType::VertexShader, &viewProjection);
	simpleModels_->SetBRegisterData(1, ShaderType::VertexShader, &vsHeapSlot);
	simpleModels_->SetBRegisterData(0, ShaderType::PixelShader, &psHeapSlot);
}

void TestPhase::Draw_SimpleModels()
{
	simpleModels_->Draw();
}

void TestPhase::DrawImGui_SimpleModels()
{
	ImGui::Begin("SimpleModelsTest");

	for (int i = 0; i < instanceCount_; i++)
	{
		if (ImGui::TreeNode(("Instance " + std::to_string(i)).c_str()))
		{
			ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(), &transforms_[i].scale.x, 0.01f);
			ImGui::DragFloat3(("Rotate##" + std::to_string(i)).c_str(), &transforms_[i].rotate.x, 0.01f);
			ImGui::DragFloat3(("Translate##" + std::to_string(i)).c_str(), &transforms_[i].translate.x, 0.01f);
			ImGui::ColorEdit4(("Color##" + std::to_string(i)).c_str(), &colors_[i].x);
			const char* textureNames[] = { "uvChecker", "monsterBall" };
			int currentTextureIndex = textureIndices_[i];
			if (ImGui::Combo(("Texture##" + std::to_string(i)).c_str(), &currentTextureIndex, textureNames, IM_ARRAYSIZE(textureNames)))
			{
				textureIndices_[i] = currentTextureIndex == 0 ? t_uvChecker_ : t_monsterBall_;
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}



void TestPhase::Initialize_Sprite()
{
	sprite_ = std::make_unique<RenderObject>();
	sprite_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprite_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprite_->SetupFromShaders();
	sprite_->modelID_ = m_plane_;
	spriteTexture_ = t_uvChecker_;

	spriteTransform_.scale = Vector3(128.0f, 128.0f, 0.0f);
	spriteTransform_.translate = Vector3(140.0f, 140.0f, 0.0f);
}

void TestPhase::Update_Sprite()
{
	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(c_main_);
	Matrix4x4 orthographicMatrix = Game::Camera::Getter::GetOrthoProjectionMatrix(c_main_);
	Matrix4x4 worldMatrix = spriteTransform_.GetWorldMatrix();
	Matrix4x4 wvp = worldMatrix * orthographicMatrix;
	Vector4 color = spriteColor_;

	sprite_->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
	sprite_->SetBRegisterData(1, ShaderType::VertexShader, &worldMatrix);
	sprite_->SetBRegisterData(0, ShaderType::PixelShader, &color);
	sprite_->SetBRegisterData(1, ShaderType::PixelShader, &spriteTexture_);
}

void TestPhase::Draw_Sprite()
{
	sprite_->Draw();
}

void TestPhase::DrawImGui_Sprite()
{
	ImGui::Begin("SpriteTest");
	ImGui::DragFloat3("Scale", &spriteTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &spriteTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Translate", &spriteTransform_.translate.x, 0.01f);
	ImGui::ColorEdit4("Color", &spriteColor_.x);
	const char* textureNames[] = { "uvChecker", "monsterBall" };
	int currentTextureIndex = (spriteTexture_ == t_uvChecker_) ? 0 : 1;
	if (ImGui::Combo("Texture", &currentTextureIndex, textureNames, IM_ARRAYSIZE(textureNames)))
	{
		spriteTexture_ = (currentTextureIndex == 0) ? t_uvChecker_ : t_monsterBall_;
	}
	ImGui::End();
}



void TestPhase::Initialize_Animation()
{
	modelData_ = Game::Asset::Model::GetData(m_human_);
	numVertices_ = static_cast<uint32_t>(modelData_->vertices.size());
	skinInstance_ = Game::Asset::Animation::CreateSkinInstance(m_human_);
	resultHeapSlot_ = Game::Resource::CreateCompute(sizeof(VertexData), numVertices_);


	animation_ = std::make_unique<RenderObject>();
	animation_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModelNonIASet.VS.hlsl";
	animation_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	animation_->SetupFromShaders();
	animation_->modelID_ = m_human_;

	animationCompute_ = std::make_unique<ComputeObject>();
	animationCompute_->psoConfig_.cs = "assets/shaders/Skinning/Skinning.CS.hlsl";
	animationCompute_->SetupFromShaders();
	animationCompute_->RegisterOutput(resultHeapSlot_);
	animationCompute_->size.x = int32_t((numVertices_ + 1023) / 1024);


	const Vector4    color = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Matrix4x4  world = Matrix4x4::MakeIdentity4x4();
	animation_->SetBRegisterData(0, ShaderType::PixelShader, &color);
	animation_->SetBRegisterData(1, ShaderType::PixelShader, &t_uvChecker_);
	animation_->SetTRegisterData(0, ShaderType::VertexShader, Game::Resource::GetSRV(resultHeapSlot_));

	animationCompute_->SetBRegisterData(0, &numVertices_);
	animationCompute_->SetTRegisterData(1, modelData_->vertexHeapSlot);
	animationCompute_->SetTRegisterData(2, modelData_->skinBindData.influenceHeapSlot);
	animationCompute_->SetURegisterData(0, Game::Resource::GetUAV(resultHeapSlot_));
}

void TestPhase::Update_Animation()
{
	animationTime_ += Game::Time::GetScaledDeltaTimeMs() * 0.001f;

	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(c_main_);
	Matrix4x4 world = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 wvp = world * viewProjection;

	Game::Asset::Animation::ComputeAnimationData(a_sneakWalk_, skinInstance_, modelData_->skinBindData, animationTime_);
	Game::Resource::UpdateData(skinInstance_.paletteHandle, skinInstance_.palette);

	animation_->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
	animation_->SetBRegisterData(1, ShaderType::VertexShader, &world);

	animationCompute_->SetTRegisterData(0, Game::Resource::GetSRV(skinInstance_.paletteHandle));
}

void TestPhase::Draw_Animation()
{
	animation_->Draw();
	animationCompute_->Dispatch();
}

void TestPhase::DrawImGui_Animation()
{
	ImGui::Begin("AnimationTest");
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f);
	ImGui::End();
}



void TestPhase::Initialize_Water()
{
	water_ = std::make_unique<RenderObject>();
	water_->psoConfig_.vs = "assets/shaders/WaterSurface/WaterSurface.VS.hlsl";
	water_->psoConfig_.ps = "assets/shaders/WaterSurface/WaterSurface.PS.hlsl";
	water_->psoConfig_.blendID = BlendStateID::Alpha;
	water_->psoConfig_.depthStencilID = DepthStencilID::Default;
	water_->psoConfig_.rasterizerID = RasterizerID::Solid_BackCull;
	water_->modelID_ = m_water_;
	water_->SetupFromShaders();
}

void TestPhase::Update_Water()
{
	waterTransformCB_.world = waterTransform_.GetWorldMatrix();
	waterTransformCB_.worldInverseTranspose = waterTransformCB_.world.Inverse().Transpose();
	waterTransformCB_.viewProjection = Game::Camera::Getter::GetViewMatrix(c_main_)
		* Game::Camera::Getter::GetProjectionMatrix(c_main_);

	// 液面はモデルローカルの天面Yをワールドへ変換して求める
	const float localSurfaceY = 1.0f; // メッシュの天面のローカルY
	waterWaveCB_.surfaceY = waterTransformCB_.world.m[1][1] * localSurfaceY + waterTransformCB_.world.m[3][1];

	waterWaveCB_.motionWaveTime += Game::Time::GetScaledDeltaTimeMs() * 0.001f;
	//waterWaveCB_.motionIntensity = motionIntensity_;   // グラスの速度から作った0〜1
	waterWaveCB_.motionIntensity = 0.5f; // 仮の値

	waterColorCB_.motionWaveTime = waterWaveCB_.motionWaveTime;
	waterColorCB_.motionIntensity = waterWaveCB_.motionIntensity;
	waterColorCB_.mixProgress = 0.1f;

	waterCameraCB_.cameraPositionWS = Game::Camera::Getter::GetWorldPosition(c_main_);

	water_->SetBRegisterData(0, ShaderType::VertexShader, &waterTransformCB_);
	water_->SetBRegisterData(1, ShaderType::VertexShader, &waterWaveCB_);
	water_->SetBRegisterData(0, ShaderType::PixelShader, &waterCameraCB_);
	water_->SetBRegisterData(1, ShaderType::PixelShader, &waterColorCB_);
	water_->SetBRegisterData(2, ShaderType::PixelShader, &waterLightingCB_);
}

void TestPhase::Draw_Water()
{
	water_->Draw();
}

void TestPhase::DrawImGui_Water()
{
	ImGui::Begin("WaterTest");

	if (ImGui::TreeNode("Transform"))
	{
		ImGui::DragFloat3("Scale", &waterTransform_.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate", &waterTransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Translate", &waterTransform_.translate.x, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Wave"))
	{
		ImGui::DragFloat3("RelativeScale", &waterWaveCB_.relativeScale.x, 0.01f);
		ImGui::DragFloat("SurfaceY", &waterWaveCB_.surfaceY, 0.01f);
		ImGui::DragFloat("CommonWorldScale", &waterWaveCB_.commonWorldScale, 0.01f);
		ImGui::DragFloat("SideWaveDepth", &waterWaveCB_.sideWaveDepth, 0.001f);
		ImGui::DragFloat("WaveHeight", &waterWaveCB_.waveHeight, 0.001f);
		ImGui::DragFloat("WaveFrequency", &waterWaveCB_.waveFrequency, 0.1f);
		ImGui::DragFloat("WaveSpeed", &waterWaveCB_.waveSpeed, 0.01f);
		ImGui::DragFloat("MotionHeightBoost", &waterWaveCB_.motionHeightBoost, 0.01f);
		ImGui::DragFloat("MotionIntensity", &waterWaveCB_.motionIntensity, 0.01f);
		ImGui::DragFloat("MotionWaveTime", &waterWaveCB_.motionWaveTime, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("color"))
	{
		ImGui::ColorEdit4("colorA", &waterColorCB_.colorA.x);
		ImGui::ColorEdit4("colorB", &waterColorCB_.colorB.x);
		ImGui::ColorEdit4("baseColor", &waterColorCB_.baseColor.x);
		ImGui::DragFloat("colorBalance", &waterColorCB_.colorBalance, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("colorBlendWidth", &waterColorCB_.colorBlendWidth, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("colorDistortion", &waterColorCB_.colorDistortion, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("colorPatternScale", &waterColorCB_.colorPatternScale, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("convectionSpeed", &waterColorCB_.convectionSpeed, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("convectionStrength", &waterColorCB_.convectionStrength, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("convectionScale", &waterColorCB_.convectionScale, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("mixProgress", &waterColorCB_.mixProgress, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("motionIntensity", &waterColorCB_.motionIntensity, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("motionWaveTime", &waterColorCB_.motionWaveTime, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("smoothness", &waterColorCB_.smoothness, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("fresnelStrength", &waterColorCB_.fresnelStrength, 0.01f, 0.0f, 1.0f);

		ImGui::TreePop();
	}


	ImGui::End();
}
