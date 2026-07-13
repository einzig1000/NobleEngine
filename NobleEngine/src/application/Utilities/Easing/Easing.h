#pragma once
#include <cmath>
#include "definition/definition.h"

namespace Easing
{
	float EasingFloat(float start, float end, EaseType easeType, float t);
	Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float t);
	int COLOR(int startColor, int endColor, float t);
	
	float CalculateEasedValue(float start, float end, EaseType easeType, float t);

	float F_LINEAR(float start, float end, float t);
	float F_IN_SINE(float start, float end, float t);
	float F_OUT_SINE(float start, float end, float t);
	float F_IN_OUT_SINE(float start, float end, float t);
	float F_IN_QUAD(float start, float end, float t);
	float F_OUT_QUAD(float start, float end, float t);
	float F_IN_OUT_QUAD(float start, float end, float t);
	float F_IN_CUBIC(float start, float end, float t);
	float F_OUT_CUBIC(float start, float end, float t);
	float F_IN_OUT_CUBIC(float start, float end, float t);
	float F_IN_QUART(float start, float end, float t);
	float F_OUT_QUART(float start, float end, float t);
	float F_IN_OUT_QUART(float start, float end, float t);
	float F_IN_QUINT(float start, float end, float t);
	float F_OUT_QUINT(float start, float end, float t);
	float F_IN_OUT_QUINT(float start, float end, float t);
	float F_IN_EXPO(float start, float end, float t);
	float F_OUT_EXPO(float start, float end, float t);
	float F_IN_OUT_EXPO(float start, float end, float t);
	float F_IN_CIRC(float start, float end, float t);
	float F_OUT_CIRC(float start, float end, float t);
	float F_IN_OUT_CIRC(float start, float end, float t);
	float F_IN_BACK(float start, float end, float t);
	float F_OUT_BACK(float start, float end, float t);
	float F_IN_OUT_BACK(float start, float end, float t);
	float F_IN_ELASTIC(float start, float end, float t);
	float F_OUT_ELASTIC(float start, float end, float t);
	float F_IN_OUT_ELASTIC(float start, float end, float t);
	float F_IN_BOUNCE(float start, float end, float t);
	float F_OUT_BOUNCE(float start, float end, float t);

};