#ifndef TIME_H
#define TIME_H

#include <SFML/System.hpp>

//Wrapper around sf::Clock to provide the elapsed time overall, and elapsed time since the last frame update

class Time
{
	private: // data members
		static sf::Clock _clock;

		static float _currentTime;
		static float _gameDeltaTime;

		static float _currentRenderTime;
		static float _renderDeltaTime;

	private: // disable creation of time object
		Time(){}

	public: // functons
		static void update();
		static void updateRenderTime();

		static float now();
		static float gameLoopDelta();
		static float renderDelta();

};

#endif // TIME_H
