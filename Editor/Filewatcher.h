#pragma once
namespace Plaza::Editor {
	class Filewatcher {
	public:
		static void Start(std::string path);
		static void UpdateOnMainThread();
		static void AddToMainThread(const std::function<void()>& function);

		static std::vector<std::function<void()>> mMainThreadQueue;
	};
}