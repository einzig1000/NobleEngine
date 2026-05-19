#include "UIManager/ScreenMode/UIElement/UIElement.h"

void UIElement::Draw()
{
	for (const auto& sprite : sprites_)
	{
		sprite->Draw();
	}
}