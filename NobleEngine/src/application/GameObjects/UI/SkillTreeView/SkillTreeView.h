#pragma once
#include <Game.h>
#include <definition/SkillTreeData/SkillTreeData.h>
#include <memory>
#include <vector>

/// <summary>
/// スキルツリーのノードと線を、専用のレンダーテクスチャに描く
/// ズーム・移動は正射影の範囲を変えるだけ(はみ出た分はレンダーテクスチャの外なので勝手に切れる)
/// ゲームのUI(SkillTree)とエディタ(SkillTreeEditor)の両方で使う
/// </summary>
class SkillTreeView
{
public:
	// ノードの見え方
	enum class NodeLook
	{
		Unlocked,	// 開放済み(明るい)
		Affordable,	// 1つ先で、今すぐ開放できる(アイコンは暗い・枠が光る)
		Available,	// 1つ先で、コストが足りない(アイコンは暗い)
		Silhouette,	// さらに先(アイコンは黒く塗りつぶし)
	};

	// ノードの大きさ(ツリー上のpx)
	static constexpr float kNodeSize = 72.0f;
	static constexpr float kIconSize = 56.0f;
	// 線の太さ(ツリー上のpx)
	static constexpr float kLineWidth = 8.0f;
	// ズームの範囲
	static constexpr float kMinZoom = 0.25f;
	static constexpr float kMaxZoom = 2.0f;

public:
	SkillTreeView();
	~SkillTreeView();

	// 描き込むレンダーテクスチャの大きさ(px)。変わったときだけ作り直す
	void SetViewSize(uint32_t width, uint32_t height);

	// 毎フレーム。looks は nodes と同じ並び(足りない分は Unlocked)。hoverIndex のノードは少し大きく描く
	void Update(const std::vector<SkillNodeData>& nodes, const std::vector<NodeLook>& looks, int32_t hoverIndex);
	// レンダーテクスチャに描く
	void Draw();

	// ビュー上の座標(ビューの左上が0,0。px) ⇔ ツリー上の座標
	Vector2 ViewToTree(const Vector2& viewPos) const;
	Vector2 TreeToView(const Vector2& treePos) const;
	// viewPos の下の点を動かさずにズーム(rate倍)
	void ZoomAt(const Vector2& viewPos, float rate);
	// ビュー上で viewDelta だけドラッグした分、動かす
	void Pan(const Vector2& viewDelta);
	// 中心がノードのある範囲(+margin)から出ないようにする
	void ClampCenter(const std::vector<SkillNodeData>& nodes, float margin);
	// 全ノードの真ん中を中心にして、ズームを1に戻す
	void ResetView(const std::vector<SkillNodeData>& nodes);
	// treePos にあるノードの添字(nodes の並び)。無ければ -1
	int32_t PickNode(const std::vector<SkillNodeData>& nodes, const Vector2& treePos) const;

	int32_t GetRenderTextureID() const { return rt_ID_; }
	const Vector2& GetViewSize() const { return viewSize_; }
	float GetZoom() const { return zoom_; }
	const Vector2& GetCenter() const { return center_; }

	// 板ポリ1枚を描くRenderObject(深度なし)。ツールチップの背景などでも使う
	static std::unique_ptr<RenderObject> CreateSpriteRender();

private:
	// 板1枚分
	struct Quad
	{
		std::unique_ptr<RenderObject> render;
		Matrix4x4 world;
		Matrix4x4 wvp;
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		int32_t textureID = -1;
	};

	// pool の数を count 以上にする
	static void Reserve(std::vector<Quad>& pool, size_t count);
	// 中心・大きさ・回転(Z)から行列を作る
	void SetQuad(Quad& quad, const Vector2& center, const Vector2& size, float rotateZ, const Vector4& color, int32_t textureID) const;
	void DrawQuad(const Quad& quad) const;

	// このビューの番号(レンダーテクスチャの名前が他のビューとかぶらないように)
	int32_t instanceNumber_ = 0;

	int32_t rt_ID_ = -1;
	Vector2 viewSize_;
	// ビューの中心に来るツリー上の座標
	Vector2 center_;
	float zoom_ = 1.0f;
	Matrix4x4 projection_;

	int32_t frameTextureID_ = -1;
	int32_t lineTextureID_ = -1;

	// 描く板(線 → 枠 → アイコンの順に描く)
	// 使う数はノードの数で変わるので、多めに持っておいて先頭から使う
	std::vector<Quad> lines_;
	std::vector<Quad> frames_;
	std::vector<Quad> icons_;
	size_t lineCount_ = 0;
	size_t frameCount_ = 0;
	size_t iconCount_ = 0;
};