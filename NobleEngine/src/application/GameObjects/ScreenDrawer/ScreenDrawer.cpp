#include "ScreenDrawer.h"
#include <GameObjects/EventBus/EventBus.h>

ScreenDrawer::ScreenDrawer()
{
	rt_3D_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "3D");
	rt_3D_depth_ = Game::Asset::RenderTexture::GetRenderTextureDepthID("3D");
	rt_UI_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "UI");
	rt_Background_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "Background");
	rt_main_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "Main");

	for (int i = 0; i < 5; i++)
	{
		rt_PostEffect_.push_back(Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "PostEffect" + std::to_string(i)));
	}

	int32_t m_plane = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");


	texelSize = Vector2(1.0f / Game::Window::GetWidth(), 1.0f / Game::Window::GetHeight());

	draw_3D_Vignette_ = std::make_unique<RenderObject>();
	draw_3D_Vignette_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_3D_Vignette_->psoConfig_.ps = "assets/shaders/FullScreen/Vignette.PS.hlsl";
	draw_3D_Vignette_->modelID_ = m_plane;
	draw_3D_Vignette_->SetupFromShaders();

	draw_3D_DepthBasedOutline_ = std::make_unique<RenderObject>();
	draw_3D_DepthBasedOutline_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_3D_DepthBasedOutline_->psoConfig_.ps = "assets/shaders/FullScreen/DepthBasedOutline.PS.hlsl";
	draw_3D_DepthBasedOutline_->modelID_ = m_plane;
	draw_3D_DepthBasedOutline_->SetupFromShaders();

	draw_3D_GaussianFilter_[0] = std::make_unique<RenderObject>();
	draw_3D_GaussianFilter_[0]->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_3D_GaussianFilter_[0]->psoConfig_.ps = "assets/shaders/FullScreen/GaussianFilterFirst.PS.hlsl";
	draw_3D_GaussianFilter_[0]->modelID_ = m_plane;
	draw_3D_GaussianFilter_[0]->SetupFromShaders();

	draw_3D_GaussianFilter_[1] = std::make_unique<RenderObject>();
	draw_3D_GaussianFilter_[1]->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_3D_GaussianFilter_[1]->psoConfig_.ps = "assets/shaders/FullScreen/GaussianFilterSecond.PS.hlsl";
	draw_3D_GaussianFilter_[1]->modelID_ = m_plane;
	draw_3D_GaussianFilter_[1]->SetupFromShaders();

	draw_3D_GrayScale_ = std::make_unique<RenderObject>();
	draw_3D_GrayScale_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_3D_GrayScale_->psoConfig_.ps = "assets/shaders/FullScreen/GrayScale.PS.hlsl";
	draw_3D_GrayScale_->modelID_ = m_plane;
	draw_3D_GrayScale_->SetupFromShaders();

	draw_UI_ = std::make_unique<RenderObject>();
	draw_UI_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_UI_->psoConfig_.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	draw_UI_->modelID_ = m_plane;
	draw_UI_->SetupFromShaders();

	draw_3D_ = std::make_unique<RenderObject>();
	draw_3D_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_3D_->psoConfig_.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	draw_3D_->modelID_ = m_plane;
	draw_3D_->SetupFromShaders();

	draw_Background_ = std::make_unique<RenderObject>();
	draw_Background_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_Background_->psoConfig_.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	draw_Background_->modelID_ = m_plane;
	draw_Background_->SetupFromShaders();

	draw_main_ = std::make_unique<RenderObject>();
	draw_main_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_main_->psoConfig_.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	draw_main_->modelID_ = m_plane;
	draw_main_->SetupFromShaders();
}

ScreenDrawer::~ScreenDrawer()
{}

void ScreenDrawer::Update(int32_t cameraID)
{
	if (eventBus_)
	{
		const std::vector<Event>& events = eventBus_->GetEvents(EventType::PlayerDamaged);
		if (!events.empty())
		{
			TakeDamage();
			eventBus_->Clear(EventType::PlayerDamaged);
		}
	}

	DamageEffectUpdate();

	// 背景書き込み
	// 書き込み先：rt_PostEffect_[0]
	// 参照元：rt_3D_, rt_3D_depth_, rt_Background_
	draw_Background_->SetCBufferData(0, ShaderType::PixelShader, &rt_Background_);
	draw_Background_->Draw(rt_PostEffect_[0], { rt_3D_, rt_3D_depth_, rt_Background_ });

	// 3D描画
	// 書き込み先：rt_PostEffect_[0]
	// 参照元：rt_3D_, rt_3D_depth_, rt_Background_
	draw_3D_->SetCBufferData(0, ShaderType::PixelShader, &rt_3D_);
	draw_3D_->Draw(rt_PostEffect_[0], { rt_3D_, rt_3D_depth_, rt_Background_ });

	// アウトライン描画
	// 書き込み先：rt_PostEffect_[0]
	// 参照元：rt_3D_、rt_3D_depth_
	draw_3D_DepthBasedOutline_->SetCBufferData(0, ShaderType::PixelShader, &rt_PostEffect_[0]);
	draw_3D_DepthBasedOutline_->SetCBufferData(1, ShaderType::PixelShader, &rt_3D_depth_);
	Matrix4x4 projectionInverse = Game::Camera::Getter::GetProjectionMatrix(cameraID).Inverse();
	draw_3D_DepthBasedOutline_->SetCBufferData(2, ShaderType::PixelShader, &projectionInverse);
	draw_3D_DepthBasedOutline_->Draw(rt_PostEffect_[1], { rt_PostEffect_[0], rt_3D_depth_ });

	// vignette描画
	// 書き込み先：rt_PostEffect_[1]
	// 参照元：rt_PostEffect_[0]
	draw_3D_Vignette_->SetCBufferData(0, ShaderType::PixelShader, &rt_PostEffect_[1]);
	draw_3D_Vignette_->SetCBufferData(1, ShaderType::PixelShader, &vignette_Brightness);
	draw_3D_Vignette_->Draw(rt_PostEffect_[2], { rt_PostEffect_[1] });

	// ガウシアンフィルタ描画
	// 書き込み先：rt_PostEffect_[2]
	// 参照元：rt_PostEffect_[1]
	draw_3D_GaussianFilter_[0]->SetCBufferData(0, ShaderType::PixelShader, &rt_PostEffect_[2]);
	draw_3D_GaussianFilter_[0]->SetCBufferData(1, ShaderType::PixelShader, &gaussianFilter_Radius_);
	draw_3D_GaussianFilter_[0]->SetCBufferData(2, ShaderType::PixelShader, &texelSize);
	draw_3D_GaussianFilter_[0]->Draw(rt_PostEffect_[3], { rt_PostEffect_[2] });

	// ガウシアンフィルタ描画
	// 書き込み先：rt_PostEffect_[3]
	// 参照元：rt_PostEffect_[2]
	draw_3D_GaussianFilter_[1]->SetCBufferData(0, ShaderType::PixelShader, &rt_PostEffect_[3]);
	draw_3D_GaussianFilter_[1]->SetCBufferData(1, ShaderType::PixelShader, &gaussianFilter_Radius_);
	draw_3D_GaussianFilter_[1]->SetCBufferData(2, ShaderType::PixelShader, &texelSize);
	draw_3D_GaussianFilter_[1]->Draw(rt_PostEffect_[4], { rt_PostEffect_[3] });

	// グレースケール描画
	// 書き込み先：rt_PostEffect_[4]
	// 参照元：rt_PostEffect_[3]
	draw_3D_GrayScale_->SetCBufferData(0, ShaderType::PixelShader, &rt_PostEffect_[4]);
	draw_3D_GrayScale_->SetCBufferData(1, ShaderType::PixelShader, &grayScale_Scale);
	draw_3D_GrayScale_->Draw(rt_main_, { rt_UI_, rt_PostEffect_[4] });

	// UI描画
	// 書き込み先：rt_main_
	// 参照元：rt_Background_, rt_UI_, rt_PostEffect_[4]
	draw_UI_->SetCBufferData(0, ShaderType::PixelShader, &rt_UI_);
	draw_UI_->Draw(rt_main_, { rt_UI_, rt_PostEffect_[4] });

	// メイン描画
	// 書き込み先：エンジンのデフォルトレンダーターゲット
	// 参照元：rt_main_
	draw_main_->SetCBufferData(0, ShaderType::PixelShader, &rt_main_);
	draw_main_->Draw(-1, { rt_main_ });
}

void ScreenDrawer::Draw()
{
}

void ScreenDrawer::DrawImGui()
{
	ImGui::Begin("ScreenDrawer");
	ImGui::DragFloat("Vignette Brightness", &vignette_Brightness);
	ImGui::DragFloat("GrayScale Scale", &grayScale_Scale, 0.01f);
	ImGui::DragInt("GaussianFilter Radius", &gaussianFilter_Radius_, 1, 1, 100);
	ImGui::End();
}

void ScreenDrawer::TakeDamage()
{
	frame = 3.0f;
}

void ScreenDrawer::DamageEffectUpdate()
{
	if (frame > 0)
	{
		frame -= 0.01f;
		vignette_Brightness = Game::Math::Ease::Easing(12.0f, 100000.0f, EaseType::IN_QUINT, (3.0f - frame) / 3.0f);
		gaussianFilter_Radius_ = static_cast<int32_t>(Game::Math::Ease::Easing(20.0f, 0.0f, EaseType::LINEAR, (3.0f - frame) / 3.0f));
		grayScale_Scale = Game::Math::Ease::Easing(1.0f, 0.0f, EaseType::LINEAR, (3.0f - frame) / 3.0f);
	}
	else
	{
		vignette_Brightness = 100000.0f;
		gaussianFilter_Radius_ = 0;
		grayScale_Scale = 0.0f;
	}
}
