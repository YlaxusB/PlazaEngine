#include "Engine/Core/PreCompiledHeaders.h"
#include "CsScriptComponentSerializer.h"
namespace Plaza {
	void ComponentSerializer::CsScriptComponentSerializer::SerializeAll(YAML::Emitter& out, uint64_t uuid) {
		out << YAML::Key << "CsScriptComponent" << YAML::BeginMap;
		out << YAML::Key << "Uuid" << YAML::Value << uuid;
		out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
		auto range = Application->activeScene->csScriptComponents.equal_range(uuid);
		for (auto it = range.first; it != range.second; ++it) {
			ComponentSerializer::CsScriptComponentSerializer::Serialize(out, it->second);
		}
		out << YAML::EndSeq; // Scripts

		out << YAML::EndMap; // CsScriptComponent
	}
	void ComponentSerializer::CsScriptComponentSerializer::Serialize(YAML::Emitter& out, CsScriptComponent& script) {
		out << YAML::BeginMap;

		out << YAML::Key << "Name" << YAML::Value << std::filesystem::path{ script.scriptPath }.stem().string();
		if (script.scriptPath.starts_with(Application->projectPath)) {
			out << YAML::Key << "Path" << YAML::Value << script.scriptPath.substr(Application->projectPath.size(), script.scriptPath.size() - Application->projectPath.size());
		}
		else {
			out << YAML::Key << "Path" << YAML::Value << script.scriptPath;
		}

		out << YAML::Key << "Uuid" << YAML::Value << script.uuid;

		out << YAML::Key << "SerializedFields" << YAML::BeginMap;
		for (auto& [scriptClassKey, scriptClass] : script.scriptClasses) {
			for (auto& [key, value] : scriptClass->fields) {
				out << YAML::Key << "Field" << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << mono_field_get_name(value);
				int val = 0;
				if (mono_type_get_type(mono_field_get_type(value)) == MONO_TYPE_I4)
					mono_field_get_value(scriptClass->monoObject, value, &val);


				out << YAML::Key << "Value" << YAML::Value << val;
				out << YAML::EndMap;
			}
		}

		out << YAML::EndMap;

		out << YAML::EndMap;
	}

	CsScriptComponent* ComponentSerializer::CsScriptComponentSerializer::DeSerialize(YAML::Node data) {
		CsScriptComponent* script = new CsScriptComponent(data["Uuid"].as<uint64_t>());
		std::string csFileName = Application->projectPath + data["Path"].as<std::string>();
		script->Init(csFileName);
		Application->activeProject->scripts.at(csFileName).entitiesUsingThisScript.emplace(data["Uuid"].as<uint64_t>());
		return script;
	}

	/*
						for (auto& [key, value] : Application->activeProject->scripts) {
						if (ImGui::MenuItem(filesystem::path{ key }.stem().string().c_str())) {
							CsScriptComponent* script = new CsScriptComponent(entity.uuid);
							std::string csFileName = filesystem::path{ key }.replace_extension(".cs").string();
							script->Init(csFileName);;
							Application->activeProject->scripts.at(csFileName).entitiesUsingThisScript.emplace(entity.uuid);
							if (Application->runningScene) {
								for (auto& [key, value] : script->scriptClasses) {
									Mono::OnStart(value->monoObject);
								}
							}
							entity.AddComponent<CsScriptComponent>(script);
						}
					}
	*/
}

/*
#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
namespace Plaza {
	class ComponentSerializer::CsScriptComponentSerializer {
	public:
		static void Serialize(YAML::Emitter& out, CsScriptComponent& script);
		static CsScriptComponent* DeSerialize(YAML::Node data);
	};
}
*/