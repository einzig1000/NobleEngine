#include "GameManager/Phase/TestPhase/TestPhase.h"
#include <Utilities/Json/JsonManager.h>

TestPhase::TestPhase()
{
	Load_renderTarget();

	Load_camera();

	Load_renderObject();

	// カメラ
	//c_worldView_ = Game::Camera::AddCamera();
	//Game::Camera::Setter::SetScreenSize(Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight())), 0, EaseType::IN_BACK, c_worldView_);
	//c_main1_ = Game::Camera::AddCamera();
	//Game::Camera::Setter::SetScreenSize(Vector2(float(Game::Window::GetWidth() / 2.0f), float(Game::Window::GetHeight())), 0, EaseType::IN_BACK, c_main1_);
	//c_main2_ = Game::Camera::AddCamera();
	//Game::Camera::Setter::SetScreenSize(Vector2(float(Game::Window::GetWidth() / 2.0f), float(Game::Window::GetHeight())), 0, EaseType::IN_BACK, c_main2_);
	//c_miniMap1_ = Game::Camera::AddCamera();
	//Game::Camera::Setter::SetScreenSize(Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight())), 0, EaseType::IN_BACK, c_miniMap1_);
	//c_miniMap2_ = Game::Camera::AddCamera();
	//Game::Camera::Setter::SetScreenSize(Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight())), 0, EaseType::IN_BACK, c_miniMap2_);

	//// 加工前レンダーテクスチャ
	//renderTargetSizes_["main1"] = Vector2(float(Game::Window::GetWidth() / 2.0f), float(Game::Window::GetHeight()));
	//renderTargetNames_["main1"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["main1"].x), uint32_t(renderTargetSizes_["main1"].y), "Main1_");
	//rt_main1_depth_ = Game::Asset::GetRenderTextureDepthID("Main1_");
	//renderTargetSizes_["main2"] = Vector2(float(Game::Window::GetWidth() / 2.0f), float(Game::Window::GetHeight()));
	//renderTargetNames_["main2"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["main2"].x), uint32_t(renderTargetSizes_["main2"].y), "Main2_");
	//rt_main2_depth_ = Game::Asset::GetRenderTextureDepthID("Main2_");
	//renderTargetSizes_["miniMap1"] = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));
	//renderTargetNames_["miniMap1"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["miniMap1"].x), uint32_t(renderTargetSizes_["miniMap1"].y), "MiniMap1_");
	//rt_miniMap1_depth_ = Game::Asset::GetRenderTextureDepthID("MiniMap1_");
	//renderTargetSizes_["miniMap2"] = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));
	//renderTargetNames_["miniMap2"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["miniMap2"].x), uint32_t(renderTargetSizes_["miniMap2"].y), "MiniMap2_");
	//rt_miniMap2_depth_ = Game::Asset::GetRenderTextureDepthID("MiniMap2_");

	//// 加工後レンダーテクスチャ
	//renderTargetSizes_["postEffect1"] = Vector2(float(Game::Window::GetWidth() / 2.0f), float(Game::Window::GetHeight()));
	//renderTargetNames_["postEffect1"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["postEffect1"].x), uint32_t(renderTargetSizes_["postEffect1"].y), "postEffect1");
	//
	//renderTargetSizes_["postEffect2"] = Vector2(float(Game::Window::GetWidth() / 2.0f), float(Game::Window::GetHeight()));
	//renderTargetNames_["postEffect2"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["postEffect2"].x), uint32_t(renderTargetSizes_["postEffect2"].y), "postEffect2");
	//
	//renderTargetSizes_["postEffect3"] = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));
	//renderTargetNames_["postEffect3"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["postEffect3"].x), uint32_t(renderTargetSizes_["postEffect3"].y), "postEffect3");
	//renderTargetSizes_["postEffect4"] = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));
	//renderTargetNames_["postEffect4"] = Game::Asset::RenderTexture::CreateRenderTexture(uint32_t(renderTargetSizes_["postEffect4"].x), uint32_t(renderTargetSizes_["postEffect4"].y), "postEffect4");

	// テクスチャ読み込み
	t_uvChecker = Game::Asset::Texture::Load("assets/engine/texture/uvChecker.png");
	t_monsterBall_ = Game::Asset::Texture::Load("assets/engine/texture/monsterBall.png");
	t_white1x1_ = Game::Asset::Texture::Load("assets/engine/texture/white1x1.png");
	t_dds_ = Game::Asset::Texture::Load("assets/engine/texture/rostock_laage_airport_4k.dds");

	// モデル読み込み
	int32_t model1 = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");
	int32_t model2 = Game::Asset::Model::Load("assets/engine/model/sphere/sphere.obj");
	int32_t model3 = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");

	// オーディオ読み込み
	audio1 = Game::Asset::Audio::Load("assets/engine/audio/BGM/InGame.mp3");
	audio2 = Game::Asset::Audio::Load("assets/engine/audio/SE/バトル用/氷魔法1.mp3");


	//RenderObjectData data1;
	//data1.name = "cbvOnly";
	//data1.config.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//data1.config.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//data1.modelPath = "assets/engine/model/cube/cube.obj";
	//data1.out.push_back("mainL");

	//renderObjectData_.push_back(data1);

	//RenderObjectData data2;
	//data2.name = "cbvAndSrv";
	//data2.config.ps = "assets/shaders/SimpleModel/SimpleModels.PS.hlsl";
	//data2.config.vs = "assets/shaders/SimpleModel/SimpleModels.VS.hlsl";
	//data2.modelPath = "assets/engine/model/cube/cube.obj";
	//data2.out.push_back("mainR");

	//renderObjectData_.push_back(data2);

	//RenderObjectData data3;
	//data3.name = "skybox";
	//data3.config.ps = "assets/shaders/SkyBox/SkyBox.PS.hlsl";
	//data3.config.vs = "assets/shaders/SkyBox/SkyBox.VS.hlsl";
	//data3.modelPath = "assets/engine/model/sphere/sphere.obj";
	//data3.out.push_back("minimapL");

	//renderObjectData_.push_back(data3);

	//RenderObjectData data4;
	//data4.name = "PunctualLight";
	//data4.config.ps = "assets/shaders/PunctualLight/PunctualLight.PS.hlsl";
	//data4.config.vs = "assets/shaders/PunctualLight/PunctualLight.VS.hlsl";
	//data4.modelPath = "assets/engine/model/sphere/sphere.obj";
	//data4.out.push_back("minimapR");

	//renderObjectData_.push_back(data4);

	//RenderObjectData data5;
	//data5.name = "postEffects1";
	//data5.config.ps = "assets/shaders/FullScreen/RadialBlue.PS.hlsl";
	//data5.config.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//data5.modelPath = "assets/engine/model/plane/plane.obj";
	//data5.config.dsvFormatID = DSVFormatID::Unknown;
	//data5.out.push_back("minimapR");

	//renderObjectData_.push_back(data5);

	//RenderObjectData data6;
	//data6.name = "postEffects2";
	//data6.config.ps = "assets/shaders/FullScreen/Vignette.PS.hlsl";
	//data6.config.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//data6.modelPath = "assets/engine/model/plane/plane.obj";
	//data6.config.dsvFormatID = DSVFormatID::Unknown;
	//data6.out.push_back("minimapR");

	//renderObjectData_.push_back(data6);

	//RenderObjectData data7;
	//data7.name = "postEffects3";
	//data7.config.ps = "assets/shaders/FullScreen/LuminanceBasedOutline.PS.hlsl";
	//data7.config.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//data7.modelPath = "assets/engine/model/plane/plane.obj";
	//data7.config.dsvFormatID = DSVFormatID::Unknown;
	//data7.out.push_back("minimapR");

	//renderObjectData_.push_back(data7);

	//RenderObjectData data8;
	//data8.name = "postEffects3";
	//data8.config.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	//data8.config.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//data8.modelPath = "assets/engine/model/plane/plane.obj";
	//data8.config.dsvFormatID = DSVFormatID::Unknown;
	//data8.out.push_back("minimapR");

	//renderObjectData_.push_back(data8);

	//RenderObjectData data9;
	//data9.name = "screenDraw1";
	//data9.config.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//data9.config.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//data9.modelPath = "assets/engine/model/plane/plane.obj";
	//data9.config.dsvFormatID = DSVFormatID::Unknown;
	//data9.out.push_back("minimapR");

	//renderObjectData_.push_back(data9);

	//RenderObjectData data10;
	//data10.name = "screenDraw1";
	//data10.config.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//data10.config.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//data10.modelPath = "assets/engine/model/plane/plane.obj";
	//data10.config.dsvFormatID = DSVFormatID::Unknown;
	//data10.out.push_back("minimapR");

	//renderObjectData_.push_back(data10);

	//RenderObjectData data11;
	//data11.name = "screenDraw1";
	//data11.config.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//data11.config.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//data11.modelPath = "assets/engine/model/plane/plane.obj";
	//data11.config.dsvFormatID = DSVFormatID::Unknown;
	//data11.out.push_back("minimapR");

	//renderObjectData_.push_back(data11);

	//RenderObjectData data12;
	//data12.name = "screenDraw1";
	//data12.config.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//data12.config.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//data12.modelPath = "assets/engine/model/plane/plane.obj";
	//data12.config.dsvFormatID = DSVFormatID::Unknown;
	//data12.out.push_back("minimapR");

	//renderObjectData_.push_back(data12);

	//Save_renderObject();

	//cbvOnly_ = std::make_unique<RenderObject>();
	//cbvOnly_->modelID_ = model1;
	//cbvOnly_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//cbvOnly_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//cbvOnly_->SetupFromShaders();

	//cbvAndSrv_ = std::make_unique<RenderObject>();
	//cbvAndSrv_->modelID_ = model1;
	//cbvAndSrv_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModels.PS.hlsl";
	//cbvAndSrv_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModels.VS.hlsl";
	//cbvAndSrv_->instanceNum_ = 10;
	//cbvAndSrv_->SetupFromShaders();

	//line_ = std::make_unique<RenderObject>();
	//line_->psoConfig_.ps = "assets/shaders/Line/Line.PS.hlsl";
	//line_->psoConfig_.vs = "assets/shaders/Line/Line.VS.hlsl";
	//line_->SetupFromShaders();

	//skybox_ = std::make_unique<RenderObject>();
	//skybox_->modelID_ = model1;
	//skybox_->psoConfig_.ps = "assets/shaders/SkyBox/SkyBox.PS.hlsl";
	//skybox_->psoConfig_.vs = "assets/shaders/SkyBox/SkyBox.VS.hlsl";
	//skybox_->SetupFromShaders();

	//PunctualLight_ = std::make_unique<RenderObject>();
	//PunctualLight_->modelID_ = model2;
	//PunctualLight_->psoConfig_.ps = "assets/shaders/PunctualLight/PunctualLight.PS.hlsl";
	//PunctualLight_->psoConfig_.vs = "assets/shaders/PunctualLight/PunctualLight.VS.hlsl";
	//PunctualLight_->SetupFromShaders();

	//environmentMap_ = std::make_unique<RenderObject>();
	//environmentMap_->modelID_ = model2;
	//environmentMap_->psoConfig_.ps = "assets/shaders/EnvironmentMap/EnvironmentMap.PS.hlsl";
	//environmentMap_->psoConfig_.vs = "assets/shaders/EnvironmentMap/EnvironmentMap.VS.hlsl";
	//environmentMap_->SetupFromShaders();

	//postEffects_[0] = std::make_unique<RenderObject>();
	//postEffects_[0]->modelID_ = model3;
	//postEffects_[0]->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//postEffects_[0]->psoConfig_.ps = "assets/shaders/FullScreen/RadialBlue.PS.hlsl";
	//postEffects_[0]->psoConfig_.dsvFormatID = DSVFormatID::Unknown;
	//postEffects_[0]->SetupFromShaders();

	//postEffects_[1] = std::make_unique<RenderObject>();
	//postEffects_[1]->modelID_ = model3;
	//postEffects_[1]->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//postEffects_[1]->psoConfig_.ps = "assets/shaders/FullScreen/Vignette.PS.hlsl";
	//postEffects_[1]->psoConfig_.dsvFormatID = DSVFormatID::Unknown;
	//postEffects_[1]->SetupFromShaders();

	//postEffects_[2] = std::make_unique<RenderObject>();
	//postEffects_[2]->modelID_ = model3;
	//postEffects_[2]->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//postEffects_[2]->psoConfig_.ps = "assets/shaders/FullScreen/LuminanceBasedOutline.PS.hlsl";
	//postEffects_[2]->psoConfig_.dsvFormatID = DSVFormatID::Unknown;
	//postEffects_[2]->SetupFromShaders();

	//postEffects_[3] = std::make_unique<RenderObject>();
	//postEffects_[3]->modelID_ = model3;
	//postEffects_[3]->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	//postEffects_[3]->psoConfig_.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	//postEffects_[3]->psoConfig_.dsvFormatID = DSVFormatID::Unknown;
	//postEffects_[3]->SetupFromShaders();

	//screenDrawObjectMain1_ = std::make_unique<RenderObject>();
	//screenDrawObjectMain1_->modelID_ = model3;
	////screenDrawObjectMain1_->psoConfig_.blendID = BlendStateID::Normal2;
	//screenDrawObjectMain1_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//screenDrawObjectMain1_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//screenDrawObjectMain1_->SetupFromShaders();

	//screenDrawObjectMain2_ = std::make_unique<RenderObject>();
	//screenDrawObjectMain2_->modelID_ = model3;
	////screenDrawObjectMain2_->psoConfig_.blendID = BlendStateID::Normal2;
	//screenDrawObjectMain2_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//screenDrawObjectMain2_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//screenDrawObjectMain2_->SetupFromShaders();

	//screenDrawObjectMiniMap1_ = std::make_unique<RenderObject>();
	//screenDrawObjectMiniMap1_->modelID_ = model3;
	////screenDrawObjectMiniMap1_->psoConfig_.blendID = BlendStateID::Normal2;
	//screenDrawObjectMiniMap1_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//screenDrawObjectMiniMap1_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//screenDrawObjectMiniMap1_->SetupFromShaders();

	//screenDrawObjectMiniMap2_ = std::make_unique<RenderObject>();
	//screenDrawObjectMiniMap2_->modelID_ = model3;
	////screenDrawObjectMiniMap2_->psoConfig_.blendID = BlendStateID::Normal2;
	//screenDrawObjectMiniMap2_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//screenDrawObjectMiniMap2_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	//screenDrawObjectMiniMap2_->SetupFromShaders();


	transform1_.scale = { 10.0f,10.0f,10.0f };
	color1_ = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	for (size_t i = 0; i < 10; ++i)
	{
		transform2_[i].scale = { 10.0f,10.0f,10.0f };
		transform2_[i].translate = { 0.0f, 0.0f, static_cast<float>(((i + 1) * -15)) };
		color2_[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
		tex2_[i] = t_monsterBall_;
	}

	lightData_.LightCount = 1;

	main1ScreenTransform_.scale = { 320.0f,360.0f,0.0f };
	main1ScreenTransform_.translate = { 320.0f, 360.0f, 2.0f };
	main1ScreenTransform_.rotate = { -3.14159265f, 0.0f, 0.0f };

	main2ScreenTransform_.scale = { 320.0f,360.0f,0.0f };
	main2ScreenTransform_.translate = { 960.0f, 360.0f, 2.0f };
	main2ScreenTransform_.rotate = { -3.14159265f, 0.0f, 0.0f };

	miniMap1ScreenTransform_.scale = { 128.00f,72.00f,0.0f };
	miniMap1ScreenTransform_.translate = { 1280.0f - 178.0f, 720.0f - 122.0f, 1.0f };
	miniMap1ScreenTransform_.rotate = { -3.14159265f, 0.0f, 0.0f };

	miniMap2ScreenTransform_.scale = { 128.00f,72.00f,0.0f };
	miniMap2ScreenTransform_.translate = { 640.0f - 178.0f, 720.0f - 122.0f, 1.0f };
	miniMap2ScreenTransform_.rotate = { -3.14159265f, 0.0f, 0.0f };

	testAnimation_ = std::make_unique<TestAnimation>();
	testAnimation_->Initialize();
	testParticle_ = std::make_unique<TestParticle>();
	testParticle_->Initialize();
}

TestPhase::~TestPhase()
{
}

void TestPhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;
}

void TestPhase::Update()
{
#pragma region メインカメラ１

	//// メインカメラ1
	////Game::Camera::Update(c_main1_);
	//Game::Camera::Update(0);
	//Matrix4x4 viewProjection_main1 = Game::Camera::Getter::GetViewProjectionMatrix(0);
	//
	//Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform1_.scale, transform1_.rotate, transform1_.translate);
	//Matrix4x4 worldViewProjection = worldMatrix * viewProjection_main1;
	//cbvOnly_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	//cbvOnly_->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
	//cbvOnly_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	//cbvOnly_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);

#pragma endregion


#pragma region メインカメラ２

	////Game::Camera::Update(c_main2_);
	//Game::Camera::Update(1);
	//Matrix4x4 viewProjection_main2 = Game::Camera::Getter::GetViewProjectionMatrix(1);

	//std::vector<Matrix4x4> worldMatrices2;
	//for (size_t i = 0; i < 10; ++i)
	//{
	//	Matrix4x4 worldMatrix2 = Matrix4x4::MakeAffineMatrix(transform2_[i].scale, transform2_[i].rotate, transform2_[i].translate);
	//	worldMatrices2.push_back(worldMatrix2);
	//}
	//cbvAndSrv_->SetSBufferData(0, ShaderType::PixelShader, &color2_, sizeof(Vector4), 10);
	//cbvAndSrv_->SetSBufferData(1, ShaderType::PixelShader, &tex2_, sizeof(int32_t), 10);
	//cbvAndSrv_->SetCBufferData(0, ShaderType::VertexShader, &viewProjection_main2);
	//cbvAndSrv_->SetSBufferData(0, ShaderType::VertexShader, worldMatrices2.data(), sizeof(Matrix4x4), worldMatrices2.size());

	//line_->SetCBufferData(0, ShaderType::VertexShader, &viewProjection_main2);
	//line_->SetCBufferData(0, ShaderType::PixelShader, &color1_);

#pragma endregion


#pragma region ミニマップカメラ１

	//Game::Camera::Update(2);
	//Matrix4x4 viewMatrix_miniMap1 = Game::Camera::Getter::GetViewMatrix(2);
	//Matrix4x4 projectionMatrix_miniMap1 = Game::Camera::Getter::GetProjectionMatrix(2);

	//Matrix4x4 noTranslateView = viewMatrix_miniMap1;
	//noTranslateView.m[3][0] = 0.0f;
	//noTranslateView.m[3][1] = 0.0f;
	//noTranslateView.m[3][2] = 0.0f;
	//Matrix4x4 noTranslateViewProjection = noTranslateView * projectionMatrix_miniMap1;
	//skybox_->SetCBufferData(0, ShaderType::VertexShader, &noTranslateViewProjection);
	//skybox_->SetCBufferData(0, ShaderType::PixelShader, &t_dds_);

#pragma endregion


#pragma region ミニマップカメラ２

	//Game::Camera::Update(3);
	//Vector3 cameraPos_miniMap2_ = Game::Camera::Getter::GetTranslate(3);
	//Matrix4x4 projectionMatrix_miniMap2_ = Game::Camera::Getter::GetProjectionMatrix(3);
	//Matrix4x4 projectionInverse_miniMap2_ = projectionMatrix_miniMap2_.Inverse();

	//PunctualLight_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	//PunctualLight_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	//PunctualLight_->SetCBufferData(0, ShaderType::PixelShader, &cameraPos_miniMap2_);
	//PunctualLight_->SetCBufferData(1, ShaderType::PixelShader, &lightData_);
	//PunctualLight_->SetCBufferData(2, ShaderType::PixelShader, &materialData_);
	//PunctualLight_->SetCBufferData(3, ShaderType::PixelShader, &t_uvChecker);

	//environmentMap_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	//environmentMap_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	//environmentMap_->SetCBufferData(0, ShaderType::PixelShader, &cameraPos_miniMap2_);
	//environmentMap_->SetCBufferData(1, ShaderType::PixelShader, &lightData_);
	//environmentMap_->SetCBufferData(2, ShaderType::PixelShader, &materialData_);
	//environmentMap_->SetCBufferData(3, ShaderType::PixelShader, &t_uvChecker);
	//environmentMap_->SetCBufferData(4, ShaderType::PixelShader, &t_dds_);

#pragma endregion


#pragma region スクリーンカメラ

	//Game::Camera::Update(c_worldView_);
	//Matrix4x4 orthoProj_world = Game::Camera::Getter::GetOrthoProjectionMatrix(c_worldView_);

	//Matrix4x4 mainScreenWorldMatrix = Matrix4x4::MakeAffineMatrix(main1ScreenTransform_.scale, main1ScreenTransform_.rotate, main1ScreenTransform_.translate);
	//Matrix4x4 mainScreenWorldViewProjection = mainScreenWorldMatrix * orthoProj_world;
	//// rt_Vignetteの画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMain1_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	//screenDrawObjectMain1_->SetCBufferData(1, ShaderType::PixelShader, &rt_Vignette_);
	//screenDrawObjectMain1_->SetCBufferData(0, ShaderType::VertexShader, &mainScreenWorldViewProjection);
	//screenDrawObjectMain1_->SetCBufferData(1, ShaderType::VertexShader, &mainScreenWorldMatrix);

	//Matrix4x4 mainScreen2WorldMatrix = Matrix4x4::MakeAffineMatrix(main2ScreenTransform_.scale, main2ScreenTransform_.rotate, main2ScreenTransform_.translate);
	//Matrix4x4 mainScreen2WorldViewProjection = mainScreen2WorldMatrix * orthoProj_world;
	//// rt_GrayScaleの画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMain2_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	//screenDrawObjectMain2_->SetCBufferData(1, ShaderType::PixelShader, &rt_GrayScale_);
	//screenDrawObjectMain2_->SetCBufferData(0, ShaderType::VertexShader, &mainScreen2WorldViewProjection);
	//screenDrawObjectMain2_->SetCBufferData(1, ShaderType::VertexShader, &mainScreen2WorldMatrix);

	//Matrix4x4 miniMap1WorldMatrix = Matrix4x4::MakeAffineMatrix(miniMap1ScreenTransform_.scale, miniMap1ScreenTransform_.rotate, miniMap1ScreenTransform_.translate);
	//Matrix4x4 miniMap1WorldViewProjection = miniMap1WorldMatrix * orthoProj_world;
	//// rt_luminanceBasedOutlineの画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMiniMap1_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	//screenDrawObjectMiniMap1_->SetCBufferData(1, ShaderType::PixelShader, &rt_luminanceBasedOutline_);
	//screenDrawObjectMiniMap1_->SetCBufferData(0, ShaderType::VertexShader, &miniMap1WorldViewProjection);
	//screenDrawObjectMiniMap1_->SetCBufferData(1, ShaderType::VertexShader, &miniMap1WorldMatrix);

	//Matrix4x4 miniMap2WorldMatrix = Matrix4x4::MakeAffineMatrix(miniMap2ScreenTransform_.scale, miniMap2ScreenTransform_.rotate, miniMap2ScreenTransform_.translate);
	//Matrix4x4 miniMap2WorldViewProjection = miniMap2WorldMatrix * orthoProj_world;
	//// rt_depthBasedOutlineの画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMiniMap2_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	//screenDrawObjectMiniMap2_->SetCBufferData(1, ShaderType::PixelShader, &rt_depthBasedOutline_);
	//screenDrawObjectMiniMap2_->SetCBufferData(0, ShaderType::VertexShader, &miniMap2WorldViewProjection);
	//screenDrawObjectMiniMap2_->SetCBufferData(1, ShaderType::VertexShader, &miniMap2WorldMatrix);

#pragma endregion

	//// main1の画像をSetCBufferDataしrt_Vignetteに書き込む
	//postEffects_[0]->SetCBufferData(0, ShaderType::PixelShader, &rt_main1_);
	//// main2の画像をSetCBufferDataしrt_GrayScaleに書き込む
	//postEffects_[1]->SetCBufferData(0, ShaderType::PixelShader, &rt_main2_);
	//// miniMap1の画像をSetCBufferDataしrt_luminanceBasedOutlineに書き込む
	//postEffects_[2]->SetCBufferData(0, ShaderType::PixelShader, &rt_miniMap1_);
	//// miniMap2の画像をSetCBufferDataしrt_depthBasedOutlineに書き込む
	//postEffects_[3]->SetCBufferData(0, ShaderType::PixelShader, &rt_miniMap2_);
	//postEffects_[3]->SetCBufferData(1, ShaderType::PixelShader, &rt_miniMap2_depth_);
	//postEffects_[3]->SetCBufferData(2, ShaderType::PixelShader, &projectionMatrix_miniMap2_);


	//testAnimation_->Update(0);
	//testParticle_->Update(0);

	//if (Game::IO::Key::IsJustPressed(VK_F12))
	//{
	//	//Game::Window::ToggleFullscreen();
	//}
	//if (Game::IO::Key::IsJustPressed(VK_F11))
	//{
	//	Game::Asset::RenderTexture::SaveAllRenderTextureToFile("screenshots");
	//}
	//if (Game::IO::Key::IsJustPressed(VK_ESCAPE))
	//{
	//	Game::quit();
	//}
}

void TestPhase::Draw()
{
	//// main1に書き込む
	//cbvOnly_->Draw(rt_main1_);

	//// main2に書き込む
	//cbvAndSrv_->Draw(rt_main2_);

	//// miniMap1に書き込む
	//skybox_->Draw(rt_miniMap1_);

	//// miniMap2に書き込む
	//PunctualLight_->Draw(rt_miniMap2_);
	//environmentMap_->Draw(rt_miniMap2_);


	////line_->Draw();
	////PunctualLight_->Draw();

	//// rt_main1_の画像をSetCBufferDataしrt_Vignetteに書き込む
	//postEffects_[0]->PostEffectDraw(rt_Vignette_);
	//// rt_main2_の画像をSetCBufferDataしrt_GrayScaleに書き込む
	//postEffects_[1]->PostEffectDraw(rt_GrayScale_);
	//// rt_miniMap1_の画像をSetCBufferDataしrt_luminanceBasedOutline_に書き込む
	//postEffects_[2]->PostEffectDraw(rt_luminanceBasedOutline_);
	//// rt_miniMap2_の画像をSetCBufferDataしrt_depthBasedOutline_に書き込む
	//postEffects_[3]->PostEffectDraw(rt_depthBasedOutline_);

	//// rt_Vignetteの画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMain1_->ScreenDraw();
	//// rt_GrayScale_の画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMain2_->ScreenDraw();
	//// rt_luminanceBasedOutline_の画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMiniMap1_->ScreenDraw();
	//// rt_depthBasedOutline_の画像をSetCBufferDataしBackBufferに書き込む
	//screenDrawObjectMiniMap2_->ScreenDraw();
}


void TestPhase::DrawImGui()
{

	ImGui::Begin("renderTexture"); 

	static RenderTargetData renderTargetData;
	const uint32_t minSize = 1;
	const uint32_t maxSize = 4096;

	ImGui::InputText("name", &renderTargetData.name);
	ImGui::DragScalar("renderTarget width", ImGuiDataType_U32, &renderTargetData.width, 1.0f, &minSize, &maxSize);
	ImGui::DragScalar("renderTarget height", ImGuiDataType_U32, &renderTargetData.height, 1.0f, &minSize, &maxSize);
	if (ImGui::Button("AddList"))
	{
		if (!renderTargetData.name.empty())
		{
			int32_t renderTargetID = Game::Asset::RenderTexture::CreateRenderTexture(renderTargetData.width, renderTargetData.height, renderTargetData.name);
			renderTargetData_[renderTargetID] = renderTargetData;
		}
	}

	if (ImGui::Button("Save"))Save_renderTarget();
	ImGui::SameLine();
	if (ImGui::Button("Load"))Load_renderTarget();

	//for (auto& [id, data] : renderTargetData_)
	//{
	//	if (ImGui::TreeNode(data.name.c_str()))
	//	{
	//		ImGui::Image((ImTextureID)Game::Asset::RenderTexture::GetRenderTexture(data.name), ImVec2(data.width / 10, data.height / 10));

	//		ImGui::TreePop();
	//	}
	//}

	ImGui::End();




	ImGui::Begin("Camera");

	static CameraData cameraData;
	ImGui::InputText("name", &cameraData.name);
	ImGui::DragFloat2("camera size", &cameraData.size.x, 1.0f);
	if (ImGui::Button("AddList"))
	{
		if (!renderTargetData.name.empty())
		{
			int32_t renderTargetID = Game::Asset::RenderTexture::CreateRenderTexture(renderTargetData.width, renderTargetData.height, renderTargetData.name);
			renderTargetData_[renderTargetID] = renderTargetData;
		}
	}

	if (ImGui::Button("Save"))Save_camera();
	ImGui::SameLine();
	if (ImGui::Button("Load"))Load_camera();


	ImGui::End();


	ImGui::Begin("Facade Test");
	if (ImGui::BeginTabBar("Facade Test", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable))
	{
		if (ImGui::BeginTabItem("RenderObject Test"))
		{
			if (ImGui::TreeNode("cbvOnly_"))
			{
				ImGui::ColorEdit4("color1", &color1_.x, 1);
				ImGui::DragFloat3("scale", &transform1_.scale.x, 0.1f, 0.1f, 100.0f);
				ImGui::DragFloat3("rotate", &transform1_.rotate.x, 0.1f);
				ImGui::DragFloat3("translate", &transform1_.translate.x, 0.1f, -100.0f, 100.0f);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("cbvAndSrv_"))
			{
				for (size_t i = 0; i < 10; ++i)
				{
					ImGui::ColorEdit4(("color2_" + std::to_string(i)).c_str(), &color2_[i].x, 1);
					ImGui::DragInt(("textureID" + std::to_string(i)).c_str(), &tex2_[i], 1, 0, 10);
					ImGui::DragFloat3(("scale" + std::to_string(i)).c_str(), &transform2_[i].scale.x, 0.1f, 0.1f, 100.0f);
					ImGui::DragFloat3(("rotate" + std::to_string(i)).c_str(), &transform2_[i].rotate.x, 0.1f);
					ImGui::DragFloat3(("translate" + std::to_string(i)).c_str(), &transform2_[i].translate.x, 0.1f, -100.0f, 100.0f);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("PunctualLight_"))
			{
				ImGui::ColorEdit4("light color", &lightData_.lights[0].color.x, 1);
				ImGui::DragFloat("light intensity", &lightData_.lights[0].intensity, 0.1f, 0.0f, 10.0f);
				static const char* items[] = { "Dir", "Point", "Spot" };
				ImGui::Combo("light type", &lightData_.lights[0].type, items, IM_ARRAYSIZE(items));
				ImGui::SeparatorText("directional light");
				ImGui::DragFloat3("light direction", &lightData_.lights[0].direction.x, 0.1f);
				ImGui::SeparatorText("spot light");
				ImGui::DragFloat("light spotCos", &lightData_.lights[0].spotCos, 0.01f, -1.0f, 1.0f);
				ImGui::SeparatorText("point light");
				ImGui::DragFloat3("light position", &lightData_.lights[0].position.x, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("light range", &lightData_.lights[0].range, 0.1f, 0.1f, 100.0f);


				ImGui::SeparatorText("Material");

				ImGui::ColorEdit3("material specular", &materialData_.diffuseColor.x, 1);
				ImGui::ColorEdit3("material diffuse", &materialData_.specularColor.x, 1);
				ImGui::DragFloat("material shininess", &materialData_.shininess, 0.1f, 0.0f, 100.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Screen"))
			{
				ImGui::DragFloat3("main1 scale", &main1ScreenTransform_.scale.x, 0.1f);
				ImGui::DragFloat3("main1 rotate", &main1ScreenTransform_.rotate.x, 0.1f);
				ImGui::DragFloat3("main1 translate", &main1ScreenTransform_.translate.x, 0.1f);

				ImGui::Separator();

				ImGui::DragFloat3("main2 scale", &main2ScreenTransform_.scale.x, 0.1f);
				ImGui::DragFloat3("main2 rotate", &main2ScreenTransform_.rotate.x, 0.1f);
				ImGui::DragFloat3("main2 translate", &main2ScreenTransform_.translate.x, 0.1f);

				ImGui::Separator();

				ImGui::DragFloat3("miniMap1 scale", &miniMap1ScreenTransform_.scale.x, 0.1f);
				ImGui::DragFloat3("miniMap1 rotate", &miniMap1ScreenTransform_.rotate.x, 0.1f);
				ImGui::DragFloat3("miniMap1 translate", &miniMap1ScreenTransform_.translate.x, 0.1f);

				ImGui::Separator();

				ImGui::DragFloat3("miniMap2 scale", &miniMap2ScreenTransform_.scale.x, 0.1f);
				ImGui::DragFloat3("miniMap2 rotate", &miniMap2ScreenTransform_.rotate.x, 0.1f);
				ImGui::DragFloat3("miniMap2 translate", &miniMap2ScreenTransform_.translate.x, 0.1f);

				ImGui::TreePop();
			}

			ImGui::EndTabItem();
		}


#pragma region debugDraw test

		if (ImGui::BeginTabItem("DebugDraw Test"))
		{
			//ImGui::Text("GreenSphere");
			//ImGui::DragFloat3("sphere center", &sphere1_.center.x, 0.1f);
			//ImGui::DragFloat("sphere radius", &sphere1_.radius, 0.1f, 0.1f, 100.0f);
			//ImGui::Text("BlueSphereXYZ");
			//ImGui::DragFloat3("sphereXYZ center", &sphereXYZ1_.center.x, 0.1f);
			//ImGui::DragFloat3("sphereXYZ radius", &sphereXYZ1_.radius.x, 0.1f, 0.1f, 100.0f);
			//ImGui::Text("YellowCylinder");
			//ImGui::DragFloat3("cylinder bottomCenter", &cylinder1_.bottomCenter.x, 0.1f);
			//ImGui::DragFloat3("cylinder topCenter", &cylinder1_.topCenter.x, 0.1f);
			//ImGui::DragFloat("cylinder radius", &cylinder1_.radius, 0.1f, 0.1f, 100.0f);
			//ImGui::Text("PurpleAABB");
			//ImGui::DragFloat3("aabb min", &aabb1_.min.x, 0.1f);
			//ImGui::DragFloat3("aabb max", &aabb1_.max.x, 0.1f);
			//ImGui::Text("pos on cylindrical coord");
			//ImGui::DragFloat("radius", &cylindricalPos_.radius, 0.1f, 0.0f, 100.0f);
			//ImGui::DragFloat("theta", &cylindricalPos_.theta, 0.1f);
			//ImGui::DragFloat("height", &cylindricalPos_.height, 0.1f);

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region audio test

		if (ImGui::BeginTabItem("Audio Test"))
		{
			if (ImGui::Button("Play Audio1"))
			{
				Game::Audio::PlayAudio(audio1, true);
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop Audio1"))
			{
				Game::Audio::StopAudio(audio1);
			}
			if (ImGui::Button("Play Audio2"))
			{
				Game::Audio::PlayAudio(audio2, false);
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop Audio2"))
			{
				Game::Audio::StopAudio(audio2);
			}
			float volume1 = Game::Audio::GetVolume(audio1);
			ImGui::SliderFloat("audio1 volume", &volume1, 0.0f, 1.0f);
			ImGui::Text("audio1 is playing : %d", Game::Audio::IsAudioPlaying(audio1));
			Game::Audio::SetAudioVolume(audio1, volume1);
			float volume2 = Game::Audio::GetVolume(audio2);
			ImGui::SliderFloat("audio2 volume", &volume2, 0.0f, 1.0f);
			ImGui::Text("audio2 is playing : %d", Game::Audio::IsAudioPlaying(audio2));
			Game::Audio::SetAudioVolume(audio2, volume2);
			float masterVolume = Game::Audio::GetMasterVolume();
			ImGui::SliderFloat("master volume", &masterVolume, 0.0f, 1.0f);
			Game::Audio::SetMasterVolume(masterVolume);

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region camera test

		if (ImGui::BeginTabItem("Camera Test"))
		{
			static Vector3 cameraCenterTarget;
			static int32_t cameraCenterFrame = 120;
			ImGui::DragFloat3("camera center", &cameraCenterTarget.x, 0.1f);
			ImGui::DragInt("camera center frame", &cameraCenterFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Center"))
			{
				Game::Camera::Setter::SetCenter(cameraCenterTarget, cameraCenterFrame, EaseType::IN_CUBIC, 0);
			}

			static Vector3 cameraRotateTarget;
			static int32_t cameraRotateFrame = 120;
			ImGui::DragFloat3("camera rotate", &cameraRotateTarget.x, 0.1f);
			ImGui::DragInt("camera rotate frame", &cameraRotateFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Rotate"))
			{
				Game::Camera::Setter::SetRotate(cameraRotateTarget, cameraRotateFrame, EaseType::IN_CUBIC, 0);
			}

			static float cameraDistanceTarget = 0.0f;
			static int32_t cameraDistanceFrame = 120;
			ImGui::DragFloat("camera distance", &cameraDistanceTarget, 0.1f);
			ImGui::DragInt("camera distance frame", &cameraDistanceFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Distance"))
			{
				Game::Camera::Setter::SetDistance(cameraDistanceTarget, cameraDistanceFrame, EaseType::IN_CUBIC, 0);
			}

			static float intensity = 3.0f;
			static float duration = 35.0f;
			static float frequency = 15.0f;
			ImGui::DragFloat("camera shake intensity", &intensity, 0.1f);
			ImGui::DragFloat("camera shake duration", &duration, 0.1f);
			ImGui::DragFloat("camera shake frequency", &frequency, 0.1f);
			if (ImGui::Button("Start Camera Shake"))
			{
				Game::Camera::Shake::Start(intensity, duration, frequency, 0);
			}
			ImGui::Text("is camera shaking : %d", Game::Camera::Shake::IsShaking(0));
			if (ImGui::Button("Stop Camera Shake"))
			{
				Game::Camera::Shake::Stop(0);
			}

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region mouse test

		if (ImGui::BeginTabItem("mouse Test"))
		{
			ImGui::Text("Mouse Position: (%.1f, %.1f)", Game::IO::Mouse::Get2DPosition().x, Game::IO::Mouse::Get2DPosition().y);
			//ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", Game::IO::Mouse::Get3DPosition(0).x, Game::IO::Mouse::Get3DPosition(0).y, Game::IO::Mouse::Get3DPosition(0).z);
			//ImGui::Text("Mouse Ray Origin: (%.1f, %.1f, %.1f)", Game::IO::Mouse::GetRay(0).origin.x, Game::IO::Mouse::GetRay(0).origin.y, Game::IO::Mouse::GetRay(0).origin.z);
			//ImGui::Text("Mouse Ray Diff  : (%.1f, %.1f, %.1f)", Game::IO::Mouse::GetRay(0).diff.x, Game::IO::Mouse::GetRay(0).diff.y, Game::IO::Mouse::GetRay(0).diff.z);
			ImGui::Text("Mouse Wheel: %d", Game::IO::Mouse::GetWheel());

			ImGui::Text("Mouse Buttons:");
			ImGui::Text("Left Button - %d-%d-%d : %d",
				Game::IO::Mouse::IsJustPressed(0),
				Game::IO::Mouse::IsHeld(0),
				Game::IO::Mouse::IsJustReleased(0),
				Game::IO::Mouse::HoldFrames(0));
			ImGui::Text("Right Button - %d-%d-%d : %d",
				Game::IO::Mouse::IsJustPressed(1),
				Game::IO::Mouse::IsHeld(1),
				Game::IO::Mouse::IsJustReleased(1),
				Game::IO::Mouse::HoldFrames(1));
			ImGui::Text("Middle Button - %d-%d-%d : %d",
				Game::IO::Mouse::IsJustPressed(2),
				Game::IO::Mouse::IsHeld(2),
				Game::IO::Mouse::IsJustReleased(2),
				Game::IO::Mouse::HoldFrames(2));

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region keyboard test

		if (ImGui::BeginTabItem("keyboard Test"))
		{
			//struct KeyInfo
			//{
			//	const char* name;
			//	int32_t dik;
			//};

			//static const KeyInfo kKeys[] = {
			//	{"A", DIK_A}, {"B", DIK_B}, {"C", DIK_C}, {"D", DIK_D},
			//	{"E", DIK_E}, {"F", DIK_F}, {"G", DIK_G}, {"H", DIK_H},
			//	{"I", DIK_I}, {"J", DIK_J}, {"K", DIK_K}, {"L", DIK_L},
			//	{"M", DIK_M}, {"N", DIK_N}, {"O", DIK_O}, {"P", DIK_P},
			//	{"Q", DIK_Q}, {"R", DIK_R}, {"S", DIK_S}, {"T", DIK_T},
			//	{"U", DIK_U}, {"V", DIK_V}, {"W", DIK_W}, {"X", DIK_X},
			//	{"Y", DIK_Y}, {"Z", DIK_Z},

			//	{"Space", DIK_SPACE},

			//	{"F1", DIK_F1}, {"F2", DIK_F2}, {"F3", DIK_F3}, {"F4", DIK_F4},
			//	{"F5", DIK_F5}, {"F6", DIK_F6}, {"F7", DIK_F7}, {"F8", DIK_F8},
			//	{"F9", DIK_F9}, {"F10", DIK_F10}, {"F11", DIK_F11}, {"F12", DIK_F12},

			//	{"Enter", DIK_RETURN}, {"Escape", DIK_ESCAPE},

			//	{"Up", DIK_UP}, {"Down", DIK_DOWN}, {"Left", DIK_LEFT}, {"Right", DIK_RIGHT},

			//	{"LShift", DIK_LSHIFT}, {"RShift", DIK_RSHIFT},

			//	{"0", DIK_0}, {"1", DIK_1}, {"2", DIK_2}, {"3", DIK_3}, {"4", DIK_4},
			//	{"5", DIK_5}, {"6", DIK_6}, {"7", DIK_7}, {"8", DIK_8}, {"9", DIK_9}
			//};

			//for (const auto& k : kKeys)
			//{
			//	if (Game::IO::Key::IsHeld(k.dik) ||
			//		Game::IO::Key::IsJustPressed(k.dik) ||
			//		Game::IO::Key::IsJustReleased(k.dik))
			//	{
			//		ImGui::Text("%s : %d:%d:%d _ %d",
			//			k.name,
			//			Game::IO::Key::IsJustPressed(k.dik),
			//			Game::IO::Key::IsHeld(k.dik),
			//			Game::IO::Key::IsJustReleased(k.dik),
			//			Game::IO::Key::HoldFrames(k.dik)
			//		);
			//	}
			//}
			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region pad test

		if (ImGui::BeginTabItem("pad Test"))
		{
			ImGui::Text("Connected Pad Sum: %d", Game::IO::Pad::GetConnectedPadNum());
			for (int32_t padIndex = 0; padIndex < Game::IO::Pad::GetConnectedPadNum(); ++padIndex)
			{
				ImGui::Text("Pad %d Buttons:", padIndex);
				ImGui::Text("------------------------------");
				ImGui::Text("Pad Left  Stick: (%.2f, %.2f)", Game::IO::Pad::GetLeftStick(padIndex).x, Game::IO::Pad::GetLeftStick(padIndex).y);
				ImGui::Text("Pad Right Stick: (%.2f, %.2f)", Game::IO::Pad::GetRightStick(padIndex).x, Game::IO::Pad::GetRightStick(padIndex).y);
				ImGui::Text("Pad Left  Trigger: %.2f", Game::IO::Pad::GetLeftTrigger(padIndex));
				ImGui::Text("Pad Right Trigger: %.2f", Game::IO::Pad::GetRightTrigger(padIndex));
				ImGui::Text("Pad Buttons:");
				ImGui::Text("A - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_A),
					Game::IO::Pad::IsHeld(padIndex, PAD_A),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_A),
					Game::IO::Pad::HoldFrames(padIndex, PAD_A));
				ImGui::Text("B - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_B),
					Game::IO::Pad::IsHeld(padIndex, PAD_B),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_B),
					Game::IO::Pad::HoldFrames(padIndex, PAD_B));
				ImGui::Text("X - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_X),
					Game::IO::Pad::IsHeld(padIndex, PAD_X),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_X),
					Game::IO::Pad::HoldFrames(padIndex, PAD_X));
				ImGui::Text("Y - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_Y),
					Game::IO::Pad::IsHeld(padIndex, PAD_Y),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_Y),
					Game::IO::Pad::HoldFrames(padIndex, PAD_Y));
				ImGui::Text("LB - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_LB),
					Game::IO::Pad::IsHeld(padIndex, PAD_LB),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_LB),
					Game::IO::Pad::HoldFrames(padIndex, PAD_LB));
				ImGui::Text("RB - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_RB),
					Game::IO::Pad::IsHeld(padIndex, PAD_RB),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_RB),
					Game::IO::Pad::HoldFrames(padIndex, PAD_RB));
				ImGui::Text("Back - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_BACK),
					Game::IO::Pad::IsHeld(padIndex, PAD_BACK),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_BACK),
					Game::IO::Pad::HoldFrames(padIndex, PAD_BACK));
				ImGui::Text("Start - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_START),
					Game::IO::Pad::IsHeld(padIndex, PAD_START),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_START),
					Game::IO::Pad::HoldFrames(padIndex, PAD_START));
				ImGui::Text("LS - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_LS),
					Game::IO::Pad::IsHeld(padIndex, PAD_LS),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_LS),
					Game::IO::Pad::HoldFrames(padIndex, PAD_LS));
				ImGui::Text("RS - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_RS),
					Game::IO::Pad::IsHeld(padIndex, PAD_RS),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_RS),
					Game::IO::Pad::HoldFrames(padIndex, PAD_RS));
				ImGui::Text("DPad Up - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_UP),
					Game::IO::Pad::IsHeld(padIndex, PAD_UP),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_UP),
					Game::IO::Pad::HoldFrames(padIndex, PAD_UP));
				ImGui::Text("DPad Down - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_DOWN),
					Game::IO::Pad::IsHeld(padIndex, PAD_DOWN),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_DOWN),
					Game::IO::Pad::HoldFrames(padIndex, PAD_DOWN));
				ImGui::Text("DPad Left - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_LEFT),
					Game::IO::Pad::IsHeld(padIndex, PAD_LEFT),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_LEFT),
					Game::IO::Pad::HoldFrames(padIndex, PAD_LEFT));
				ImGui::Text("DPad Right - %d-%d-%d : %d",
					Game::IO::Pad::IsJustPressed(padIndex, PAD_RIGHT),
					Game::IO::Pad::IsHeld(padIndex, PAD_RIGHT),
					Game::IO::Pad::IsJustReleased(padIndex, PAD_RIGHT),
					Game::IO::Pad::HoldFrames(padIndex, PAD_RIGHT));
				ImGui::Button("vibrate");
				static Vector2 vibrationAmount = { 0.5f,0.5f };
				ImGui::DragFloat2("vibration amount", &vibrationAmount.x, 0.1f, 0.0f, 1.0f);
				if (ImGui::IsItemClicked())
				{
					Game::IO::Pad::SetVibration(padIndex, vibrationAmount.x, vibrationAmount.y);
				}
			}

			ImGui::EndTabItem();
		}

#pragma endregion

		ImGui::EndTabBar();
	}
	ImGui::End();


	ImGui::Begin("------debug info------");
	ImGui::Text("ESC : Quit Application");
	ImGui::Text("F1  : Hide Debug Info");
	ImGui::Text("F11 : Screenshot");
	ImGui::Text("F12 : Toggle Fullscreen");
	ImGui::Text("FPS: %.1f ", Game::Time::GetFrameRate());
	ImGui::End();

	// ImGui の主な関数一覧
	//📐 レイアウト・カーソル操作
	//	- NewLine() : 改行して次の行へ
	//	- SameLine() : 同じ行に次の要素を配置
	//	- Spacing() : 標準的な余白を挿入
	//	- Separator() : 水平線を描画
	//	- Dummy() : 指定サイズの空白を確保
	//📝 テキスト関連
	//	- Text() : テキスト表示
	//	- BulletText() : 箇条書き風テキスト
	//	- LabelText() : ラベル＋値の組み合わせ表示
	//🎛 基本ウィジェット
	//	- Button() : ボタン
	//	- Checkbox() : チェックボックス
	//	- RadioButton() : ラジオボタン
	//	- ProgressBar() : 進捗バー
	//	- Image() : 画像表示
	//🎚 スライダー・入力
	//	- SliderFloat() / SliderInt() : スライダーで数値入力
	//	- DragFloat() / DragInt() : ドラッグ操作で数値入力
	//	- InputText() : テキスト入力
	//	- InputFloat() / InputInt() : 数値入力
	//🎨 カラー関連
	//	- ColorEdit3() / ColorEdit4() : RGB / RGBA カラー編集
	//	- ColorPicker3() / ColorPicker4() : カラーピッカー
	//📂 コンボ・リスト
	//	- Combo() : ドロップダウン選択
	//	- ListBox() : リスト選択
	//	- Selectable() : 選択可能アイテム
	//🌳 階層構造
	//	- TreeNode() / TreePop() : ツリー表示
	//	- CollapsingHeader() : 折り畳み可能なヘッダー
	//🍴 メニュー・ポップアップ
	//	- BeginMenu() / EndMenu() : メニュー
	//	- OpenPopup() / BeginPopup() / EndPopup() : ポップアップ
	//	- BeginTooltip() / EndTooltip() : ツールチップ
	//📊 テーブル・カラム
	//	- BeginTable() / EndTable() : テーブル表示
	//	- TableNextRow() / TableNextColumn() : テーブル操作
	//	- レガシー API : Columns()
	//🔧 ユーティリティ
	//	- IsItemHovered() : マウスが乗っているか判定
	//	- IsItemActive() : アクティブ状態判定

}

void TestPhase::Load_renderTarget()
{
	const std::string filePath = "assets/application/json/TestPhase/";

	const std::string fileName = "renderTargets.json";

	//if (JsonManager::Load(filePath + fileName))
	//{
	//	renderTargetData_.clear();

	//	int32_t i = 0;
	//	while (true)
	//	{
	//		std::string basePath = "/render_targets/" + std::to_string(i);

	//		RenderTargetData data;

	//		if (!JsonManager::Load(filePath + fileName, basePath + "/name", data.name)) break;
	//		JsonManager::Load(filePath + fileName, basePath + "/width", data.width);
	//		JsonManager::Load(filePath + fileName, basePath + "/height", data.height);
	//		JsonManager::Load(filePath + fileName, basePath + "/in", data.in);
	//		JsonManager::Load(filePath + fileName, basePath + "/out", data.out);

	//		int32_t id = Game::Asset::RenderTexture::CreateRenderTexture(data.width, data.height, data.name);

	//		renderTargetData_[id] = data;

	//		i++;
	//	}
	//}

}

void TestPhase::Save_renderTarget()
{
	const std::string filePath = "assets/application/json/TestPhase/";

	const std::string fileName = "renderTargets.json";

	int32_t i = 0;
	for (auto [key, value] : renderTargetData_)
	{
		std::string basePath = "/render_targets/" + std::to_string(i);

		JsonManager::AddParam(filePath + fileName, basePath + "/name", value.name);
		JsonManager::AddParam(filePath + fileName, basePath + "/width", value.width);
		JsonManager::AddParam(filePath + fileName, basePath + "/height", value.height);
		JsonManager::AddParam(filePath + fileName, basePath + "/in", value.in);
		JsonManager::AddParam(filePath + fileName, basePath + "/out", value.out);

		i++;
	}
	JsonManager::Save(filePath + fileName);
}

void TestPhase::Load_camera()
{
	const std::string filePath = "assets/application/json/TestPhase/";

	const std::string fileName = "cameras.json";

	//if (JsonManager::Load(filePath + fileName))
	//{
	//	cameraData_.clear();

	//	int32_t i = 0;
	//	while (true)
	//	{
	//		std::string basePath = "/cameras/" + std::to_string(i);

	//		CameraData data;

	//		if (!JsonManager::Load(filePath + fileName, basePath + "/name", data.name)) break;
	//		JsonManager::Load(filePath + fileName, basePath + "/size", data.size);
	//		JsonManager::Load(filePath + fileName, basePath + "/center", data.center);
	//		JsonManager::Load(filePath + fileName, basePath + "/rotation", data.rotation);
	//		JsonManager::Load(filePath + fileName, basePath + "/distance", data.distance);
	//		JsonManager::Load(filePath + fileName, basePath + "/fov", data.fov);

	//		int32_t id = Game::Camera::AddCamera();
	//		Game::Camera::Setter::SetCenter(data.center, 0, EaseType::IN_BACK, id);
	//		Game::Camera::Setter::SetDistance(data.distance, 0, EaseType::IN_BACK, id);
	//		Game::Camera::Setter::SetFovTarget(data.fov, 0, EaseType::IN_BACK, id);
	//		Game::Camera::Setter::SetRotate(data.rotation, 0, EaseType::IN_BACK, id);
	//		Game::Camera::Setter::SetScreenSize(data.size, 0, EaseType::IN_BACK, id);

	//		cameraData_[id] = data;

	//		i++;
	//	}
	//}
}

void TestPhase::Save_camera()
{
	const std::string filePath = "assets/application/json/TestPhase/";

	const std::string fileName = "cameras.json";

	int32_t i = 0;
	for (auto [key, value] : cameraData_)
	{
		std::string basePath = "/cameras/" + std::to_string(i);

		JsonManager::AddParam(filePath + fileName, basePath + "/name", value.name);
		JsonManager::AddParam(filePath + fileName, basePath + "/size", value.size);
		JsonManager::AddParam(filePath + fileName, basePath + "/center", value.center);
		JsonManager::AddParam(filePath + fileName, basePath + "/rotation", value.rotation);
		JsonManager::AddParam(filePath + fileName, basePath + "/distance", value.distance);
		JsonManager::AddParam(filePath + fileName, basePath + "/fov", value.fov);

		i++;
	}
	JsonManager::Save(filePath + fileName);
}

void TestPhase::Load_renderObject()
{
	const std::string filePath = "assets/application/json/TestPhase/";

	const std::string fileName = "renderObjects.json";

	//if (JsonManager::Load(filePath + fileName))
	//{
	//	renderObjectData_.clear();
	//
	//	int32_t i = 0;
	//	while (true)
	//	{
	//		std::string basePath = "/render_objects/" + std::to_string(i);
	//
	//		RenderObjectData data;
	//
	//		if (!JsonManager::Load(filePath + fileName, basePath + "/name", data.name)) break;
	//		JsonManager::Load(filePath + fileName, basePath + "/modelPath", data.modelPath);
	//		JsonManager::Load(filePath + fileName, basePath + "/out", data.out);
	//		JsonManager::Load(filePath + fileName, basePath + "/config.vs", data.config.vs);
	//		JsonManager::Load(filePath + fileName, basePath + "/config.ps", data.config.ps);
	//		JsonManager::Load(filePath + fileName, basePath + "/config.ms", data.config.ms);
	//		JsonManager::Load(filePath + fileName, basePath + "/config.dsvFormatID", data.config.dsvFormatID);
	//
	//		renderObjectData_.push_back(data);
	//
	//		i++;
	//	}
	//}
}

void TestPhase::Save_renderObject()
{
	//const std::string filePath = "assets/application/json/TestPhase/";

	//const std::string fileName = "renderObjects.json";

	//for (size_t i = 0; i < renderObjectData_.size(); ++i)
	//{
	//	const auto& value = renderObjectData_[i];
	//	std::string basePath = "/render_objects/" + std::to_string(i);

	//	JsonManager::AddParam(filePath + fileName, basePath + "/name", value.name);
	//	JsonManager::AddParam(filePath + fileName, basePath + "/modelPath", value.modelPath);
	//	JsonManager::AddParam(filePath + fileName, basePath + "/out", value.out);
	//	JsonManager::AddParam(filePath + fileName, basePath + "/config.vs", value.config.vs);
	//	JsonManager::AddParam(filePath + fileName, basePath + "/config.ps", value.config.ps);
	//	JsonManager::AddParam(filePath + fileName, basePath + "/config.ms", value.config.ms);
	//	JsonManager::AddParam(filePath + fileName, basePath + "/config.dsvFormatID", value.config.dsvFormatID);
	//}
	//JsonManager::Save(filePath + fileName);
}

void TestPhase::DrawDebugInfo()
{
	//ImGui::Begin("------debug info------");
	//ImGui::Text("ESC : Quit Application");
	//ImGui::Text("F1  : Hide Debug Info");
	//ImGui::Text("F3  : Toggle Camera Release or Debug");
	//ImGui::Text("F5  : Toggle Camera FirstPerson or ThirdPerson");
	//ImGui::Text("F12 : Toggle Fullscreen");
	//ImGui::Text("DeltaTime: %.3f ms", Game::Time::GetDeltaTime() * 1000.0f);
	//ImGui::Text("FPS: %.1f ", Game::Time::GetFrameRate());
	//ImGui::End();
}
