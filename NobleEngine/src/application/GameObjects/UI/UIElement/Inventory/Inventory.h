#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <array>
#include <string>

class Inventory : public IUIElement
{
public:
	Inventory();
	~Inventory() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
	void DrawImGui() override;

	Vector3 GetSlotPosition(int32_t index) const;

private:
	// グレードアップ先の最大数(枠に収まる3つまで)
	static constexpr int32_t kMaxUpgradeCount = 3;

	Matrix4x4 orthographic_;

	// 右上の枠の1行分(グレードアップ先1つ)
	struct UpgradeRow
	{
		// グレードアップ先
		ItemID resultID = ItemID::MAX;
		// 消費レベル
		int32_t levelCost = 0;
		// レベルが足りているか
		bool canAfford = false;
		// 「消費レベル:N」の文字と位置
		std::string costText;
		Vector2 costTextPos;
		// 矢印の位置
		Vector2 arrowPos;
	};

	// クリック(グレードアップ先・スロットの選択)
	void HandleClick();
	// 右上の枠(ツールのグレードアップ)の中身を更新
	void UpdateUpgradeTable();
	// 右上の枠を描く
	void DrawUpgradeTable(int32_t rt_ID);
	// Inventory.png上の座標(px)を画面の座標に直す
	Vector3 TextureToScreen(float x, float y) const;

	// 4x9 のインベントリ用アイコン
	std::vector<ElementData> inventoryIcons_;
	// 4x2 のアクセサリ用アイコン
	std::vector<ElementData> accessoryIcons_;
	// 装備アップグレード欄で使うアイコン
	std::vector<ElementData> upgradeIcons_;

	// 選んでいるスロット(-1なら選んでいない)
	int32_t selectedSlot_ = -1;
	// グレードアップ先
	std::array<UpgradeRow, kMaxUpgradeCount> upgradeRows_;
	int32_t upgradeRowCount_ = 0;
	// 矢印と消費レベルの文字サイズ(Updateで決める)
	int32_t arrowCharSize_ = 0;
	int32_t costCharSize_ = 0;

	// 採掘ptゲージ
	ElementData gaugeFill_;
	Vector4 gaugeFillColor_ = Vector4{ 0.216f, 1.0f, 0.015f, 1.0f };
	std::string levelText_;
	Vector2 levelTextPos_;
	int32_t levelCharSize_ = 0;
};