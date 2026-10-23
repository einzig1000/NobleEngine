//#pragma once
//#include <Game.h>
//#include <definition/definition.h>
//#include <GameObjects/UI/UIData.h>
//#include <externals/MagicEnum/magic_enum.hpp>
//#include <Utilities/Logger/Logger.h>
//#include <array>
//#include <string>
//#include <string_view>
//#include <vector>
//
//class EventBus;
//class ItemInventory;
//class MiningPointGauge;
//class ItemIconManager;
//
//
//class IUIElement
//{
//public:
//	IUIElement() = default;
//	virtual ~IUIElement() = default;
//	// UI要素の初期化
//	virtual void Initialize() = 0;
//	// UI要素の更新
//	virtual void Update(int32_t cameraID) = 0;
//	// UI要素の描画
//	virtual void Draw(int32_t rt_ID) = 0;
//
//	virtual void DrawImGui() = 0;
//
//	virtual void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }
//	virtual void SetInventory(const ItemInventory* inventory) { inventory_ = inventory; }
//	virtual void SetMiningGauge(const MiningPointGauge* miningGauge) { miningGauge_ = miningGauge; }
//	virtual void SetIconManager(ItemIconManager* iconManager) { iconManager_ = iconManager; }
//	virtual void SetNextUIMode(UIScreenType* nextUIMode) { nextUIMode_ = nextUIMode; }
//
//protected:
//	EventBus* eventBus_ = nullptr;
//	UIScreenType* nextUIMode_ = nullptr;
//
//	const MiningPointGauge* miningGauge_ = nullptr;
//	const ItemInventory* inventory_ = nullptr;
//	ItemIconManager* iconManager_ = nullptr;
//
//	std::vector<ElementInstanceData> sprites_;
//};


#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <GameObjects/UI/UIData.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <Utilities/Logger/Logger.h>
#include <array>
#include <string>
#include <string_view>
#include <vector>

class EventBus;
class ItemInventory;
class MiningPointGauge;
class ItemIconManager;


class IUIElement
{
public:
	IUIElement() = default;
	virtual ~IUIElement() = default;
	// UI要素の初期化
	virtual void Initialize() = 0;
	// UI要素の更新
	virtual void Update(int32_t cameraID) = 0;
	// UI要素の描画
	virtual void Draw(int32_t rt_ID) = 0;

	virtual void DrawImGui() = 0;

	virtual void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }
	virtual void SetInventory(const ItemInventory* inventory) { inventory_ = inventory; }
	virtual void SetMiningGauge(const MiningPointGauge* miningGauge) { miningGauge_ = miningGauge; }
	virtual void SetIconManager(ItemIconManager* iconManager) { iconManager_ = iconManager; }
	virtual void SetNextUIMode(UIScreenType* nextUIMode) { nextUIMode_ = nextUIMode; }

protected:

	// sprites_にJsonのデータを入れてrenderObjectを生成する
	void BuildSprites(UIElementType type);
	// Partで分類しlabelIndex順に並べたarrayを生成する
	template<typename PartEnum>
	std::array<std::vector<int32_t>, magic_enum::enum_count<PartEnum>()> FindParts() const
	{
		// returnする箱を作成
		std::array<std::vector<int32_t>, magic_enum::enum_count<PartEnum>()> result;

		// PartEnum全走査
		for (PartEnum part : magic_enum::enum_values<PartEnum>())
		{
			const std::string_view name = magic_enum::enum_name(part);

			std::vector<int32_t>& indices = result[static_cast<size_t>(part)];

			// sprites_全走査
			for (size_t i = 0; i < sprites_.size(); ++i)
			{
				// labelは必ずPartEnumのどれかと一致する
				if (sprites_[i].data.label != name) continue;

				const int32_t labelIndex = sprites_[i].data.labelIndex;
				// labelIndexが負なら
				if (labelIndex < 0)
				{
					// あってはならないタイプのエラー
					__debugbreak();
					Log("%s の labelIndex が負の値です(%d)", name.data(), labelIndex);
					continue;
				}
				// vecterのサイズが足りなくなったら
				if (labelIndex >= static_cast<int32_t>(indices.size()))
				{
					indices.resize(static_cast<size_t>(labelIndex + 1), -1);
				}
				// 既に要素が入っているなら
				if (indices[labelIndex] >= 0)
				{
					// あってはならないタイプのエラー
					__debugbreak();
					Log("%s の %d 番が重複しています", name.data(), labelIndex);
					continue;
				}

				// 順番通りにアクセス出来るようにsprites_の要素番号を入れる
				indices[labelIndex] = static_cast<int32_t>(i);
			}

			// sprites_全走査したのに空なら
			if (indices.empty())
			{
				__debugbreak();
				Log("UIConfig.json に %s がありません", name.data());
			}

			// 連番になっているか確認する
			for (int32_t k = 0; k < static_cast<int32_t>(indices.size()); ++k)
			{
				if (indices[k] < 0)
				{
					__debugbreak();
					Log("%s の %d 番がありません", name.data(), k);
				}
			}
		}

		// return
		return result;
	}
	// indicesからlabelIndexが一致するスプライトを取得。無ければ nullptr
	ElementInstanceData* GetSprite(const std::vector<int32_t>& indices, int32_t labelIndex);

	// 静的スプライトの全更新
	void UpdateStaticRenderObject(const Matrix4x4& orthographic);
	// 静的スプライトの全描画
	void DrawStaticRenderObject(int32_t rt_ID);
	// スプライトの描画
	void DrawRenderObject(int32_t rt_ID, const ElementInstanceData& sprite);

	// 中心・大きさ(基準解像度のpx)・回転から行列を作る
	void UpdateQuad(ElementInstanceData& sprite, const Vector2& center, const Vector2& size, const Vector3& rotate, const Matrix4x4& orthographic);
	// 描画したときの大きさ
	Vector2 GetDrawSize(const ElementData& data);
	// 描画したときの中心
	Vector2 GetCenter(const ElementData& data);
	// マウスの位置を基準解像度の座標に直す
	Vector2 GetMouseUIPosition();
	// マウスが四角の中にあるか
	bool IsMouseOver(const ElementData& data, const Vector2& mouseUI);

	// 数字の文字列を、center が数字の中心になるように置いたときの左上(DotGothic16用)
	Vector2 GetDigitTextPosition(const std::string& text, int32_t charSize, const Vector2& center);
	// 黒で上下左右にずらして縁取りしてから、本体を描く(DrawStringを5回呼ぶ)
	void DrawOutlinedString(int32_t rt_ID, const std::string& text, int32_t charSize, const Vector2& pos, const Vector4& color, float outlineWidth);


protected:
	EventBus* eventBus_ = nullptr;
	UIScreenType* nextUIMode_ = nullptr;

	const MiningPointGauge* miningGauge_ = nullptr;
	const ItemInventory* inventory_ = nullptr;
	ItemIconManager* iconManager_ = nullptr;

private:

	// UIConfig.json から作ったスプライト(並び順 = 描画順)
	std::vector<ElementInstanceData> sprites_;
};