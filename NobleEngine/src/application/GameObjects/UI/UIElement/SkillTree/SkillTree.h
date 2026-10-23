#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <array>
#include <string>

class SkillTree : public IUIElement
{
public:
	enum class Part
	{
		MainSlots,		// メイン3段のスロット		(labelIndex 0〜26 = インベントリの9〜35番)
		HotbarSlots,	// ホットバー段のスロット	(labelIndex 0〜8 = インベントリの0〜8番)
		AccessorySlots,	// アクセサリのスロット		(labelIndex 0〜7)
		GaugeFill,		// 採掘ptゲージの塗り		(Jsonのscaleが満タンの大きさ。pivotを左端にすると左から伸びる)
		LevelText,		// レベルの数字の中心		(テクスチャなしの目印)
	};

public:
	SkillTree();
	~SkillTree() override;
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
	// インベントリの番号(0〜35)のスロットのスプライト。0〜8はホットバー段、9〜35はメイン3段
	ElementInstanceData* GetInventorySlotSprite(int32_t index);
	// クリック(スロットの選択)
	void HandleClick();

	// parts_[パーツ][labelIndex] = sprites_のインデックス
	std::array<std::vector<int32_t>, magic_enum::enum_count<Part>()> parts_;

	// アイコン
	std::vector<ElementInstanceData> inventoryIcons_;
	std::vector<ElementInstanceData> accessoryIcons_;

	// 選んでいるスロット
	int32_t selectedSlot_ = -1;

	// 個数の文字の座標オフセット
	Vector2 countTextOffset_;

	// 採掘ptゲージ
	Vector2 gaugeFullScale_;
	Vector4 gaugeFillColor_ = Vector4{ 0.216f, 1.0f, 0.015f, 1.0f };
	std::string levelText_;
	Vector2 levelTextPos_;
	int32_t levelCharSize_ = 46;
};