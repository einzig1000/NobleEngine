#pragma once
#include <Game.h>

class SkyBox
{
public:
	SkyBox();
	~SkyBox();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);

private:
	// レンダラー
	std::unique_ptr<RenderObject> render_;

	int32_t textureID_ = -1;
};

