#include "IUIScreen.h"
#include <GameObjects/UI/UIElement/IUIElement.h>

void IUIScreen::SetEventBus(EventBus* eventBus)
{
	eventBus_ = eventBus;
	for (auto& element : uiElements_)
	{
		element->SetEventBus(eventBus);
	}
}
