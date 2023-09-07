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
			std::string compileCommand = "mcs -target:library -out:\"" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll\" " + "\"" + std::string(csFilePath) + "\"";
			compileCommand += " -reference:\"" + Application->dllPath + "\\PlazaScriptCore.dll\"";
			std::cout << compileCommand << std::endl;
			int result = system(compileCommand.c_str());
			if (result == 0) {
				// Compilation Success

				//Mono::Init();

				//MonoClass* testingClass = Mono::GetClassInAssembly(Mono::LoadCSharpAssembly(dllPath.string()), "", "Unnamed");

				for (uint64_t entityUuid : Application->activeProject->scripts.at(csFilePath).entitiesUsingThisScript) {
					Application->activeScene->csScriptComponents.at(entityUuid).Init(csFilePath);
					//Application->activeScene->csScriptComponents.at(entityUuid).monoObject = Mono::InstantiateClass("", "Unnamed", Mono::LoadCSharpAssembly(dllPath.string()), Mono::mAppDomain, entityUuid);
					for (auto& [key, value] : Application->activeScene->csScriptComponents.at(entityUuid).scriptClasses) {
						Mono::OnStart(value->monoObject);
					}
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
			std::filesystem::path dllPath = std::filesystem::path{ scriptPath }.replace_extension(".dll");

			/* Get and save variables of these scripts */
			//std::unordered_map<uint64_t, std::unordered_map<std::string, MonoObject*>> fields;
			//for (uint64_t entityUuid : Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript) {
			//	for (auto& [className, classScript] : Application->activeScene->csScriptComponents.at(entityUuid).scriptClasses) {
			//		// Get the fields of the class
			//		MonoClassField* field = NULL;
			//		void* iter = NULL;
			//		std::unordered_map<std::string, MonoObject*> classFields;
			//		while ((field = mono_class_get_fields(classScript->klass, &iter)) != NULL)
			//		{
			//			// Check if the field is of type float (you can check other types as needed)
			//			if (mono_type_get_type(mono_field_get_type(field)) == MONO_TYPE_R4)
			//			{
			//				// Get the name of the field
			//				const char* fieldName = mono_field_get_name(field);

			//				// Access and manipulate the field value (read or write)
			//				MonoObject* fieldValue;
			//				mono_field_get_value(classScript->monoObject, field, &fieldValue);
			//				classFields[fieldName] = fieldValue;
			//				//fields.emplace(entityUuid, std::pair{ fieldName, fieldValue });
			//				// Now you have the field name (fieldName) and its value (fieldValue) in C++
			//			}
			//		}

			//		fields[entityUuid] = classFields;
			//	}
			//}

			/* Unload and Load again the app domain */
			mono_domain_set(mono_get_root_domain(), false);
			mono_domain_unload(Mono::mAppDomain);

			Mono::Init();


			// Recompile the C# script to .dll
			std::string compileCommand = "mcs -target:library -out:\"" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll\" " + "\"" + std::string(scriptPath) + "\"";
			compileCommand += " -reference:\"" + Application->dllPath + "\\PlazaScriptCore.dll\"";
			int result = system(compileCommand.c_str());
			if (result == 0) {
				// Reload the assembly in all entities using this script
				for (uint64_t entityUuid : Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript) {
					Application->activeScene->csScriptComponents.at(entityUuid).Init(scriptPath);
					for (auto& [className, classScript] : Application->activeScene->csScriptComponents.at(entityUuid).scriptClasses) {
						//Mono::OnStart(classScript->monoObject);
						//Mono::OnStart(Mono::InstantiateClass(className.c_str(), className.c_str(), Mono::LoadCSharpAssembly(dllPath.string()), Mono::mAppDomain, entityUuid));
					}
				}
			}

			for (auto& [key, value] : Application->activeScene->csScriptComponents) {
				if(value.scriptPath != scriptPath)
				value.Init(value.scriptPath);
			}

			/* Add variables again */
			//for (uint64_t entityUuid : Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript) {
			//	for (auto& [className, classScript] : Application->activeScene->csScriptComponents.at(entityUuid).scriptClasses) {
			//		MonoClassField* field = NULL;
			//		void* iter = NULL;
			//		while ((field = mono_class_get_fields(classScript->klass, &iter)) != NULL)
			//		{
			//			// Check if the field is of type float (you can check other types as needed)
			//			if (mono_type_get_type(mono_field_get_type(field)) == MONO_TYPE_R4)
			//			{
			//				// Get the name of the field
			//				const char* fieldName = mono_field_get_name(field);

			//				// Access and manipulate the field value (read or write)
			//				if (fields.at(entityUuid).find(fieldName) != fields.at(entityUuid).end()) {
			//					auto fieldValue = fields.at(entityUuid).at(fieldName);
			//					mono_field_set_value(classScript->monoObject, field, &fieldValue);
			//				}


			//				// Now you have the field name (fieldName) and its value (fieldValue) in C++
			//			}
			//		}
			//	}
			//}
		}
	}
}