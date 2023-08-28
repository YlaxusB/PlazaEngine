#pragma once
namespace Plaza {
	class ScriptManagerSerializer {
	public:
		static void Create(std::string fullPath);
		static void AddScript(std::string scriptConfigPath, std::string scriptPath, std::chrono::system_clock::time_point time);
		static void Serialize(std::string fullPath, std::map<std::string, std::chrono::system_clock::time_point> scripts);
		static std::map<std::string, std::chrono::system_clock::time_point> DeSerialize(std::string fullPath);
	};
}