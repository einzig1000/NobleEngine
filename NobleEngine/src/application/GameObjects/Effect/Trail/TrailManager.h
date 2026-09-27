#pragma once
#include <GameObjects/Effect/Trail/ITrail.h>
#include <vector>

class TrailManager
{
public:
	TrailManager();

	void AddTrail(ITrail* trail);

	void Update(int32_t c_ID);
	void Draw(int32_t rt_ID);

private:
	std::vector<ITrail*> trails_;
};

