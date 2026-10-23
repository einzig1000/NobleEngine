#include "MiningMode.h"
#include <System/EventBus/EventBus.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <cmath>

namespace
{
	// 選ばれている側のアイコンを揺らす大きさ(ラジアン)
	constexpr float kWobbleAngle = 0.2f;
}

MiningMode::MiningMode()
{
	BuildLayout();
}

MiningMode::~MiningMode()
{}

void MiningMode::Initialize()
{}

void MiningMode::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	// マウスが画面の左半分ならSwing、右半分ならRange
	const Vector2 mouse = GetMouseUIPosition();
	const bool leftKey = mouse.x < static_cast<float>(Constexprs::kInitialWindowWidth) * 0.5f;

	// 選ばれている側のアイコンを揺らす
	const float wobble = std::sinf(Game::Time::GetElapsedSecTime()) * kWobbleAngle;
	if (ElementInstanceData* swing = GetPart(Part::Swing))
	{
		swing->data.rotate.z = baseRotateZ_[static_cast<size_t>(Part::Swing)] + (leftKey ? wobble : 0.0f);
	}
	if (ElementInstanceData* range = GetPart(Part::Range))
	{
		range->data.rotate.z = baseRotateZ_[static_cast<size_t>(Part::Range)] + (leftKey ? 0.0f : wobble);
	}

	// クリックで決定
	if (Game::IO::Mouse::IsJustPressed(0))
	{
		if (eventBus_)
		{
			Event event;
			event.type = EventType::MiningModeChanged;
			MiningPattern pattern = leftKey ? MiningPattern::Swing : MiningPattern::Range;
			event.data = pattern;
			eventBus_->Notify(event);
		}

		if (nextUIMode_)
		{
			*nextUIMode_ = UIScreenType::Playing;
		}
	}

	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void MiningMode::Draw(int32_t rt_ID)
{
	// 静的スプライトの描画
	DrawStaticRenderObject(rt_ID);

}

void MiningMode::DrawImGui()
{
	ImGui::Begin("MiningMode");
	// UIEditorで保存したあとなどに、データを読み直す
	if (ImGui::Button("UIデータを読み直す"))
	{
		BuildLayout();
	}
	ImGui::End();
}

void MiningMode::BuildLayout()
{
	BuildSprites(UIElementType::MiningMode);
	parts_ = FindParts<Part>();

	// Jsonの回転を覚えておく
	for (Part part : magic_enum::enum_values<Part>())
	{
		const ElementInstanceData* sprite = GetPart(part);
		baseRotateZ_[static_cast<size_t>(part)] = sprite ? sprite->data.rotate.z : 0.0f;
	}
}

ElementInstanceData* MiningMode::GetPart(Part part, int32_t labelIndex)
{
	return GetSprite(parts_[static_cast<size_t>(part)], labelIndex);
}