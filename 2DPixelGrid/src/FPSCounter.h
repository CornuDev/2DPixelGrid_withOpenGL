/*
	Usage guide:
	1. Include the "FPSCounter.h"
	2. Create an instance of the `FPSCounter` class and pass the coordinates of the text position as parameters to its constructor.
	3. Within the main rendering loop, call the `displayAndUpdateFPS` function after rendering each frame to display and update the FPS. This function will draw the FPS text at the specified position and update the FPS value once per second.
*/
#pragma once

#include <GL/freeglut.h>  
#include <iostream>
#include <string>
#include <ctime>

class FPSCounter
{
private:
	clock_t lastTime;
	int frameCount;
	double lastFps;

public:
	FPSCounter()
		: lastTime(clock()), frameCount(0), lastFps(0) {}
	// init the clock

	~FPSCounter() {}

	void displayAndUpdateFPS(float posX, float posY)
	{
		double fps = calculateFPS();

		if (fps >= 0.0)
		{
			lastFps = fps;
		}

		drawFPS(lastFps, posX, posY);

		frameCount++; //Update Counter
		glutPostRedisplay();
	}

private:
	double calculateFPS()
	{
		clock_t currentTime = clock();
		double elapsedTime = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;

		if (elapsedTime > 1.0)
		{
			double fps = frameCount / elapsedTime;
			frameCount = 0;
			lastTime = currentTime;
			return fps;
		}
		return -1.0; // Return -1 if FPS is not calculated this time
	}

	void drawFPS(double fps, float posX, float posY)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(1.0f, 1.0f, 1.0f);
		glRasterPos2f(posX, posY);

		std::string fps_str = "FPS: " + std::to_string(fps);
		for (int i = 0; i < fps_str.length(); ++i) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, fps_str[i]);
		}
	}
};

