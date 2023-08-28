#pragma once
namespace Plaza {
	class Script {
	public:
		std::chrono::system_clock::time_point lastModifiedDate;
		vector<uint64_t> entitiesUsingThisScript;
	};
}