#include "ColorConverter.h"
#include <algorithm>

namespace ColorConverter
{
    Vector4 ConvertUintToVector4(uint32_t color)
    {
        float r = ((color >> 24) & 0xFF) / 255.0f;
        float g = ((color >> 16) & 0xFF) / 255.0f;
        float b = ((color >> 8) & 0xFF) / 255.0f;
        float a = (color & 0xFF) / 255.0f;
        return { r, g, b, a };
    }

    uint32_t ConvertVector4ToUint(Vector4 color)
    {
        uint32_t r = static_cast<uint32_t>(std::clamp(color.x * 255.0f, 0.0f, 255.0f));
        uint32_t g = static_cast<uint32_t>(std::clamp(color.y * 255.0f, 0.0f, 255.0f));
        uint32_t b = static_cast<uint32_t>(std::clamp(color.z * 255.0f, 0.0f, 255.0f));
        uint32_t a = static_cast<uint32_t>(std::clamp(color.w * 255.0f, 0.0f, 255.0f));
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    Vector4 ConvertARGBtoRGBA(const Vector4& argb)
    {
        return { argb.y, argb.z, argb.w, argb.x };
    }
};