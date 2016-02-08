#ifndef FPS_H
#define FPS_H

/**
 * Frames per second utility class
 */
class FPS
{
	private:
		FPS(){}

	public:
		static void update();
		static unsigned int getFPS();

	private:
		static float _startTime;
		static unsigned int _count;
		static unsigned int _fps;
};

#endif // FPS_H
