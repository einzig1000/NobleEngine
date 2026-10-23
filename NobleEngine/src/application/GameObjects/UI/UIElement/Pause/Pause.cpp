#include "Pause.h"
#include <System/EventBus/EventBus.h>

Pause::Pause()
{
	// 背景・ボタンなど(UIConfig.json の Pause)
	BuildSprites(UIElementType::Pause);
}

Pause::~Pause()
{}

void Pause::Initialize()
{}

void Pause::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);



	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void Pause::Draw(int32_t rt_ID)
{
	// 静的スプライトの描画
	DrawStaticRenderObject(rt_ID);
}

void Pause::DrawImGui()
{}