#include "Easing.h"
#include <cmath>
#include <numbers>

namespace Easing
{
	float EasingFloat(float start, float end, EaseType easeType, float t)
	{
		if (t >= 1.0f)return end;
		else if (t <= 0.0f)return start;

		return CalculateEasedValue(start, end, easeType, t);
	}

	Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float t)
	{
		if (t >= 1.0f)return end;
		else if (t <= 0.0f)return start;

		Vector3 Return = start;
		if (start.x != end.x)Return.x = CalculateEasedValue(start.x, end.x, easeType, t);
		if (start.y != end.y)Return.y = CalculateEasedValue(start.y, end.y, easeType, t);
		if (start.z != end.z)Return.z = CalculateEasedValue(start.z, end.z, easeType, t);

		return Return;
	}


	float CalculateEasedValue(float start, float end, EaseType easeType, float t)
	{
		switch (easeType)
		{
		case EaseType::LINEAR:
			return F_LINEAR(start, end, t);
		case EaseType::IN_SINE:
			return F_IN_SINE(start, end, t);
		case EaseType::OUT_SINE:
			return F_OUT_SINE(start, end, t);
		case EaseType::IN_OUT_SINE:
			return F_IN_OUT_SINE(start, end, t);
		case EaseType::IN_QUAD:
			return F_IN_QUAD(start, end, t);
		case EaseType::OUT_QUAD:
			return F_OUT_QUAD(start, end, t);
		case EaseType::IN_OUT_QUAD:
			return F_IN_OUT_QUAD(start, end, t);
		case EaseType::IN_CUBIC:
			return F_IN_CUBIC(start, end, t);
		case EaseType::OUT_CUBIC:
			return F_OUT_CUBIC(start, end, t);
		case EaseType::IN_OUT_CUBIC:
			return F_IN_OUT_CUBIC(start, end, t);
		case EaseType::IN_QUART:
			return F_IN_QUART(start, end, t);
		case EaseType::OUT_QUART:
			return F_OUT_QUART(start, end, t);
		case EaseType::IN_OUT_QUART:
			return F_IN_OUT_QUART(start, end, t);
		case EaseType::IN_QUINT:
			return F_IN_QUINT(start, end, t);
		case EaseType::OUT_QUINT:
			return F_OUT_QUINT(start, end, t);
		case EaseType::IN_OUT_QUINT:
			return F_IN_OUT_QUINT(start, end, t);
		case EaseType::IN_EXPO:
			return F_IN_EXPO(start, end, t);
		case EaseType::OUT_EXPO:
			return F_OUT_EXPO(start, end, t);
		case EaseType::IN_OUT_EXPO:
			return F_IN_OUT_EXPO(start, end, t);
		case EaseType::IN_CIRC:
			return F_IN_CIRC(start, end, t);
		case EaseType::OUT_CIRC:
			return F_OUT_CIRC(start, end, t);
		case EaseType::IN_OUT_CIRC:
			return F_IN_OUT_CIRC(start, end, t);
		case EaseType::IN_BACK:
			return F_IN_BACK(start, end, t);
		case EaseType::OUT_BACK:
			return F_OUT_BACK(start, end, t);
		case EaseType::IN_OUT_BACK:
			return F_IN_OUT_BACK(start, end, t);
		case EaseType::IN_ELASTIC:
			return F_IN_ELASTIC(start, end, t);
		case EaseType::OUT_ELASTIC:
			return F_OUT_ELASTIC(start, end, t);
		case EaseType::IN_OUT_ELASTIC:
			return F_IN_OUT_ELASTIC(start, end, t);
		case EaseType::IN_BOUNCE:
			return F_IN_BOUNCE(start, end, t);
		case EaseType::OUT_BOUNCE:
			return F_OUT_BOUNCE(start, end, t);
		default:
			return F_LINEAR(start, end, t);
		}
	}



	float F_LINEAR(float start, float end, float t)
	{
		return (1.0f - t) * start + t * end;
	}
	float F_IN_SINE(float start, float end, float t)
	{
		float easedT = 1.0f - cosf((t * float(std::numbers::pi_v<float>)) / 2.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_SINE(float start, float end, float t)
	{
		float easedT = sinf((t * float(std::numbers::pi_v<float>)) / 2.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_SINE(float start, float end, float t)
	{
		float easedT = -(cosf(float(std::numbers::pi_v<float>) * t) - 1.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_QUAD(float start, float end, float t)
	{
		float easedT = t * t;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_QUAD(float start, float end, float t)
	{
		float easedT = 1.0f - (1.0f - t) * (1.0f - t);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_QUAD(float start, float end, float t)
	{
		float easedT = t < 0.5f ? 2.0f * t * t
			: 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_CUBIC(float start, float end, float t)
	{
		float easedT = t * t * t;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_CUBIC(float start, float end, float t)
	{
		float easedT = 1.0f - powf(1.0f - t, 3.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_CUBIC(float start, float end, float t)
	{
		float easedT = t < 0.5f ? 4.0f * t * t * t
			: 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_QUART(float start, float end, float t)
	{
		float easedT = t * t * t * t;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_QUART(float start, float end, float t)
	{
		float easedT = 1.0f - powf(1.0f - t, 4.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_QUART(float start, float end, float t)
	{
		float easedT = t < 0.5f ? 8.0f * t * t * t * t
			: 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_QUINT(float start, float end, float t)
	{
		float easedT = t * t * t * t * t;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_QUINT(float start, float end, float t)
	{
		float easedT = 1.0f - powf(1.0f - t, 5.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_QUINT(float start, float end, float t)
	{
		float easedT = t < 0.5f ? 16.0f * t * t * t * t * t
			: 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_EXPO(float start, float end, float t)
	{
		float easedT = t == 0.0f ? 0.0f
			: powf(2.0f, 10.0f * t - 10.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_EXPO(float start, float end, float t)
	{
		float easedT = t == 1.0f ? 1.0f
			: 1.0f - powf(2.0f, -10.0f * t);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_EXPO(float start, float end, float t)
	{
		float easedT = t == 0.0f ? 0.0f
			: t == 1.0f ? 1.0f
			: t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f
			: (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_CIRC(float start, float end, float t)
	{
		float easedT = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_CIRC(float start, float end, float t)
	{
		float easedT = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_CIRC(float start, float end, float t)
	{
		float easedT = t < 0.5f
			? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f
			: (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_BACK(float start, float end, float t)
	{
		static const float c1 = 1.70158f;
		static const float c3 = c1 + 1.0f;
		float easedT = c3 * t * t * t - c1 * t * t;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_BACK(float start, float end, float t)
	{
		static const float c1 = 1.70158f;
		static const float c3 = c1 + 1.0f;
		float easedT = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_BACK(float start, float end, float t)
	{
		static const float c1 = 1.70158f;
		static const float c2 = c1 * 1.525f;
		float easedT = t < 0.5f
			? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
			: (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_ELASTIC(float start, float end, float t)
	{
		static const float c4 = (2.0f * float(std::numbers::pi_v<float>)) / 3.0f;
		float easedT = t == 0.0f ? 0.0f
			: t == 1.0f ? 1.0f
			: -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_OUT_ELASTIC(float start, float end, float t)
	{
		static const float c4 = (2.0f * float(std::numbers::pi_v<float>)) / 3.0f;
		float easedT = t == 0.0f ? 0.0f
			: t == 1.0f ? 1.0f
			: powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_OUT_ELASTIC(float start, float end, float t)
	{
		static const float c5 = (2.0f * float(std::numbers::pi_v<float>)) / 4.5f;
		float easedT = t == 0.0f ? 0.0f
			: t == 1.0f ? 1.0f
			: t < 0.5f
			? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f
			: (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
		return (1.0f - easedT) * start + easedT * end;
	}
	float F_IN_BOUNCE(float start, float end, float t)
	{
		static const float n1 = 7.5625f;
		static const float d1 = 2.75f;

		float easedT = 0.0f;
		if (t < 1.0f / d1)easedT = n1 * t * t;
		else if (t < 2 / d1)easedT = n1 * (t -= 1.5f / d1) * t + 0.75f;
		else if (t < 2.5 / d1)easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		else easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;

		return (1.0f - easedT) * end + easedT * start;
	}
	float F_OUT_BOUNCE(float start, float end, float t)
	{
		static const float n1 = 7.5625f;
		static const float d1 = 2.75f;

		float easedT = 0.0f;
		if (t < 1.0f / d1)easedT = n1 * t * t;
		else if (t < 2 / d1)easedT = n1 * (t -= 1.5f / d1) * t + 0.75f;
		else if (t < 2.5 / d1)easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		else easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;
		return (1.0f - easedT) * start + easedT * end;
	}

	int COLOR(int startColor, int endColor, float t)
	{
		return int((1.0f - t) * startColor + t * endColor);
	}
}