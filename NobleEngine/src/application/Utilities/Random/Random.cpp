#include "Random.h"
#include <random>

namespace Random
{
    int RandomInt(int min, int max)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }

    float RandomFloat(float min, float max, int decimalPlaces)
    {
        int scale = static_cast<int>(std::pow(10, decimalPlaces));
        int intMin = static_cast<int>(std::round(min * scale));
        int intMax = static_cast<int>(std::round(max * scale));
        int randomInt = RandomInt(intMin, intMax);
        return static_cast<float>(randomInt) / scale;
    }
};