#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <array>
#include <memory>

/// <summary>
/// アイテムのアイコンを、モデルをレンダーテクスチャに描いて作る
/// UIが初めてそのアイテムのアイコンを頼んだときに作る
/// </summary>
class ItemIconManager
{
public:
	// アイコンの大きさ(px)。UIでの表示(64x64)と同じにして、そのままの大きさで貼る
	static constexpr uint32_t kIconSize = 64;

	ItemIconManager();
	~ItemIconManager();

	// アイコンのrt_IDを取得。まだ無ければ作る
	int32_t GetIcon(ItemID id);
	// 描く必要のあるアイコンを描く。UIを描く前に毎フレーム呼ぶ
	void Draw();

	// アイコン用のRenderObjectを作る(ItemEditorのプレビューでも使う)
	static std::unique_ptr<RenderObject> CreateRenderObject();
	// アイテム1つを、アイコンのカメラで指定のレンダーテクスチャに描く(ItemEditorのプレビューでも使う)
	static void DrawItem(RenderObject& render, const ItemInfo& info, int32_t renderTextureID);

private:
	// アイコン1つ分
	struct Icon
	{
		// 描き込むレンダーテクスチャ(-1ならまだ作っていない)
		int32_t renderTextureID = -1;
		// 描くときに使う(同じフレームに複数描くことがあるので、アイコンごとに持つ)
		std::unique_ptr<RenderObject> render;
	};

	// モデルが画面に収まるようにカメラを置いて、ビュープロジェクション行列を作る
	static Matrix4x4 MakeViewProjection(const ItemInfo& info, const ModelData& model);

	std::array<Icon, static_cast<size_t>(ItemID::MAX)> icons_;
	std::unordered_map<ItemID, Icon*> needsDrawIcons_; // 描く必要のあるアイコンのポインタ
};
