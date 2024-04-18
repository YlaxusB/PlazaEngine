#pragma once
#include <mutex>
#include <queue>

namespace Plaza {
	class Thread {
	public:
		Thread() {};
		void AddToQueue(std::function<void()> function) {
			this->mQueue.push(function);
		}
		void Update() {
			std::function<void()> task;
			std::lock_guard<std::mutex> lock(mQueueMutex);

			if (!mQueue.empty()) {
				task = mQueue.front();
				mQueue.pop();
			}

			if (task) {
				task();
			}
		}
	private:
		std::mutex mQueueMutex = std::mutex();
		std::queue<std::function<void()>> mQueue = std::queue<std::function<void()>>();
	};
}