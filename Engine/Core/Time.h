#pragma once
#include "Engine/Application/Application.h"
namespace Engine {
	class Time
	{
	public:
		static int frameCount;
		static float previousTime;
		static float deltaTime;
		static float lastFrame;

		static void Update();
	};
}


