#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <definition/SkillTreeData/SkillTreeData.h>
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <GameObjects/UI/SkillTreeView/SkillTreeView.h>
#include <GameObjects/Character/SkillState/SkillState.h>
#include <array>
#include <memory>
#include <string>
#include <vector>

class SkillTree : public IUIElement
{
public:
	enum class Part
	{
		TreeArea,		// ツリーを映す場所(テクスチャなしの目印。scale がそのまま大きさpx。絵はツリーのレンダーテクスチャになる)
		InventoryTab,	// インベントリに戻るタブ(クリック判定。テクスチャなしの目印なら scale が大きさpx)
		LevelText,		// レベルの数字の中心(テクスチャなしの目印)
	};

public:
	SkillTree();
	~SkillTree() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
	void DrawImGui() override;

private:
	// ツールチップ1行分
	struct TooltipLine
	{
		std::string text;
		Vector4 color;
	};

	// Jsonのデータからsprites_を作る
	void BuildLayout();
	// PartのlabelIndex番目のスプライト
	ElementInstanceData* GetPart(Part part, int32_t labelIndex = 0);
	// ノードのデータが変わっていたら取り直す(エディタで保存したときなど)
	void RefreshNodes();
	// ノードの状態を調べて見え方を決める
	void UpdateStatuses();
	// マウス操作(ホイールでズーム・ドラッグで移動・クリックで開放・タブ)
	void UpdateInput();
	// ツールチップの中身と位置
	void UpdateTooltip();
	// ノードをクリックしたとき
	void OnClickNode(int32_t nodeIndex);

	Matrix4x4 orthographic_;

	// parts_[パーツ][labelIndex] = sprites_のインデックス
	std::array<std::vector<int32_t>, magic_enum::enum_count<Part>()> parts_;

	// ツリーの絵(専用のレンダーテクスチャに描く)
	std::unique_ptr<SkillTreeView> view_;
	// 表示中のノード(データ置き場のコピー)と、その版
	std::vector<SkillNodeData> nodes_;
	int32_t nodesRevision_ = -1;
	// ノードごとの状態と見え方(nodes_ と同じ並び)
	std::vector<SkillNodeStatus> statuses_;
	std::vector<SkillTreeView::NodeLook> looks_;

	// ドラッグ
	bool isPressing_ = false;	// ツリーの上で左ボタンを押している
	bool isDragging_ = false;	// 押してから一定以上動いた(=クリックではない)
	Vector2 pressMouse_;
	Vector2 prevMouse_;
	// カーソルが乗っているノード(nodes_ の添字。-1なら無し)
	int32_t hoverNode_ = -1;

	// ツールチップ
	std::vector<TooltipLine> tooltipLines_;
	Vector2 tooltipPos_;	// 左上
	ElementInstanceData tooltipBack_;
	int32_t tooltipCharSize_ = 24;

	// レベル
	std::string levelText_;
	Vector2 levelTextPos_;
	int32_t levelCharSize_ = 46;
	Vector4 levelColor_ = Vector4{ 0.216f, 1.0f, 0.015f, 1.0f };
};