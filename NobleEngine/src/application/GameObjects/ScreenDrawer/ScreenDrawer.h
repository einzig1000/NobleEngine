#pragma once
#include <cstdint>
#include <Game.h>

class EventBus;

class ScreenDrawer
{
public:
	ScreenDrawer();
	~ScreenDrawer();
	void Update(int32_t cameraID);
	void Draw();
	void DrawImGui();

	int32_t Get3DRenderTexture() const { return rt_3D_; }
	int32_t GetUIRenderTexture() const { return rt_UI_; }
	int32_t GetBackgroundRenderTexture() const { return rt_Background_; }

	void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }

	void TakeDamage();

private:
	EventBus* eventBus_ = nullptr;

	void DamageEffectUpdate();

	int32_t rt_main_ = -1;	// 最終的に描画されるレンダーテクスチャ

	int32_t rt_3D_ = -1;	// マップとかプレイヤーが描画されるレンダーターゲット
	int32_t rt_3D_depth_ = -1;	// マップとかプレイヤーが描画されるレンダーターゲットの深度
	int32_t rt_Background_ = -1;	// 背景が描画されるレンダーターゲット
	int32_t rt_UI_ = -1;	// UIが描画されるレンダーターゲット

	std::vector<int32_t> rt_PostEffect_;


	// ヴィネット
	std::unique_ptr<RenderObject> draw_3D_Vignette_;
	float vignette_Brightness = 100000.0f; //16.0f;

	// 深度アウトライン
	std::unique_ptr<RenderObject> draw_3D_DepthBasedOutline_;

	// ぼかし
	std::unique_ptr<RenderObject> draw_3D_GaussianFilter_[2];
	int gaussianFilter_Radius_ = 0; // ぼかし半径
	Vector2 texelSize;

	// グレースケール
	std::unique_ptr<RenderObject> draw_3D_GrayScale_;
	float grayScale_Scale = 0.0f;


	std::unique_ptr<RenderObject> draw_3D_;
	std::unique_ptr<RenderObject> draw_UI_;
	std::unique_ptr<RenderObject> draw_Background_;
	std::unique_ptr<RenderObject> draw_main_;

	EasingSet<float> damageEffect_EasingSet;

	float frame = 0.0f;
};

