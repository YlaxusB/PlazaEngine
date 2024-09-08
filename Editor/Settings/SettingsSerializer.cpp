#include "Engine/Core/PreCompiledHeaders.h"
#include "SettingsSerializer.h"
#include "EditorSettings.h"
namespace Plaza::Editor {
	void EditorSettingsSerializer::Serialize(std::string filePath) {
		if (filePath.empty()) {
			filePath = Application::Get()->enginePathAppData + "Settings.yaml";
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Vsync" << YAML::Value << Settings::vsync;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void EditorSettingsSerializer::DeSerialize(std::string filePath) {
		if (filePath.empty()) {
			filePath = Application::Get()->enginePathAppData + "Settings.yaml";
		}
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (data["Vsync"]) {
			Settings::vsync = data["Vsync"].as<bool>();
		}
		Settings::ReapplyAllSettings();
	}
}