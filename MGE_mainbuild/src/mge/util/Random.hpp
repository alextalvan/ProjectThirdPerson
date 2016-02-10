#ifndef RANDOM_H
#define RANDOM_H

namespace Utils
{
    class Random
    {
    public:
        static void Initialize();
        static int GetValue(int rangeStart, int rangeEnd);
        static bool Roll(int sides);
    };
}

#endif // RANDOM_H
