#pragma once
#include <cstdint>

class IParticle
{
public:
	virtual ~IParticle() = default;

	virtual void Update(int32_t c_ID) = 0;
	virtual void Draw(int32_t rt_ID) = 0;

};

