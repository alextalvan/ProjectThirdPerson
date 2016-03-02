#include "mge/core/Time.hpp"

sf::Clock Time::_clock;
float Time::_currentTime = 0.0f;
float Time::_gameDeltaTime = 0.0f;
float Time::_gameLoopLastTime = 0.0f;

void Time::gameLoopUpdate()
{
	float newTime = _clock.getElapsedTime().asSeconds();
	_gameDeltaTime = newTime - _gameLoopLastTime;
	_currentTime = newTime;
	_gameLoopLastTime = newTime;
}

void Time::update()
{
	float newTime = _clock.getElapsedTime().asSeconds();
	_currentTime = newTime;
}

float Time::now()
{
	return _currentTime;
}

float Time::gameLoopDelta()
{
	return _gameDeltaTime;
}
