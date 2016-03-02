#pragma once
#include <SFML/System.hpp>

class Timer
{
public:
    Timer(float duration = 10.0f);
    ~Timer();

    bool isFinished();
    void Reset();
    void SetDuration(float dur);
    float GetElapsedTime();

private:
    float _startTime = 0.0f;
    float _duration;
};
