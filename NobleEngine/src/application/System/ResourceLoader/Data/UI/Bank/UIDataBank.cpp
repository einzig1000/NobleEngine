#include "UIDataBank.h"

void UIDataBank::SetData(UIElementType type, const std::vector<ElementData>& data)
{
	dataBank_[type] = data;
}

const std::vector<ElementData>* UIDataBank::GetData(UIElementType type) const
{
	auto it = dataBank_.find(type);
	if (it != dataBank_.end())
	{
		return &(it->second);
	}
	else
	{
		return nullptr;
	}
}
