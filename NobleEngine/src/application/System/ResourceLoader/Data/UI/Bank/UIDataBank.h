#pragma once
#include <GameObjects/UI/UIData.h>
#include <unordered_map>
#include <vector>


class UIDataBank
{
public:
	void SetData(UIElementType type, const std::vector<ElementData>& data);

	const std::vector<ElementData>* GetData(UIElementType type) const;

private:

	std::unordered_map<UIElementType, std::vector<ElementData>> dataBank_;

};

