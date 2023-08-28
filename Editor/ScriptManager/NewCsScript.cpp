#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
namespace Plaza::Editor {
	void ScriptManager::NewCsScript(std::string fullPath) {
		std::ofstream file = Utils::Filesystem::CreateNewFile(fullPath);
		if (file.is_open()) {
			std::string fileName = std::filesystem::path{ fullPath }.stem().string();
			file << "// Your C# code goes here" << std::endl;
			file << "using System;" << std::endl;
			file << "public class " << fileName << std::endl;
			file << "{" << std::endl;
			file << "	public void OnStart()" << std::endl;
			file << "	{" << std::endl;
			file << "	" << std::endl;
			file << "	}" << std::endl;
			file << "	" << std::endl;
			file << "	public void OnRestart()" << std::endl;
			file << "	{" << std::endl;
			file << "	" << std::endl;
			file << "	}" << std::endl;
			file << "	" << std::endl;
			file << "	public void OnUpdate()" << std::endl;
			file << "	{" << std::endl;
			file << "	" << std::endl;
			file << "	}" << std::endl;
			file << "}" << std::endl;
			file.close();
		}

		Application->activeProject->scripts.emplace(fullPath, std::chrono::system_clock::now());
		std::map<std::string, std::chrono::system_clock::time_point> map = Application->activeProject->scripts;
		ScriptManagerSerializer::Serialize(Application->activeProject->directory + "\\Scripts" + Standards::scriptConfigExtName, map);
	}
}