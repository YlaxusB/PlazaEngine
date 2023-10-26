#pragma once
#include "Engine/Vendor/Filewatcher/Filewatcher.h"
namespace Plaza::Editor {
	class Filewatcher {
	public:
		static void Start(std::string path);
		static void UpdateOnMainThread();
		static void AddToMainThread(const std::function<void()>& function);

		static std::vector<std::function<void()>> mMainThreadQueue;
		static std::map<filewatch::Event, std::string> mQueuedEvents;
	};
}