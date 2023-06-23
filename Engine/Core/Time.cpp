#include "Time.h"
#include <GLFW/glfw3.h>

void Engine::Time::Update() {
	// Delta time
	int& frameCount = Time::frameCount;
	float& previousTime = Time::previousTime;
	float& deltaTime = Time::deltaTime;
	float& lastFrame = Time::lastFrame;
	// Measure speed
	double currentTime = glfwGetTime();
	frameCount++;
	// If a second has passed.
	if (currentTime - previousTime >= 1.0)
	{
		// Display the frame count here any way you want.
		//std::cout << frameCount << std::endl;

		frameCount = 0;
		previousTime = currentTime;
	}


	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
}