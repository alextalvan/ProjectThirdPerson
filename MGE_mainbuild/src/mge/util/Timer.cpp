#include "mge/util/Timer.hpp"
#include <iostream>
#include "mge/core/Time.hpp"

Timer::Timer(float duration) : _duration(duration)
{

}

Timer::~Timer()
{

}

bool Timer::isFinished()
{
    float dur = (Time::now() - _startTime);
    return dur > _duration;
}

void Timer::Reset()
{
   _startTime = Time::now();
}

void Timer::SetDuration(float dur)
{
    _duration = dur;
}

float Timer::GetElapsedTime()
{
   return Time::now() - _startTime;
}
