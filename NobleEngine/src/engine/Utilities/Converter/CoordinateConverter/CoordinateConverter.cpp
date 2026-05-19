#include "CoordinateConverter.h"
#include <cmath>
#include <algorithm>

namespace CoordinateConverter
{
    Coordinate_cylindrical ToCylindrical(const Vector3& cartesian)
    {
        Coordinate_cylindrical out{};
        out.radius = std::sqrt(cartesian.x * cartesian.x + cartesian.z * cartesian.z);
        out.theta = std::atan2(cartesian.z, cartesian.x); // [-pi, +pi]
        out.height = cartesian.y;
        return out;
    }
    Coordinate_cylindrical ToCylindrical(const Coordinate_spherical& spherical)
    {
        // 球面 -> 直交 -> 円柱
        const Vector3 cart = ToCartesian(spherical);
        return ToCylindrical(cart);
    }
    Vector3 ToCartesian(const Coordinate_cylindrical& cylindrical)
    {
        Vector3 out{};
        out.x = cylindrical.radius * std::cos(cylindrical.theta);
        out.z = cylindrical.radius * std::sin(cylindrical.theta);
        out.y = cylindrical.height;
        return out;
    }
    Vector3 ToCartesian(const Coordinate_spherical& spherical)
    {
        Vector3 out{};

        if (spherical.radius <= 1e-8f)
        {
            return out;
        }

        const float cosPhi = std::cos(spherical.phi);

        out.x = spherical.radius * cosPhi * std::cos(spherical.theta);
        out.z = spherical.radius * cosPhi * std::sin(spherical.theta);
        out.y = spherical.radius * std::sin(spherical.phi);

        return out;
    }
    Coordinate_spherical ToSpherical(const Vector3& cartesian)
    {
        Coordinate_spherical out{};
        out.radius = std::sqrt(
            cartesian.x * cartesian.x +
            cartesian.y * cartesian.y +
            cartesian.z * cartesian.z);

        // 原点は角度が定義できないので 0 扱い
        if (out.radius <= 1e-8f)
        {
            out.theta = 0.0f;
            out.phi = 0.0f;
            return out;
        }

        // 方位角（XZ平面）
        out.theta = std::atan2(cartesian.z, cartesian.x);

        // 仰角（XZ平面から上。[-pi/2, +pi/2]）
        float t = cartesian.y / out.radius;
        t = std::clamp(t, -1.0f, 1.0f); // 浮動小数誤差対策
        out.phi = std::asin(t);

        return out;
    }
    Coordinate_spherical ToSpherical(const Coordinate_cylindrical& cylindrical)
    {
        // 円柱 -> 直交 -> 球面
        const Vector3 cart = ToCartesian(cylindrical);
        return ToSpherical(cart);
    }
};