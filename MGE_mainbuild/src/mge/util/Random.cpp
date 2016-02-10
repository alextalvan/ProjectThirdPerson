#include <mge/util/Random.hpp>
#include <cstdlib>
#include <ctime>

namespace Utils
{
    void Random::Initialize()
    {
        srand(time(NULL));
    }

    int Random::GetValue(int rangeStart, int rangeEnd)
    {
        //assert(rangeEng>rangeStart);
        int range = rangeEnd - rangeStart + 1;

        return rand()%range + rangeStart;
    }

    bool Random::Roll(int sides)
    {
        return (rand()%sides) == 0 ;
    }
}
