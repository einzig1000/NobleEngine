#pragma once
#include <EngineDefinition/EngineDefinition.h>

namespace ColorConverter
{
    // ARGBをRGBA
    Vector4 ConvertARGBtoRGBA(const Vector4& argb);
    // RGBAをARGB
    Vector4 ConvertUintToVector4(uint32_t color);
    uint32_t ConvertVector4ToUint(Vector4 color);
};

