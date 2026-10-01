#pragma once
#include <Game.h>
#include <memory>
#include <definition/definition.h>

class DataManager;

class ItemEditor
{
public:
	ItemEditor(DataManager* dataManager);
	~ItemEditor();

	void Update();
	void Draw();
	void DrawImGui();

private:
	void DrawIconImGui();

	DataManager* dataManager_;

	int32_t renderTextureID_ = -1;
	int32_t cameraID_ = -1;

	std::unique_ptr<RenderObject> renderObject_;
	int32_t textureID_ = -1;
	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	EulerTransforms transforms_;

	ItemGenre genre_ = ItemGenre::MAX;
	ToolID toolID_ = ToolID::MAX;
	ToolInfo toolInfo{};
	BlockID blockID_ = BlockID::MAX;
	BlockInfo blockInfo{};
	ObjectID objectID_ = ObjectID::MAX;
	ObjectInfo objectInfo{};

	bool fullscreen_ = false;

	// アイコンのカメラ調整
	// 調整中のアイテム(MAXなら何も選んでいない)
	ItemID iconItemID_ = ItemID::MAX;
	// 調整中の値(Saveするまでゲームには反映されない)
	ItemInfo iconItemInfo_{};
	// プレビュー(ゲーム内のアイコンと同じ描き方で描く)
	int32_t iconPreviewTextureID_ = -1;
	std::unique_ptr<RenderObject> iconRenderObject_;
	// プレビューを1回でも描いたか(まだ描いていないレンダーテクスチャは表示しない)
	bool iconPreviewDrawn_ = false;
};