#pragma once
#include "Engine/Application/Application.h"

#include <chrono>
using namespace std;
using namespace std::chrono;
namespace Engine {
	class Time
	{
	public:
		static int drawCalls;
		static int addInstanceCalls;
		static int frameCount;
		static float previousTime;
		static float deltaTime;
		static float lastFrame;
		static float fps;
		static float msPerFrame;
		static void Update();
	};

	class Profiler {
	public:
		string name;
		int divider;
		Profiler(string name_t, int divider_t = 10) : name(name_t) {
			this->divider = divider_t;
			Start();
		}
		high_resolution_clock::time_point startTime;
		high_resolution_clock::time_point endTime;
		void Start() {
			if (Time::frameCount % divider)
				startTime = std::chrono::high_resolution_clock::now();
		}
		void Stop() {
			if (Time::frameCount % divider) {
				endTime = std::chrono::high_resolution_clock::now();
				Print();
			}
		}

		void Print() {
			if (Time::frameCount % divider) {
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
				// Print the execution time
				std::cout << name << ": " << duration.count() << " ms" << std::endl;
			}
		}
	};
}


inline int Engine::Time::drawCalls = 0;
inline int Engine::Time::addInstanceCalls = 0;
inline int Engine::Time::frameCount = 0;
inline float Engine::Time::previousTime = 0;
inline float Engine::Time::deltaTime = 0;
inline float Engine::Time::lastFrame = 0;
inline float Engine::Time::fps = 0;
inline float Engine::Time::msPerFrame = 0;