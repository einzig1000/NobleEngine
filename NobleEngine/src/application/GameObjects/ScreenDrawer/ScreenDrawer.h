#pragma once
#include <cstdint>
#include <Game.h>

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

	void TakeDamage();

private:
	void DamageEffectUpdate();

	int32_t rt_main_ = -1;	// 最終的に描画されるレンダーテクスチャ

	int32_t rt_3D_ = -1;	// マップとかプレイヤーが描画されるレンダーターゲット
	int32_t rt_3D_depth_ = -1;	// マップとかプレイヤーが描画されるレンダーターゲットの深度
	std::vector<int32_t> rt_3D_Effect_;	// エフェクトをかけていくレンダーターゲット
	int32_t rt_UI_ = -1;	// UIが描画されるレンダーターゲット

	// ヴィネット
	std::unique_ptr<RenderObject> draw_3D_Vignette_;
	float vignette_Brightness = 100000.0f; //16.0f;
	// 深度アウトライン
	std::unique_ptr<RenderObject> draw_3D_DepthBasedOutline_;
	// ぼかし
	std::unique_ptr<RenderObject> draw_3D_GaussianFilter_[2];
	int gaussianFilter_Radius_ = 0; // ぼかし半径
	// グレースケール
	std::unique_ptr<RenderObject> draw_3D_GrayScale_;
	float grayScale_Scale = 0.0f;


	std::unique_ptr<RenderObject> draw_3D_;
	std::unique_ptr<RenderObject> draw_UI_;
	std::unique_ptr<RenderObject> draw_main_;

	EasingSet<float> vignette_Brightness_Easing;

	float frame;
};

