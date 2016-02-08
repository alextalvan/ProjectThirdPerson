#include "mge/core/Time.hpp"

sf::Clock Time::_clock;
float Time::_currentTime = 0.0f;
float Time::_gameDeltaTime = 0.0f;

float Time::_currentRenderTime = 0.0f;
float Time::_renderDeltaTime = 0.0f;

void Time::update()
{
	float newTime = _clock.getElapsedTime().asSeconds();
	_gameDeltaTime = newTime - _currentTime;
	_currentTime = newTime;
}

void Time::updateRenderTime()
{
    float newTime = _clock.getElapsedTime().asSeconds();
    _renderDeltaTime = newTime - _currentRenderTime;
	_currentRenderTime = newTime;
}

float Time::now()
{
	return _currentTime;
}

float Time::gameLoopDelta()
{
	return _gameDeltaTime;
}

float Time::renderDelta()
{
	return _renderDeltaTime;
}
