#pragma once
#include <GameObjects/UI/UIData.h>
#include <Game.h>
#include <string>
#include <string_view>
#include <vector>

class UIEditor
{
public:
	UIEditor();
	~UIEditor();
	void Update();
	void Draw();
	void DrawImGui();

private:
	// 編集するUIElementを切り替える(保存していない変更は捨てる)
	void SelectElementType(UIElementType type);
	// 編集中のデータを、データ置き場(最後に読み込み・保存した内容)に戻す
	void Revert();

	// ImGuiの各部分
	// UIElementの選択・保存・元に戻す
	void DrawTypeSelector();
	// コードが使うパーツがそろっているか
	void DrawPartCheck();
	// 要素の一覧と、追加・複製・削除・並べ替え
	void DrawElementList();
	// 選んでいる要素の値
	void DrawSelectedElement();
	// 選んでいる要素を見本にして、グリッドで並べて追加する
	void DrawGridTool();
	// プレビュー(クリックで選ぶ・ドラッグで動かす)
	void DrawPreview();

	// 選んでいる要素の番号が正しいか
	bool HasSelection() const;
	// テクスチャを設定する。ファイルから読んだテクスチャでなければ false(レンダーテクスチャなどは保存できない)
	static bool SetTexture(ElementData& data, int32_t textureID);
	// 指定の位置(基準解像度のpx)にある一番手前の要素。無ければ -1
	int32_t PickElement(const Vector2& canvasPos) const;
	// 選んでいる要素を動かす(%)。moveSameLabel_ なら同じlabelもまとめて動かす
	void MoveSelected(const Vector2& deltaRate);

	// コードが使うパーツ名の一覧(各UIElementの enum class Part の名前)
	static std::vector<std::string_view> GetPartNames(UIElementType type);

	int32_t rt_ID = -1;
	int32_t c_ID = -1;

	// 編集中のUIElementとデータ
	UIElementType elementType_ = UIElementType::MAX;
	std::vector<ElementData> elementData_;
	int32_t selectedElementIndex_ = -1;
	// 保存していない変更があるか
	bool isDirty_ = false;

	// プレビュー用のスプライト(elementData_ と同じ並び)
	std::vector<ElementInstanceData> previewSprites_;

	// プレビューでドラッグしたときに、同じlabelの要素もまとめて動かすか
	bool moveSameLabel_ = false;
	// テクスチャを設定できなかったときの知らせ
	std::string textureMessage_;

	// グリッドで並べる設定(個数・間隔は基準解像度のpx)
	Vector2int gridCount_ = { 9, 1 };
	Vector2 gridPitch_ = { 72.0f, 72.0f };
};