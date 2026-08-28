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
	std::unique_ptr<RenderObject> render_;
	ItemID currentItemID_ = ItemID::MAX;
	ColliderShape worldCollider_;

	Matrix4x4 parentWorldMatrix_;
	EulerTransforms pivotTransform_;
	EulerTransforms itemTransform_;

	float startTime_ = 0.0f;
};

