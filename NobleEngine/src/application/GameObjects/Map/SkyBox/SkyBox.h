#pragma once
#include <Game.h>

class EventBus;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();
	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);

	void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }

private:
	EventBus* eventBus_ = nullptr;

	// レンダラー
	std::unique_ptr<RenderObject> render_;

	int32_t textureID_ = -1;
};

