#pragma once
#include <Game.h>
#include <memory>
#include <definition/definition.h>

class HaveItem
{
public:
	HaveItem();
	~HaveItem();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTextureID);

	void SetItem(ItemID itemID);

	void SetParentWorldMatrix(const Matrix4x4& world) { parentWorldMatrix_ = world; }

	const ColliderShape& GetWorldCollider() const { return worldCollider_; }

private:
	// 描画オブジェクト
	std::unique_ptr<RenderObject> render_;
	Matrix4x4 wvpMatrix_;
	Matrix4x4 worldMatrix_;
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	int32_t t_haveItem_ = -1;
	float animationTime_ = 0.0f;

	const ItemInfo* itemInfo_ = nullptr;
	const ModelData* modelData_ = nullptr;

	// アニメーションID
	int32_t a_haveItem_ = -1;

	// 現在持っているアイテムのID
	ItemID currentItemID_ = ItemID::MAX;
	// 持っているアイテムのワールド行列
	ColliderShape worldCollider_;

	// 親(プレイヤー)行列
	Matrix4x4 parentWorldMatrix_;
	// アイテムのピボットの変換行列
	EulerTransforms pivotTransform_;
};

