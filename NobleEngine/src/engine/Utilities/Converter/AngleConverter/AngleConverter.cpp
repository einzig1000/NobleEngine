#include "AngleConverter.h"
#include <numbers>

namespace AngleConverter
{
	float ToRadian(float degree)
	{
		return degree * (std::numbers::pi_v<float> / 180.0f);
	}

	float ToDegree(float radian)
	{
		return radian * (180.0f / std::numbers::pi_v<float>);
	}
}
