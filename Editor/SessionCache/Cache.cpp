#include "Engine/Core/PreCompiledHeaders.h"
#include "Cache.h"
#include "Engine/Core/Standards.h"
namespace Plaza::Editor {
	void Cache::Load() {
		////   DeSerialize(Application->enginePathAppData + "\\" + "cache.yaml");
	}

	void Cache::Serialize(const std::string filePath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "LastOpenProject" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Path" << YAML::Value << Application->projectPath + "\\" + Application->activeProject->name + Standards::projectExtName;
		out << YAML::Key << "Name" << YAML::Value << Application->activeProject->name;
		out << YAML::EndMap;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void Cache::DeSerialize(const std::string filePath) {
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data) {
			std::cout << "File is empty!" << std::endl;
			return;
		}

		auto lastOpenProjectEntry = data["LastOpenProject"];
		std::string path = lastOpenProjectEntry["Path"].as<std::string>();
		std::string name = lastOpenProjectEntry["Name"].as<std::string>();

		Project::Load(path);
	}
}