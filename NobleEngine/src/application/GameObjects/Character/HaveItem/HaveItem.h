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

	void SetCharacterPosition(Vector3& position) { charactorPosition_ = &position; }
	void SetItem(ItemID itemID) { currentItemID = itemID; }

private:
	std::unique_ptr<RenderObject> rightHandItemRenderObject_;
	std::vector<AABB> itemAABB_;
	ItemID currentItemID = ItemID::None;

	Vector3* charactorPosition_;
};

