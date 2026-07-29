#pragma once
#include <Game.h>
#include <memory>

class HaveItem
{
public:
	HaveItem();
	~HaveItem();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTextureID);

	void SetItem(ItemID itemID) { currentItemID_ = itemID; }
	void SetParentWorldMatrix(const Matrix4x4& world) { parentWorldMatrix_ = world; }

	const std::vector<AABB>& GetAABB() const { return itemAABB_; }
	const std::vector<OBB>& GetOBB() const { return itemOBB_; }

private:
	std::unique_ptr<RenderObject> render_;
	std::vector<AABB> itemAABB_;
	std::vector<OBB> itemOBB_;
	ItemID currentItemID_ = ItemID::MAX;

	Matrix4x4 parentWorldMatrix_;
	EulerTransforms pivotTransform_;
	EulerTransforms itemTransform_;

	float startTime_;
};

