#include "SkillTreeData.h"
#include <cstdio>

bool IsFeatureEffect(SkillEffectType type)
{
	switch (type)
	{
	case SkillEffectType::UnlockRangeMining:
		return true;
	default:
		return false;
	}
}

std::string MakeSkillEffectText(SkillEffectType type, float value)
{
	char buffer[128] = "";
	switch (type)
	{
	case SkillEffectType::None:
		return "";
	case SkillEffectType::MiningPower:
		std::snprintf(buffer, sizeof(buffer), "採掘力 +%g", value);
		break;
	case SkillEffectType::SwingSpeed:
		std::snprintf(buffer, sizeof(buffer), "振る速さ +%g%%", value * 100.0f);
		break;
	case SkillEffectType::SwingScale:
		std::snprintf(buffer, sizeof(buffer), "ハンマーの大きさ +%g", value);
		break;
	case SkillEffectType::UnlockRangeMining:
		return "範囲採掘が使えるようになる";
	}
	return buffer;
}