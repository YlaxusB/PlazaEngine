#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
#include "Engine/Core/Scripting/Mono.h"

namespace Plaza::Editor {
	void ScriptManager::ReloadScriptsAssembly() {
		const auto& scriptsMap = ScriptManagerSerializer::DeSerialize(Application->activeProject->scriptsConfigFilePath);
		for (auto& [key, value] : scriptsMap) {
			const char* csFilePath = key.c_str();
			std::filesystem::path dllPath = std::filesystem::path{ csFilePath }.replace_extension(".dll");
			std::string compileCommand = "mcs -target:library -out:" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll " + "\"" + std::string(csFilePath) + "\"";
			compileCommand += " -reference:" + Application->dllPath + "\\PlazaScriptCore.dll";
			std::cout << compileCommand << std::endl;
			int result = system(compileCommand.c_str());
			if (result == 0) {
				// Compilation Success

				Mono::Init();

				MonoClass* testingClass = Mono::GetClassInAssembly(Mono::LoadCSharpAssembly(dllPath.string()), "", "Unnamed");

				// Allocate an instance of our class
				MonoObject* classInstance = mono_object_new(Mono::mAppDomain, testingClass);

				MonoObject* reloadedMonoObject = Mono::InstantiateClass("", "Unnamed", Mono::LoadCSharpAssembly(dllPath.string()), Mono::mAppDomain);
				Application->activeProject->monoObjects.at(dllPath.string()) = reloadedMonoObject;//Mono::InstantiateClass("", "Unnamed", Mono::mCoreAssembly, Mono::mAppDomain);
				for (uint64_t entityUuid : Application->activeProject->scripts.at(csFilePath).entitiesUsingThisScript) {
					Application->activeScene->cppScriptComponents.at(entityUuid).monoObject = Mono::InstantiateClass("", "Unnamed", Mono::LoadCSharpAssembly(dllPath.string()), Mono::mAppDomain);;
				}

			}
			else {
				// Compilation failed
				// Handle error
			}
		}
	}
}