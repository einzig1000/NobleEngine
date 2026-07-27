#include "Random.h"
#include <random>

namespace Random
{
    int32_t RandomInt(int32_t min, int32_t max)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int32_t> dist(min, max);
        return dist(rng);
    }

    float RandomFloat(float min, float max, int32_t decimalPlaces)
    {
        int32_t scale = static_cast<int32_t>(std::pow(10, decimalPlaces));
        int32_t intMin = static_cast<int32_t>(std::round(min * scale));
        int32_t intMax = static_cast<int32_t>(std::round(max * scale));
        int32_t randomInt = RandomInt(intMin, intMax);
        return static_cast<float>(randomInt) / scale;
    }
};