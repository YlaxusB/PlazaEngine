#pragma once
namespace Plaza {
	class ScriptManagerSerializer {
	public:
		static void Create(std::string fullPath);
		static void AddScript(std::string scriptConfigPath, std::string scriptPath, Script script);
		static void Serialize(std::string fullPath, std::map<std::string, Script> scripts);
		static std::map<std::string, Script> DeSerialize(std::string fullPath);
	};
}