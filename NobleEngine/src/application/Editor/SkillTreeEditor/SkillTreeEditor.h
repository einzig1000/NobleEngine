#pragma once
#include <Game.h>
#include <definition/SkillTreeData/SkillTreeData.h>
#include <GameObjects/UI/SkillTreeView/SkillTreeView.h>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

/// <summary>
/// スキルツリー(SkillTree.json)のエディタ
/// ・一覧でノードを追加・複製・削除
/// ・選んだノードの名前・説明・アイコン・効果・コスト・前提を編集
/// ・プレビュー: クリックで選ぶ / ドラッグで動かす / Ctrl+クリックで前提をつなぐ・外す / 右ドラッグで画面移動 / ホイールでズーム
/// プレビューはゲームと同じ SkillTreeView で描く
/// </summary>
class SkillTreeEditor
{
public:
	SkillTreeEditor();
	~SkillTreeEditor();

	void Update();
	void Draw();
	void DrawImGui();

private:
	// データ置き場(最後に読み込み・保存した内容)に戻す
	void Revert();
	// 保存(ゲームのスキルツリーにもすぐ反映される)
	void Save();

	// ImGuiの各部分
	// 保存・元に戻す
	void DrawToolbar();
	// 一覧と追加・複製・削除
	void DrawNodeList();
	// 選んでいるノードの値
	void DrawSelectedNode();
	// 効果
	bool DrawEffect(SkillNodeData& node);
	// コスト(レベル・素材)
	bool DrawCost(SkillNodeData& node);
	// 前提
	void DrawParents(SkillNodeData& node);
	// プレビュー
	void DrawPreview();

	// 選んでいるノード。無ければ nullptr
	SkillNodeData* GetSelected();
	// 番号から添字。無ければ -1
	int32_t FindIndex(int32_t id) const;
	// まだ使っていない番号
	int32_t MakeNewID() const;
	// ノードを足して選ぶ
	void AddNode(const SkillNodeData& node);
	// ノードを消す(ほかのノードの前提からも消す)
	void DeleteNode(int32_t index);
	// childIndex のノードの前提に parentID をつなぐ(もうつながっていれば外す)。ループになるならつながない
	void ToggleParent(int32_t childIndex, int32_t parentID);
	// nodeID の前提をたどっていくと ancestorID に着くか
	bool IsAncestor(int32_t ancestorID, int32_t nodeID) const;
	// 位置を snap_ の間隔にそろえる
	Vector2 Snap(const Vector2& position) const;
	// プレビューの見え方
	std::vector<SkillTreeView::NodeLook> MakePreviewLooks() const;

	// 編集中のノード
	std::vector<SkillNodeData> nodes_;
	int32_t selected_ = -1;
	// 保存していない変更があるか
	bool isDirty_ = false;

	// プレビュー
	std::unique_ptr<SkillTreeView> view_;
	int32_t hoverNode_ = -1;
	bool isDraggingNode_ = false;
	// 位置をこの間隔にそろえる(0ならそろえない)
	float snap_ = 24.0f;

	// 見え方の確認(オフなら全部「開放済み」で描く)
	bool showLooks_ = false;
	// 確認用に開放済みにしたノード
	std::unordered_set<int32_t> previewUnlocked_;

	// 知らせ(保存した・つなげなかった など)
	std::string message_;
};
