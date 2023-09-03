#pragma once
#include <unordered_set>
namespace Plaza {
	class Script {
	public:
		Script() {
			this->lastModifiedDate = std::chrono::system_clock::now();
		}
		std::chrono::system_clock::time_point lastModifiedDate;
		std::unordered_set<uint64_t> entitiesUsingThisScript;
	};
}