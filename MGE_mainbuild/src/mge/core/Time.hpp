#ifndef TIME_H
#define TIME_H

#include <SFML/System.hpp>

//Wrapper around sf::Clock to provide the elapsed time overall, and elapsed time since the last frame update

class Time
{
	private: // data members
		static sf::Clock _clock;

		static float _currentTime;

		static float _gameLoopLastTime;
		static float _gameDeltaTime;

	private: // disable creation of time object
		Time(){}

	public: // functons
		static void update();
		static void gameLoopUpdate();
		static float now();
		static float gameLoopDelta();

};

#endif // TIME_H
