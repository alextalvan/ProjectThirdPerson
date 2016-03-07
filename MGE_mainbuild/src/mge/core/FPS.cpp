#include <iostream>

#include "mge/core/FPS.hpp"
#include "mge/core/Time.hpp"

float FPS::_startTime = 0.0f;
unsigned int FPS::_count = 0;
unsigned int FPS::_fps = 0;

//basic fps meter

void FPS::update()
{
    Time::update();
	float time = Time::now();
	FPS::_count++;
	if ( time - _startTime >= 1.0f ) {
		_fps = _count / (time - _startTime);
		_count = 0;
		_startTime = time;
	}
}

unsigned int FPS::getFPS()
{
	return _fps;
}
