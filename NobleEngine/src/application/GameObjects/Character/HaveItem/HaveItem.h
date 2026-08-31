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

	void SetItem(ItemID itemID) { currentItemID_ = itemID; }

	void SetParentWorldMatrix(const Matrix4x4& world) { parentWorldMatrix_ = world; }

	const ColliderShape& GetWorldCollider() const { return worldCollider_; }

private:
	// 描画オブジェクト
	std::unique_ptr<RenderObject> render_;
	// 現在持っているアイテムのID
	ItemID currentItemID_ = ItemID::MAX;
	// 持っているアイテムのワールド行列
	ColliderShape worldCollider_;

	// 親(プレイヤー)行列
	Matrix4x4 parentWorldMatrix_;
	// モデルそのものの変換行列
	EulerTransforms modelTransform_;
	// アイテムの変換行列
	EulerTransforms itemTransform_;
	// アイテムのピボットの変換行列
	EulerTransforms pivotTransform_;

	EulerTransforms preTransform_;

	int32_t stage_ = 0;
	float startTime_ = 999999.0f;



	Vector3 rotate = { 0.0f, 0.0f, 0.0f };
};

