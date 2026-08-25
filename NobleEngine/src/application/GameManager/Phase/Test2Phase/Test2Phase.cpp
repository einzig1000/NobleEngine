#include "Test2Phase.h"
#include <Game.h>

Test2Phase::Test2Phase()
{
	c_main_ = Game::Camera::AddCamera();
	Game::Camera::Setter::SetDistance(5.0f, 0, EaseType::IN_BACK, c_main_);
	Game::Camera::Setter::SetEnableControl(true, c_main_);

	t_dissolveMaskTexture_ = Game::Asset::Texture::Load("assets/engine/texture/noise0.png");

	rt_main_ = Game::Asset::RenderTexture::CreateRenderTexture(1280, 720, "main");
	rt_noise_ = Game::Asset::RenderTexture::CreateRenderTexture(1280, 720, "noise");

	render_ = std::make_unique<RenderObject>();
	//render_->psoConfig_.ps = "assets/shaders/FullScreen/LuminanceBasedOutline.PS.hlsl";
	render_->psoConfig_.ps = "assets/shaders/FullScreen/Dissolve.PS.hlsl";
	//render_->psoConfig_.ps = "assets/shaders/FullScreen/RandomNoise.PS.hlsl";
	render_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	render_->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	render_->SetupFromShaders();

	weapon_ = std::make_unique<RenderObject>();
	weapon_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	weapon_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	weapon_->modelID_ = Game::Asset::Model::Load("assets/application/Minecraft/Item/tool/sword/sword.obj");
	weapon_->SetupFromShaders();
}

Test2Phase::~Test2Phase()
{}

void Test2Phase::Initialize()
{
	testParticle.Initialize();
	testAnimation.Initialize();
}

void Test2Phase::Update()
{
	Game::Camera::Update(c_main_);

	ImGui::Begin("Test2Phase");

	ImGui::DragFloat3("scale", &scale_.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
	ImGui::DragFloat3("translate", &translate_.x, 0.01f);
	ImGui::End();

	testAnimation.Update(c_main_);
	//Vector3 pos = testAnimation.GetVetexPos(index);
	Vector3 pos = testAnimation.GetVetexPos(testAnimation.GetVertexIndexByJointName("mixamorig:LeftHandMiddle1"));
	testParticle.SetEmitterSpherePos(pos);
	testParticle.Update(c_main_);


	Matrix4x4 handMatrix = testAnimation.GetJointMatrixByName("mixamorig:LeftHandMiddle1");

	Matrix4x4 weaponLocalOffset_ = Matrix4x4::MakeAffineMatrix(scale_, rotate_, translate_);
	Matrix4x4 weaponWorldMatrix = weaponLocalOffset_ * handMatrix;
	Matrix4x4 weaponWVP = weaponWorldMatrix * Game::Camera::Getter::GetViewProjectionMatrix(c_main_);

	Vector4 weaponColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	int32_t weaponTexID = 1;

	weapon_->SetCBufferData(0, ShaderType::PixelShader, &weaponColor);
	weapon_->SetCBufferData(1, ShaderType::PixelShader, &weaponTexID);
	weapon_->SetCBufferData(0, ShaderType::VertexShader, &weaponWVP);
	weapon_->SetCBufferData(1, ShaderType::VertexShader, &weaponWorldMatrix);

	if (Game::IO::Key::IsJustPressed(VK_F11))
	{
		Game::Asset::RenderTexture::SaveAllRenderTextureToFile("screenshots");
	}

	frame_ += 1.0f;

	// Dissolveの時
	render_->SetCBufferData(0, ShaderType::PixelShader, &rt_main_);
	render_->SetCBufferData(1, ShaderType::PixelShader, &t_dissolveMaskTexture_);
	render_->SetCBufferData(2, ShaderType::PixelShader, &threshold);
	Vector3 edgeColor = Vector3(1.0f, 0.0f, 0.0f);
	render_->SetCBufferData(3, ShaderType::PixelShader, &edgeColor);

	// RandomNoiseの時
	//render_->SetCBufferData(0, ShaderType::PixelShader, &frame_);
}

void Test2Phase::Draw()
{
	testParticle.Draw(rt_main_);
	testAnimation.Draw(rt_main_);
	weapon_->Draw(rt_main_);

	render_->Draw();
}

void Test2Phase::DrawImGui()
{
	ImGui::Begin("Test2Phase"); 
	ImGui::DragFloat("Threshold", &threshold, 0.01f, 0.0f, 1.0f);
	ImGui::End();
}
