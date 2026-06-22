#include "TestParticle.h"
#include <Game.h>
#include <numbers>

TestParticle::TestParticle()
{
	t_circle2 = Game::Resource::Texture::Load("resources/prototypes/texture/particle/circle2.png");
	t_uvChecker = Game::Resource::Texture::Load("resources/prototypes/texture/uvChecker.png");
	t_gradationLine = Game::Resource::Texture::Load("resources/prototypes/texture/particle/gradationLine.png");

	// 円
	{
		renderPlanes_.resize(10);
		planeColors.resize(10);
		planeTransforms.resize(10);
		for (int i = 0; i < 10; ++i)
		{
			renderPlanes_[i] = std::make_unique<RenderObject>();
			renderPlanes_[i]->modelID_ = Game::Resource::Model::Load("resources/prototypes/model/plane/plane.obj");
			renderPlanes_[i]->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
			renderPlanes_[i]->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
			renderPlanes_[i]->psoConfig_.blendID = BlendStateID::Add;
			renderPlanes_[i]->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
			renderPlanes_[i]->SetupFromShaders();
			planeColors[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
			planeTransforms[i].scale = { 0.5f,6.0f,0.5f };
			planeTransforms[i].translate = { 0.0f, 0.0f, 0.0f };
			planeTransforms[i].rotate = { Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1) };
		}
	}

	// 円柱
	{
		std::vector<VertexData> vertexData_Cylinder;
		const uint32_t kCylinderDivide = 32;
		const float kTopRadius = 1.0f;
		const float kBottomRadius = 1.0f;
		const float kHeight = 3.0f;
		const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kCylinderDivide);

		vertexData_Cylinder.reserve(kCylinderDivide * 6);

		for (uint32_t index = 0; index < kCylinderDivide; ++index)
		{
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);

			float u = float(index) / float(kCylinderDivide);
			float uNext = float(index + 1) / float(kCylinderDivide);

			// 側面の法線は円周方向（XZ平面）
			Vector3 normal1 = { -sin,     0.0f, cos };
			Vector3 normal2 = { -sinNext, 0.0f, cosNext };

			// 4頂点
			VertexData v1{ { -sin * kTopRadius,    kHeight, cos * kTopRadius,    1.0f }, { u,     0.0f }, normal1 };
			VertexData v2{ { -sinNext * kTopRadius,    kHeight, cosNext * kTopRadius,    1.0f }, { uNext, 0.0f }, normal2 };
			VertexData v3{ { -sin * kBottomRadius, 0.0f,    cos * kBottomRadius, 1.0f }, { u,     1.0f }, normal1 };
			VertexData v4{ { -sinNext * kBottomRadius, 0.0f,    cosNext * kBottomRadius, 1.0f }, { uNext, 1.0f }, normal2 };

			// 三角形① (左上 → 右上 → 左下)
			vertexData_Cylinder.push_back(v1);
			vertexData_Cylinder.push_back(v2);
			vertexData_Cylinder.push_back(v3);

			// 三角形② (左下 → 右上 → 右下)
			vertexData_Cylinder.push_back(v3);
			vertexData_Cylinder.push_back(v2);
			vertexData_Cylinder.push_back(v4);
		}

		renderCylinder_ = std::make_unique<RenderObject>();
		renderCylinder_->modelID_ = Game::Resource::Model::Create(vertexData_Cylinder, "Cylinder");
		renderCylinder_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
		renderCylinder_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
		renderCylinder_->psoConfig_.blendID = BlendStateID::Add;
		renderCylinder_->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
		renderCylinder_->SetupFromShaders();
		cylinderColor = Vector4{ 0.1f,1.0f,1.0f,1.0f };
		cylinderTransform.scale = { 1.0f,1.0f,1.0f };
		cylinderTransform.translate = { 0.0f, 0.0f, 0.0f };
		cylinderTransform.rotate = { 0.0f, 0.0f, 3.141592f };
	}

	// リング
	{
		std::vector<VertexData> vertexData_Ring;
		const uint32_t kRingDivide = 32;
		const float kOuterRadius = 1.0f;
		const float kInnerRadius = 0.2f;
		const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

		for (uint32_t index = 0; index < kRingDivide; ++index)
		{
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);
			float u = float(index) / float(kRingDivide);
			float uNext = float(index + 1) / float(kRingDivide);

			Vector3 normal = { 0.0f, 0.0f, 1.0f };

			// 4頂点
			VertexData v1{ { -sin * kOuterRadius,     cos * kOuterRadius,     0.0f, 1.0f }, { u,     0.0f }, normal };
			VertexData v2{ { -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f }, { uNext, 0.0f }, normal };
			VertexData v3{ { -sin * kInnerRadius,     cos * kInnerRadius,     0.0f, 1.0f }, { u,     1.0f }, normal };
			VertexData v4{ { -sinNext * kInnerRadius, cosNext * kInnerRadius, 0.0f, 1.0f }, { uNext, 1.0f }, normal };

			// 三角形① (左上 → 右上 → 左下)
			vertexData_Ring.push_back(v1);
			vertexData_Ring.push_back(v2);
			vertexData_Ring.push_back(v3);

			// 三角形② (左下 → 右上 → 右下)
			vertexData_Ring.push_back(v3);
			vertexData_Ring.push_back(v2);
			vertexData_Ring.push_back(v4);
		}

		renderRings_.resize(10);
		ringColors.resize(10);
		ringTransforms.resize(10);
		int32_t ringModelID = Game::Resource::Model::Create(vertexData_Ring, "Ring");
		for (int i = 0; i < 10; ++i)
		{
			renderRings_[i] = std::make_unique<RenderObject>();
			renderRings_[i]->modelID_ = ringModelID;
			renderRings_[i]->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
			renderRings_[i]->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
			renderRings_[i]->psoConfig_.blendID = BlendStateID::Add;
			renderRings_[i]->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
			renderRings_[i]->SetupFromShaders();
			ringColors[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
			ringTransforms[i].scale = { 6.0f,6.0f,6.0f };
			ringTransforms[i].translate = { 0.0f, 0.0f, 0.0f };
			ringTransforms[i].rotate = { Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1) };
		}
	}

	// 球
	{
		std::vector<VertexData> vertexData_Sphere;
		const uint32_t kSlices = 32;   // 経度分割
		const uint32_t kStacks = 16;   // 緯度分割
		const float kRadius = 1.0f;

		vertexData_Sphere.reserve(kSlices * kStacks * 6);

		const float PI = std::numbers::pi_v<float>;
		for (uint32_t stack = 0; stack < kStacks; ++stack)
		{
			// theta: 0..PI (緯度)
			float theta = (float)stack / (float)kStacks * PI;
			float thetaNext = (float)(stack + 1) / (float)kStacks * PI;

			float sinTheta = std::sin(theta);
			float cosTheta = std::cos(theta);
			float sinThetaNext = std::sin(thetaNext);
			float cosThetaNext = std::cos(thetaNext);

			for (uint32_t slice = 0; slice < kSlices; ++slice)
			{
				// phi: 0..2PI (経度)
				float phi = (float)slice / (float)kSlices * 2.0f * PI;
				float phiNext = (float)(slice + 1) / (float)kSlices * 2.0f * PI;

				float sinPhi = std::sin(phi);
				float cosPhi = std::cos(phi);
				float sinPhiNext = std::sin(phiNext);
				float cosPhiNext = std::cos(phiNext);

				// 4頂点（左上, 右上, 左下, 右下）
				Vector3 p1 = { sinTheta * cosPhi * kRadius,  cosTheta * kRadius, sinTheta * sinPhi * kRadius };
				Vector3 p2 = { sinTheta * cosPhiNext * kRadius,  cosTheta * kRadius, sinTheta * sinPhiNext * kRadius };
				Vector3 p3 = { sinThetaNext * cosPhi * kRadius, cosThetaNext * kRadius, sinThetaNext * sinPhi * kRadius };
				Vector3 p4 = { sinThetaNext * cosPhiNext * kRadius, cosThetaNext * kRadius, sinThetaNext * sinPhiNext * kRadius };

				// UV
				float u = (float)slice / (float)kSlices;
				float uNext = (float)(slice + 1) / (float)kSlices;
				float v = (float)stack / (float)kStacks;
				float vNext = (float)(stack + 1) / (float)kStacks;

				VertexData vtx1{ { p1.x, p1.y, p1.z, 1.0f }, { u,     v },     p1.Normalized() };
				VertexData vtx2{ { p2.x, p2.y, p2.z, 1.0f }, { uNext, v },     p2.Normalized() };
				VertexData vtx3{ { p3.x, p3.y, p3.z, 1.0f }, { u,     vNext },  p3.Normalized() };
				VertexData vtx4{ { p4.x, p4.y, p4.z, 1.0f }, { uNext, vNext },  p4.Normalized() };

				// 三角形① (左上 → 右上 → 左下)
				vertexData_Sphere.push_back(vtx1);
				vertexData_Sphere.push_back(vtx2);
				vertexData_Sphere.push_back(vtx3);

				// 三角形② (左下 → 右上 → 右下)
				vertexData_Sphere.push_back(vtx3);
				vertexData_Sphere.push_back(vtx2);
				vertexData_Sphere.push_back(vtx4);
			}
		}

		renderSphere_ = std::make_unique<RenderObject>();
		renderSphere_->modelID_ = Game::Resource::Model::Create(vertexData_Sphere, "Sphere");
		renderSphere_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
		renderSphere_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
		renderSphere_->psoConfig_.blendID = BlendStateID::Add;
		renderSphere_->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
		renderSphere_->SetupFromShaders();
		sphereColor = Vector4{ 1.0f,1.0f,1.0f,1.0f };
		sphereTransform.scale = { 6.0f,6.0f,6.0f };
		sphereTransform.translate = { 0.0f, 0.0f, 0.0f };
		sphereTransform.rotate = { Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1) };
	}
}

TestParticle::~TestParticle()
{}

void TestParticle::Initialize()
{}

void TestParticle::Update(int32_t cameraID)
{
	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	for (int i = 0; i < 10; ++i)
	{
		planeColors[i].w -= 0.02f;
		if (Game::Math::Rand::RandInt(0, 50) == i)
		{
			planeColors[i].w = 1.0f;
		}

		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(planeTransforms[i].scale, planeTransforms[i].rotate, planeTransforms[i].translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderPlanes_[i]->SetCBufferData(0, ShaderType::PixelShader, &planeColors[i]);
		renderPlanes_[i]->SetCBufferData(1, ShaderType::PixelShader, &t_circle2);
		renderPlanes_[i]->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderPlanes_[i]->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}

	for (int i = 0; i < 10; ++i)
	{
		ringColors[i].w -= 0.02f;
		if (Game::Math::Rand::RandInt(0, 50) == i)
		{
			ringColors[i].w = 1.0f;
		}
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(ringTransforms[i].scale, ringTransforms[i].rotate, ringTransforms[i].translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderRings_[i]->SetCBufferData(0, ShaderType::PixelShader, &ringColors[i]);
		renderRings_[i]->SetCBufferData(1, ShaderType::PixelShader, &t_gradationLine);
		renderRings_[i]->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderRings_[i]->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}

	{
		cylinderTransform.rotate.y += 0.002f;
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(cylinderTransform.scale, cylinderTransform.rotate, cylinderTransform.translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderCylinder_->SetCBufferData(0, ShaderType::PixelShader, &cylinderColor);
		renderCylinder_->SetCBufferData(1, ShaderType::PixelShader, &t_gradationLine);
		renderCylinder_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderCylinder_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}

	{
		sphereTransform.rotate.y += 0.002f;
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(sphereTransform.scale, sphereTransform.rotate, sphereTransform.translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderSphere_->SetCBufferData(0, ShaderType::PixelShader, &sphereColor);
		renderSphere_->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
		renderSphere_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderSphere_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}
}

void TestParticle::Draw(int32_t renderTextureID)
{
	for (int i = 0; i < 10; ++i)
	{
		renderPlanes_[i]->Draw(renderTextureID);
	}
	for (int i = 0; i < 10; ++i)
	{
		renderRings_[i]->Draw(renderTextureID);
	}
	renderCylinder_->Draw(renderTextureID);
	renderSphere_->Draw(renderTextureID);
}
