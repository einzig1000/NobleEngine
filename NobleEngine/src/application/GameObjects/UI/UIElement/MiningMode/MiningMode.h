#pragma once
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <array>
#include <vector>

class MiningMode : public IUIElement
{
public:
	enum class Part
	{
		Swing,	// 左
		Range,	// 右
	};

	MiningMode();
	~MiningMode() override;
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

	// parts_[パーツ][labelIndex] = sprites_のインデックス
	std::array<std::vector<int32_t>, magic_enum::enum_count<Part>()> parts_;

	// 回転(揺らすときはこれに足す)
	std::array<float, magic_enum::enum_count<Part>()> baseRotateZ_{};
};