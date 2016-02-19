#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace Utils
{
    class Random
    {
    public:
        static int GetValue(int rangeStart, int rangeEnd);
        static float GetFloatValue(float rangeStart, float rangeEnd);
        static bool Roll(int sides);
    private:
        static std::mt19937 generator;
    };
}

#endif // RANDOM_H
