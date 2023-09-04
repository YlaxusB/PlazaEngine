#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
#include "Engine/Core/Scripting/Mono.h"

namespace Plaza::Editor {
	void ScriptManager::ReloadScriptsAssembly() {
		const auto& scriptsMap = ScriptManagerSerializer::DeSerialize(Application->activeProject->scriptsConfigFilePath);
		for (auto& [key, value] : Application->activeProject->scripts) {
			const char* csFilePath = key.c_str();
			std::filesystem::path dllPath = std::filesystem::path{ csFilePath }.replace_extension(".dll");
			std::string compileCommand = "mcs -target:library -out:" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll " + "\"" + std::string(csFilePath) + "\"";
			compileCommand += " -reference:" + Application->dllPath + "\\PlazaScriptCore.dll";
			std::cout << compileCommand << std::endl;
			int result = system(compileCommand.c_str());
			if (result == 0) {
				// Compilation Success

				//Mono::Init();

				//MonoClass* testingClass = Mono::GetClassInAssembly(Mono::LoadCSharpAssembly(dllPath.string()), "", "Unnamed");

				for (uint64_t entityUuid : Application->activeProject->scripts.at(csFilePath).entitiesUsingThisScript) {
					Application->activeScene->csScriptComponents.at(entityUuid).Init(csFilePath);
					//Application->activeScene->csScriptComponents.at(entityUuid).monoObject = Mono::InstantiateClass("", "Unnamed", Mono::LoadCSharpAssembly(dllPath.string()), Mono::mAppDomain, entityUuid);
					Mono::OnStart(Application->activeScene->csScriptComponents.at(entityUuid).monoObject);
				}

			}
			else {
				// Compilation failed
				// Handle error
			}
		}
	}

	void ScriptManager::ReloadSpecificAssembly(std::string scriptPath) {
		const auto& script = Application->activeProject->scripts.find(scriptPath);
		if (script != Application->activeProject->scripts.end()) {
			// Recompile the C# script to .dll
			std::filesystem::path dllPath = std::filesystem::path{ scriptPath }.replace_extension(".dll");
			std::string compileCommand = "mcs -target:library -out:\"" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll\" " + "\"" + std::string(scriptPath) + "\"";
			compileCommand += " -reference:\"" + Application->dllPath + "\\PlazaScriptCore.dll\"";
			std::cout << compileCommand << std::endl;
			int result = system(compileCommand.c_str());
			if (result == 0) {
				// Reload the assembly in all entities using this script
				for (uint64_t entityUuid : Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript) {
					Application->activeScene->csScriptComponents.at(entityUuid).Init(scriptPath);
					Mono::OnStart(Application->activeScene->csScriptComponents.at(entityUuid).monoObject);
				}
			}
		}
	}
}