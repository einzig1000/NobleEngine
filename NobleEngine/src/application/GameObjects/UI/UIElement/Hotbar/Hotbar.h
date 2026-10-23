#pragma once
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <array>
#include <vector>

class Hotbar : public IUIElement
{
public:
	enum class Part
	{
		Slots,
	};

public:
	Hotbar();
	~Hotbar() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
	void DrawImGui() override;

private:
	Matrix4x4 orthographic_;

	// Jsonのデータからsprites_を作る
	void BuildLayout();
	// PartのlabelIndex番目のスプライト
	ElementInstanceData* GetPart(Part part, int32_t labelIndex = 0);

	// 選んでいるスロット
	int32_t selectedIndex_ = 0;

	// 個数の文字の座標オフセット
	Vector2 countTextOffset_;

	// parts_[パーツ][labelIndex] = sprites_のインデックス
	std::array<std::vector<int32_t>, magic_enum::enum_count<Part>()> parts_;

	// アイコン
	std::vector<ElementInstanceData> icons_;
};
