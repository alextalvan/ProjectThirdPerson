#include <mge/util/Random.hpp>
#include <cstdlib>
#include <ctime>

namespace Utils
{
    std::mt19937 Random::generator(time(0));

    int Random::GetValue(int rangeStart, int rangeEnd)
    {
        std::uniform_int_distribution<int> dist(rangeStart,rangeEnd);
        return dist(generator);
    }

    float Random::GetValue(float rangeStart, float rangeEnd)
    {
        std::uniform_real_distribution<float> dist(rangeStart,rangeEnd);
        return dist(generator);
    }

    bool Random::Roll(int sides)
    {

        std::uniform_int_distribution<int> dist(0,sides-1);
        return dist(generator) == 0;
    }
}
