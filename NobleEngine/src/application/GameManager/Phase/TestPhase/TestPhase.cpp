#include "GameManager/Phase/TestPhase/TestPhase.h"
#include <Utilities/functions.h>

TestPhase::TestPhase()
{
	rt_main_ = Game::Resource::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "Main");
	rt_miniMap_ = Game::Resource::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "MiniMap");
	rt_Vignette_ = Game::Resource::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "Vignette");
	rt_GrayScale_ = Game::Resource::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "GrayScale");

	t_uvChecker = Game::Resource::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	t_monsterBall_ = Game::Resource::LoadTexture("resources/Prototypes/texture/monsterBall.png");
	t_white1x1_ = Game::Resource::LoadTexture("resources/Prototypes/texture/white1x1.png");
	t_dds_ = Game::Resource::LoadTexture("resources/Prototypes/texture/rostock_laage_airport_4k.dds");

	int32_t model1 = Game::Resource::LoadModel("resources/Prototypes/model/cube.obj");
	int32_t model2 = Game::Resource::LoadModel("resources/Prototypes/model/sphere.obj");
	int32_t model3 = Game::Resource::LoadModel("resources/Prototypes/model/plane.obj");

	audio1 = Game::Resource::LoadAudio("resources/Prototypes/audio/BGM/InGame.mp3");
	audio2 = Game::Resource::LoadAudio("resources/Prototypes/audio/SE/バトル用/氷魔法1.mp3");

	cbvOnly_ = std::make_unique<RenderObject>();
	cbvOnly_->modelID_ = model1;
	cbvOnly_->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
	cbvOnly_->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
	cbvOnly_->SetupFromShaders();

	cbvAndSrv_ = std::make_unique<RenderObject>();
	cbvAndSrv_->modelID_ = model1;
	cbvAndSrv_->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModels.PS.hlsl";
	cbvAndSrv_->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModels.VS.hlsl";
	cbvAndSrv_->instanceNum_ = 10;
	cbvAndSrv_->SetupFromShaders();

	line_ = std::make_unique<RenderObject>();
	line_->psoConfig_.ps = "resources/Shaders/Line/Line.PS.hlsl";
	line_->psoConfig_.vs = "resources/Shaders/Line/Line.VS.hlsl";
	line_->SetupFromShaders();

	skybox_ = std::make_unique<RenderObject>();
	skybox_->modelID_ = model1;
	skybox_->psoConfig_.ps = "resources/Shaders/SkyBox/SkyBox.PS.hlsl";
	skybox_->psoConfig_.vs = "resources/Shaders/SkyBox/SkyBox.VS.hlsl";
	skybox_->SetupFromShaders();

	PunctualLight_ = std::make_unique<RenderObject>();
	PunctualLight_->modelID_ = model2;
	PunctualLight_->psoConfig_.ps = "resources/Shaders/PunctualLight/PunctualLight.PS.hlsl";
	PunctualLight_->psoConfig_.vs = "resources/Shaders/PunctualLight/PunctualLight.VS.hlsl";
	PunctualLight_->SetupFromShaders();

	environmentMap_ = std::make_unique<RenderObject>();
	environmentMap_->modelID_ = model2;
	environmentMap_->psoConfig_.ps = "resources/Shaders/EnvironmentMap/EnvironmentMap.PS.hlsl";
	environmentMap_->psoConfig_.vs = "resources/Shaders/EnvironmentMap/EnvironmentMap.VS.hlsl";
	environmentMap_->SetupFromShaders();

	postEffect1_ = std::make_unique<RenderObject>();
	postEffect1_->modelID_ = model3;
	postEffect1_->psoConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	postEffect1_->psoConfig_.ps = "resources/shaders/FullScreen/Vignette.PS.hlsl";
	postEffect1_->psoConfig_.dsvFormatID = DSVFormatID::Unknown;
	postEffect1_->SetupFromShaders();

	postEffect2_ = std::make_unique<RenderObject>();
	postEffect2_->modelID_ = model3;
	postEffect2_->psoConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	postEffect2_->psoConfig_.ps = "resources/shaders/FullScreen/GrayScale.PS.hlsl";
	postEffect2_->psoConfig_.dsvFormatID = DSVFormatID::Unknown;
	postEffect2_->SetupFromShaders();

	screenDrawObjectMain_ = std::make_unique<RenderObject>();
	screenDrawObjectMain_->modelID_ = model3;
	screenDrawObjectMain_->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
	screenDrawObjectMain_->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
	screenDrawObjectMain_->SetupFromShaders();

	screenDrawObjectMiniMap_ = std::make_unique<RenderObject>();
	screenDrawObjectMiniMap_->modelID_ = model3;
	screenDrawObjectMiniMap_->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
	screenDrawObjectMiniMap_->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
	screenDrawObjectMiniMap_->SetupFromShaders();


	transform1_.scale = { 11.0f,11.0f,11.0f };
	color1_ = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	for (int i = 0; i < 10; ++i)
	{
		transform2_[i].scale = { 10.0f,10.0f,10.0f };
		transform2_[i].translate = { static_cast<float>(((i + 1) * 15)), 0.0f, 0.0f };
		color2_[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
		tex2_[i] = t_monsterBall_;
	}

	lightData_.LightCount = 1;

	mainScreenTransform_.scale = { 12.8f,7.2f,0.0f };
	mainScreenTransform_.translate = { 0.0f, 0.0f, 0.0f };
	mainScreenTransform_.rotate = { 0.0f, 0.0f, 0.0f };

	miniMapScreenTransform_.scale = { 0.3f,0.3f,0.3f };
	miniMapScreenTransform_.translate = { 0.6f, -0.6f, 0.0f };
	miniMapScreenTransform_.rotate = { 0.0f, 0.0f, 0.0f };
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
	Matrix4x4 viewMatrix = Game::Camera::Getter::GetCurrentViewMatrix();
	Matrix4x4 projectionMatrix = Game::Camera::Getter::GetCurrentProjectionMatrix();
	Matrix4x4 viewProjection = Game::Camera::Getter::GetCurrentViewProjectionMatrix();
	Matrix4x4 orthoProje = Game::Camera::Getter::GetCurrentOrthoProjectionMatrix();
	Vector3 cameraPos = Game::Camera::Getter::GetCurrentTranslate();

	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform1_.scale, transform1_.rotate, transform1_.translate);
	Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
	
	cbvOnly_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	cbvOnly_->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
	cbvOnly_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	cbvOnly_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);

	std::vector<Matrix4x4> worldMatrices2;
	for (int i = 0; i < 10; ++i)
	{
		Matrix4x4 worldMatrix2 = Matrix4x4::MakeAffineMatrix(transform2_[i].scale, transform2_[i].rotate, transform2_[i].translate);
		worldMatrices2.push_back(worldMatrix2);
	}

	cbvAndSrv_->SetSBufferData(0, ShaderType::PixelShader, &color2_, sizeof(Vector4), 10);
	cbvAndSrv_->SetSBufferData(1, ShaderType::PixelShader, &tex2_, sizeof(int32_t), 10);
	cbvAndSrv_->SetCBufferData(0, ShaderType::VertexShader, &viewProjection);
	cbvAndSrv_->SetSBufferData(0, ShaderType::VertexShader, worldMatrices2.data(), sizeof(Matrix4x4), worldMatrices2.size());

	line_->SetCBufferData(0, ShaderType::VertexShader, &viewProjection);
	line_->SetCBufferData(0, ShaderType::PixelShader, &color1_);

	Matrix4x4 noTranslateView = viewMatrix;
	noTranslateView.m[3][0] = 0.0f;
	noTranslateView.m[3][1] = 0.0f;
	noTranslateView.m[3][2] = 0.0f;
	Matrix4x4 noTranslateViewProjection = noTranslateView * projectionMatrix;

	skybox_->SetCBufferData(0, ShaderType::VertexShader, &noTranslateViewProjection);
	skybox_->SetCBufferData(0, ShaderType::PixelShader, &t_dds_);

	PunctualLight_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	PunctualLight_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	PunctualLight_->SetCBufferData(0, ShaderType::PixelShader, &cameraPos);
	PunctualLight_->SetCBufferData(1, ShaderType::PixelShader, &lightData_);
	PunctualLight_->SetCBufferData(2, ShaderType::PixelShader, &materialData_);
	PunctualLight_->SetCBufferData(3, ShaderType::PixelShader, &t_uvChecker);

	environmentMap_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	environmentMap_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	environmentMap_->SetCBufferData(0, ShaderType::PixelShader, &cameraPos);
	environmentMap_->SetCBufferData(1, ShaderType::PixelShader, &lightData_);
	environmentMap_->SetCBufferData(2, ShaderType::PixelShader, &materialData_);
	environmentMap_->SetCBufferData(3, ShaderType::PixelShader, &t_uvChecker);
	environmentMap_->SetCBufferData(4, ShaderType::PixelShader, &t_dds_);

	// mainの画像をSetCBufferDataしrt_Vignetteに書き込む
	postEffect1_->SetCBufferData(0, ShaderType::PixelShader, &rt_main_);
	// miniMapの画像をSetCBufferDataしrt_GrayScaleに書き込む
	postEffect2_->SetCBufferData(0, ShaderType::PixelShader, &rt_miniMap_);

	Matrix4x4 identityMatrix = Matrix4x4::MakeIdentity4x4();

	Matrix4x4 mainScreenWorldMatrix = Matrix4x4::MakeAffineMatrix(mainScreenTransform_.scale, mainScreenTransform_.rotate, mainScreenTransform_.translate);
	//Matrix4x4 mainScreenWorldViewProjection = mainScreenWorldMatrix * viewProjection;
	//Matrix4x4 mainScreenWorldViewProjection = mainScreenWorldMatrix * orthoProje;
	Matrix4x4 mainScreenWorldViewProjection = identityMatrix;

	// rt_Vignetteの画像をSetCBufferDataしBackBufferに書き込む
	screenDrawObjectMain_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	screenDrawObjectMain_->SetCBufferData(1, ShaderType::PixelShader, &rt_Vignette_);
	screenDrawObjectMain_->SetCBufferData(0, ShaderType::VertexShader, &mainScreenWorldViewProjection);
	screenDrawObjectMain_->SetCBufferData(1, ShaderType::VertexShader, &mainScreenWorldMatrix);

	Matrix4x4 miniMapWorldMatrix = Matrix4x4::MakeAffineMatrix(miniMapScreenTransform_.scale, miniMapScreenTransform_.rotate, miniMapScreenTransform_.translate);
	//Matrix4x4 miniMapWorldViewProjection = miniMapWorldMatrix * viewProjection;
	Matrix4x4 miniMapWorldViewProjection = miniMapWorldMatrix * orthoProje;

	// rt_GrayScaleの画像をSetCBufferDataしBackBufferに書き込む
	screenDrawObjectMiniMap_->SetCBufferData(0, ShaderType::PixelShader, &color1_);
	screenDrawObjectMiniMap_->SetCBufferData(1, ShaderType::PixelShader, &rt_GrayScale_);
	screenDrawObjectMiniMap_->SetCBufferData(0, ShaderType::VertexShader, &miniMapWorldViewProjection);
	screenDrawObjectMiniMap_->SetCBufferData(0, ShaderType::VertexShader, &miniMapWorldMatrix);
}

void TestPhase::Draw()
{
	if (rt_main_ != Game::Resource::GetRenderTextureID("Main"))
	{
		DebugBreak();
	}

	// mainに書き込む
	cbvOnly_->Draw(rt_main_);
	// miniMapに書き込む
	cbvAndSrv_->Draw(rt_miniMap_);

	//line_->Draw();
	skybox_->Draw(rt_main_);
	//PunctualLight_->Draw();
	//environmentMap_->Draw(rt_main_);

	// mainの画像をSetCBufferDataしrt_Vignetteに書き込む
	postEffect1_->PostEffectDraw(rt_Vignette_);
	// miniMapの画像をSetCBufferDataしrt_GrayScaleに書き込む
	postEffect2_->PostEffectDraw(rt_GrayScale_);

	// rt_Vignetteの画像をSetCBufferDataしBackBufferに書き込む
	screenDrawObjectMain_->ScreenDraw();
	// rt_GrayScaleの画像をSetCBufferDataしBackBufferに書き込む
	screenDrawObjectMiniMap_->ScreenDraw();
}


void TestPhase::DrawImGui()
{
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
				for (int i = 0; i < 10; ++i)
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
				ImGui::DragFloat3("scale", &mainScreenTransform_.scale.x, 0.1f, 0.1f, 100.0f);
				ImGui::DragFloat3("rotate", &mainScreenTransform_.rotate.x, 0.1f);
				ImGui::DragFloat3("translate", &mainScreenTransform_.translate.x, 0.1f, -100.0f, 100.0f);

				ImGui::Separator();

				ImGui::DragFloat3("miniMap scale", &miniMapScreenTransform_.scale.x, 0.1f, 0.1f, 100.0f);
				ImGui::DragFloat3("miniMap rotate", &miniMapScreenTransform_.rotate.x, 0.1f);
				ImGui::DragFloat3("miniMap translate", &miniMapScreenTransform_.translate.x, 0.1f, -100.0f, 100.0f);

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

#pragma region light test

		if (ImGui::BeginTabItem("Light Test"))
		{
			static Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
			ImGui::ColorEdit4("light color", &lightColor.x, 1);
			Game::Light::SetLightColor(lightColor);

			static Vector3 lightDirection = { 0.0f,-1.0f,0.0f };
			lightDirection.Normalize();
			ImGui::DragFloat3("light direction", &lightDirection.x, 0.1f);
			Game::Light::SetLightDirection(lightDirection);

			static float lightIntensity = 1.0f;
			ImGui::SliderFloat("light intensity", &lightIntensity, 0.0f, 10.0f);
			Game::Light::SetLightIntensity(lightIntensity);

			static int current_item = 0;
			static const char* items[] = { "None", "Lambert", "HalfLambert" };
			if (ImGui::Combo("dirLight.shadingType", &current_item, items, IM_ARRAYSIZE(items)))
			{
				LightMode mode = LightMode::None;
				if (current_item == 0)
				{
					mode = LightMode::None;
				}
				else if (current_item == 1)
				{
					mode = LightMode::Lambert;
				}
				else if (current_item == 2)
				{
					mode = LightMode::HalfLambert;
				}
				Game::Light::ToggleLightMode(mode);
			}



			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region camera test

		if (ImGui::BeginTabItem("Camera Test"))
		{
			static Vector3 cameraCenterTarget;
			static int cameraCenterFrame = 120;
			ImGui::DragFloat3("camera center", &cameraCenterTarget.x, 0.1f);
			ImGui::DragInt("camera center frame", &cameraCenterFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Center"))
			{
				Game::Camera::MoveCameraCenter(cameraCenterTarget, cameraCenterFrame, EaseType::IN_CUBIC);
			}

			static Vector3 cameraRotateTarget;
			static int cameraRotateFrame = 120;
			ImGui::DragFloat3("camera rotate", &cameraRotateTarget.x, 0.1f);
			ImGui::DragInt("camera rotate frame", &cameraRotateFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Rotate"))
			{
				Game::Camera::MoveCameraRotate(cameraRotateTarget, cameraRotateFrame, EaseType::IN_CUBIC);
			}

			static float cameraDistanceTarget = 0.0f;
			static int cameraDistanceFrame = 120;
			ImGui::DragFloat("camera distance", &cameraDistanceTarget, 0.1f);
			ImGui::DragInt("camera distance frame", &cameraDistanceFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Distance"))
			{
				Game::Camera::MoveCameraDistance(cameraDistanceTarget, cameraDistanceFrame, EaseType::IN_CUBIC);
			}

			static float intensity = 3.0f;
			static float duration = 35.0f;
			static float frequency = 15.0f;
			ImGui::DragFloat("camera shake intensity", &intensity, 0.1f);
			ImGui::DragFloat("camera shake duration", &duration, 0.1f);
			ImGui::DragFloat("camera shake frequency", &frequency, 0.1f);
			if (ImGui::Button("Start Camera Shake"))
			{
				Game::Camera::StartCameraShake(intensity, duration, frequency);
			}
			ImGui::Text("is camera shaking : %d", Game::Camera::IsShaking());
			if (ImGui::Button("Stop Camera Shake"))
			{
				Game::Camera::StopShake();
			}

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region mouse test

		if (ImGui::BeginTabItem("mouse Test"))
		{
			ImGui::Text("Mouse Position: (%.1f, %.1f)", Game::IO::Mouse::GetPosition().x, Game::IO::Mouse::GetPosition().y);
			ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", Game::IO::Mouse::GetWorldPosition().x, Game::IO::Mouse::GetWorldPosition().y, Game::IO::Mouse::GetWorldPosition().z);
			ImGui::Text("Mouse Ray Origin: (%.1f, %.1f, %.1f)", Game::IO::Mouse::GetRay().origin.x, Game::IO::Mouse::GetRay().origin.y, Game::IO::Mouse::GetRay().origin.z);
			ImGui::Text("Mouse Ray Diff  : (%.1f, %.1f, %.1f)", Game::IO::Mouse::GetRay().diff.x, Game::IO::Mouse::GetRay().diff.y, Game::IO::Mouse::GetRay().diff.z);
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
			struct KeyInfo
			{
				const char* name;
				int dik;
			};

			static const KeyInfo kKeys[] = {
				{"A", DIK_A}, {"B", DIK_B}, {"C", DIK_C}, {"D", DIK_D},
				{"E", DIK_E}, {"F", DIK_F}, {"G", DIK_G}, {"H", DIK_H},
				{"I", DIK_I}, {"J", DIK_J}, {"K", DIK_K}, {"L", DIK_L},
				{"M", DIK_M}, {"N", DIK_N}, {"O", DIK_O}, {"P", DIK_P},
				{"Q", DIK_Q}, {"R", DIK_R}, {"S", DIK_S}, {"T", DIK_T},
				{"U", DIK_U}, {"V", DIK_V}, {"W", DIK_W}, {"X", DIK_X},
				{"Y", DIK_Y}, {"Z", DIK_Z},

				{"Space", DIK_SPACE},

				{"F1", DIK_F1}, {"F2", DIK_F2}, {"F3", DIK_F3}, {"F4", DIK_F4},
				{"F5", DIK_F5}, {"F6", DIK_F6}, {"F7", DIK_F7}, {"F8", DIK_F8},
				{"F9", DIK_F9}, {"F10", DIK_F10}, {"F11", DIK_F11}, {"F12", DIK_F12},

				{"Enter", DIK_RETURN}, {"Escape", DIK_ESCAPE},

				{"Up", DIK_UP}, {"Down", DIK_DOWN}, {"Left", DIK_LEFT}, {"Right", DIK_RIGHT},

				{"LShift", DIK_LSHIFT}, {"RShift", DIK_RSHIFT},

				{"0", DIK_0}, {"1", DIK_1}, {"2", DIK_2}, {"3", DIK_3}, {"4", DIK_4},
				{"5", DIK_5}, {"6", DIK_6}, {"7", DIK_7}, {"8", DIK_8}, {"9", DIK_9}
			};

			for (const auto& k : kKeys)
			{
				if (Game::IO::Key::IsHeld(k.dik) ||
					Game::IO::Key::IsJustPressed(k.dik) ||
					Game::IO::Key::IsJustReleased(k.dik))
				{
					ImGui::Text("%s : %d:%d:%d _ %d",
						k.name,
						Game::IO::Key::IsJustPressed(k.dik),
						Game::IO::Key::IsHeld(k.dik),
						Game::IO::Key::IsJustReleased(k.dik),
						Game::IO::Key::HoldFrames(k.dik)
					);
				}
			}
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

	ImGui::Begin("QuaternionFunctionsTest");
	if (ImGui::BeginTabBar("QuaternionFunctionsTest", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable))
	{
		if (ImGui::BeginTabItem("01-02"))
		{
			Vector3 from0 = Vector3{ 1.0f,0.7f,0.5f }.Normalize();
			Vector3 to0 = -from0;
			Vector3 from1 = Vector3{ -0.6f,0.9f,0.2f }.Normalize();
			Vector3 to1 = Vector3{ 0.4f,0.7f,-0.5f }.Normalize();
			Matrix4x4 rotateMatrix0 = Matrix4x4::DirectionToDirectionMatrix(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ -1.0f,0.0f,0.0f });
			Matrix4x4 rotateMatrix1 = Matrix4x4::DirectionToDirectionMatrix(from0, to0);
			Matrix4x4 rotateMatrix2 = Matrix4x4::DirectionToDirectionMatrix(from1, to1);

			ImGui::Text("from0 : (%5.2f, %5.2f, %5.2f)", from0.x, from0.y, from0.z);
			ImGui::Text("to0   : (%5.2f, %5.2f, %5.2f)", to0.x, to0.y, to0.z);
			ImGui::Text("from1 : (%5.2f, %5.2f, %5.2f)", from1.x, from1.y, from1.z);
			ImGui::Text("to1   : (%5.2f, %5.2f, %5.2f)", to1.x, to1.y, to1.z);
			ImGui::Text("rotateMatrix0 :");
			ImGui::NewLine();
			for (int j = 0; j < 4; ++j)
			{
				for (int i = 0; i < 4; ++i)
				{
					ImGui::SameLine();
					ImGui::Text("%6.3f ", rotateMatrix0.m[j][i]);
				}
				ImGui::NewLine();
			}
			ImGui::Text("rotateMatrix1 :");
			ImGui::NewLine();
			for (int j = 0; j < 4; ++j)
			{
				for (int i = 0; i < 4; ++i)
				{
					ImGui::SameLine();
					ImGui::Text("%6.3f ", rotateMatrix1.m[j][i]);
				}
				ImGui::NewLine();
			}
			ImGui::Text("rotateMatrix2 :");
			ImGui::NewLine();
			for (int j = 0; j < 4; ++j)
			{
				for (int i = 0; i < 4; ++i)
				{
					ImGui::SameLine();
					ImGui::Text("%6.3f ", rotateMatrix2.m[j][i]);
				}
				ImGui::NewLine();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("01-03"))
		{
			Quaternion q0 = Quaternion{ 2.0f,3.0f,4.0f,1.0f };
			Quaternion q1 = Quaternion{ 1.0f,3.0f,5.0f,2.0f };
			Quaternion identity = Quaternion::MakeIdentityQuaternion();
			Quaternion conj = q0.MakeConjugateQuaternion();
			Quaternion inv = q0.Inverse();
			Quaternion normal = q0.Normalize();
			Quaternion mul1 = q0 * q1;
			Quaternion mul2 = q1 * q0;
			float norm = q0.Norm();

			ImGui::Text("q1       : (%5.2f, %5.2f, %5.2f, %5.2f)", q0.x, q0.y, q0.z, q0.w);
			ImGui::Text("q2       : (%5.2f, %5.2f, %5.2f, %5.2f)", q1.x, q1.y, q1.z, q1.w);
			ImGui::Text("identity : (%5.2f, %5.2f, %5.2f, %5.2f)", identity.x, identity.y, identity.z, identity.w);
			ImGui::Text("conj     : (%5.2f, %5.2f, %5.2f, %5.2f)", conj.x, conj.y, conj.z, conj.w);
			ImGui::Text("inv      : (%5.2f, %5.2f, %5.2f, %5.2f)", inv.x, inv.y, inv.z, inv.w);
			ImGui::Text("normal   : (%5.2f, %5.2f, %5.2f, %5.2f)", normal.x, normal.y, normal.z, normal.w);
			ImGui::Text("mul1     : (%5.2f, %5.2f, %5.2f, %5.2f)", mul1.x, mul1.y, mul1.z, mul1.w);
			ImGui::Text("mul2     : (%5.2f, %5.2f, %5.2f, %5.2f)", mul2.x, mul2.y, mul2.z, mul2.w);
			ImGui::Text("norm     : %5.2f", norm);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("01-04"))
		{
			Quaternion rotation =
				Quaternion::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.4f, -0.2f).Normalize(), 0.45f);
			Matrix4x4 rotateMatrix = rotation.MakeRotateMatrix();
			Vector3 pointY = Vector3(2.1f, -0.9f, 1.3f);
			Vector3 rotateByQuaternion = pointY.RotateByQuaternion(rotation);
			Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

			ImGui::Text("%5.2f, %5.2f, %5.2f %5.2f : rotation",
				rotation.x, rotation.y, rotation.z, rotation.w);
			ImGui::Text("rotateMatrix:\n");
			ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f",
				rotateMatrix.m[0][0], rotateMatrix.m[1][0], rotateMatrix.m[2][0], rotateMatrix.m[3][0]);
			ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f",
				rotateMatrix.m[0][1], rotateMatrix.m[1][1], rotateMatrix.m[2][1], rotateMatrix.m[3][1]);
			ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f",
				rotateMatrix.m[0][2], rotateMatrix.m[1][2], rotateMatrix.m[2][2], rotateMatrix.m[3][2]);
			ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f",
				rotateMatrix.m[0][3], rotateMatrix.m[1][3], rotateMatrix.m[2][3], rotateMatrix.m[3][3]);
			ImGui::Text("%5.2f, %5.2f, %5.2f : rotateByQuaternion",
				rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z);
			ImGui::Text("%5.2f, %5.2f, %5.2f : rotateByMatrix",
				rotateByMatrix.x, rotateByMatrix.y, rotateByMatrix.z);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
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

