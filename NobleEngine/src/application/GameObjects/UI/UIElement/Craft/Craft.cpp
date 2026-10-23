#include "Craft.h"
#include <Game.h>

Craft::Craft()
{
	// 背景など(UIConfig.json の Craft)
	BuildSprites(UIElementType::Craft);
}

Craft::~Craft()
{}

void Craft::Initialize()
{}

void Craft::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void Craft::Draw(int32_t rt_ID)
{
	// 静的スプライトの描画
	DrawStaticRenderObject(rt_ID);
}

void Craft::DrawImGui()
{}