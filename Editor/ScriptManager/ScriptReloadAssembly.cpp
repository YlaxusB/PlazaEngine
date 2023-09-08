#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
#include "Engine/Core/Scripting/Mono.h"

namespace Plaza::Editor {
	MonoDomain* newDomain = nullptr;
	MonoObject* objectTest = nullptr;
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
					auto range = Application->activeScene->csScriptComponents.equal_range(entityUuid);
					for (auto it = range.first; it != range.second; ++it) {
						it->second.Init(csFilePath);
						for (auto& [key, value] : it->second.scriptClasses) {
							Mono::OnStart(value->monoObject);
						}
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

			/* Load the new domain */
			mono_set_assemblies_path("lib/mono");
			//mono_set_assemblies_path((Application->editorPath + "/lib/mono").c_str());
			if (Mono::mMonoRootDomain == nullptr)
				Mono::mMonoRootDomain = mono_jit_init("MyScriptRuntime");
			if (Mono::mMonoRootDomain == nullptr)
			{
				// Maybe log some error here
				return;
			}
			// Create an App Domain
			char appDomainName[] = "PlazaAppDomain";
			newDomain = mono_domain_create_appdomain(appDomainName, nullptr);
			mono_domain_set(newDomain, true);

			// Load the PlazaScriptCore.dll assembly
			Mono::mCoreAssembly = mono_domain_assembly_open(Mono::mAppDomain, (Application->dllPath + "\\PlazaScriptCore.dll").c_str());
			Mono::mCoreImage = mono_image_open((Application->dllPath + "\\PlazaScriptCore.dll").c_str(), nullptr);

			Mono::RegisterComponents();
			InternalCalls::Init();

			Mono::mEntityObject = Mono::InstantiateClass("Plaza", "Entity", Mono::LoadCSharpAssembly(Application->dllPath + "\\PlazaScriptCore.dll"), Mono::mAppDomain);
			Mono::mEntityClass = mono_object_get_class(Mono::mEntityObject);

			mono_domain_set(Mono::mAppDomain, true);

			/* Get and save variables of these scripts */
			std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> fields = ScriptManager::GetAllFields();
			Mono::ReloadAppDomain();
			Mono::mAppDomain = newDomain;
			mono_domain_set(Mono::mAppDomain, true);
			ScriptManager::RecompileDll(dllPath, scriptPath);
			// Initialize all scripts again
			for (auto& [key, value] : Application->activeScene->csScriptComponents) {
				std::string scriptPath = value.scriptPath;
				value = *new CsScriptComponent(key);
				value.Init(scriptPath);
			}

			ScriptManager::SaveAllFields(fields);
		}
	}

	void ScriptManager::RecompileDll(std::filesystem::path dllPath, std::string scriptPath) {
		// Recompile the C# script to .dll
		std::string compileCommand = "mcs -target:library -out:\"" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll\" " + "\"" + std::string(scriptPath) + "\"";
		compileCommand += " -reference:\"" + Application->dllPath + "\\PlazaScriptCore.dll\"";
		int result = system(compileCommand.c_str());
	}

	std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> ScriptManager::GetAllFields() {
		// Unordered map of Uuid with a map of script name with an unordered map of classes names and theirs values
		std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> fields;
		for (auto& [scriptPath, script] : Application->activeProject->scripts) {
			for (uint64_t entityUuid : script.entitiesUsingThisScript) {
				auto range = Application->activeScene->csScriptComponents.equal_range(entityUuid);
				for (auto it = range.first; it != range.second; ++it) {
					for (auto& [className, classScript] : it->second.scriptClasses) {
						// Get the fields of the class
						MonoClassField* field = NULL;
						void* iter = NULL;
						std::unordered_map<std::string, uint32_t> classFields;
						while ((field = mono_class_get_fields(classScript->klass, &iter)) != NULL)
						{
							// Check if the field is of type float (you can check other types as needed)
								// Get the name of the field
							const char* fieldName = mono_field_get_name(field);
							// Access and manipulate the field value (read or write)
							mono_domain_set(newDomain, true);
							MonoObject* fieldValue = mono_field_get_value_object(newDomain, field, classScript->monoObject);
							uint32_t handle = mono_gchandle_new(fieldValue, true);
							mono_domain_set(Mono::mAppDomain, true);
							classFields[fieldName] = handle;
							if (std::string(fieldName) == "a") {
								float floatValue = *(float*)mono_object_unbox(fieldValue);
								std::cout << "Primeiro: " << floatValue << std::endl;
							}

							// Access the Uuid field of the transform object
							if (std::string(fieldName) == "transform") {
								MonoClass* transformClass = mono_object_get_class(classScript->monoObject);
								MonoClassField* uuidField = mono_class_get_field_from_name(transformClass, "Uuid");
								int64_t uuidValue;
								mono_field_get_value(fieldValue, uuidField, &uuidValue);
								std::cout << "Transform Uuid Value on C++ Side 1 \n";
								std::cout << uuidValue << std::endl;
							}
							//fields.emplace(entityUuid, std::pair{ fieldName, fieldValue });
							// Now you have the field name (fieldName) and its value (fieldValue) in C++ 
						}
						fields[entityUuid].emplace(className, classFields);
					}
				}
			}
		}
		return fields;
	}

	void ScriptManager::GetField() {

	}

	void ScriptManager::SaveAllFields(std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> fields) {
		/* Add variables again */
		for (auto& [scriptPath, script] : Application->activeProject->scripts) {
			for (uint64_t entityUuid : script.entitiesUsingThisScript) {
				auto range = Application->activeScene->csScriptComponents.equal_range(entityUuid);
				for (auto it = range.first; it != range.second; ++it) {
					for (auto& [className, classScript] : it->second.scriptClasses) {
						MonoClassField* field = NULL;
						void* iter = NULL;
						while ((field = mono_class_get_fields(classScript->klass, &iter)) != NULL)
						{
							// Check if the field is of type float (you can check other types as needed)
								// Get the name of the field
							const char* fieldName = mono_field_get_name(field);

							// Access and manipulate the field value (read or write)
							if (fields.at(entityUuid).find(className) != fields.at(entityUuid).end()) {
								if (fields.at(entityUuid).at(className).find(fieldName) != fields.at(entityUuid).at(className).end()) {
									//MonoObject* fieldValue = mono_object_clone(fields.at(entityUuid).at(className).at(fieldName));

									//unsigned int gcHandle = mono_gchandle_new(fieldValue, true);
									uint32_t a = fields.at(entityUuid).at(className).at(fieldName);
									MonoObject* fieldObject = mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName));
									const char* fieldValue = mono_string_to_utf8(mono_object_to_string(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)), nullptr));
									// Assuming fieldValue is a C# string
									MonoString* monoStr = mono_object_to_string(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)), nullptr); // Get the C# string

									// Convert the C# string to a C string and then to a float
									const char* cStr = mono_string_to_utf8(monoStr);
									std::any floatValue = atof(cStr);
									//mono_field_set_value(classScript->monoObject, field, &floatValue);
									//mono_field_set_value(classScript->monoObject, field, &floatValue);

									if (std::string(fieldName) == "a") {
										const char* fieldValueStr = mono_string_to_utf8(mono_object_to_string(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)), nullptr));
										std::cout << "Saving heim: " << fieldValueStr << std::endl;
										mono_field_set_value(classScript->monoObject, field, &floatValue);
									}

									if (std::string(fieldName) == "transform") {
										const char* fieldValueStr = mono_string_to_utf8(mono_object_to_string(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)), nullptr));
										std::cout << "Transform Uuid Value on C++ Side 1 \n " << fieldValueStr << std::endl;
									}


								}
							}



							// Now you have the field name (fieldName) and its value (fieldValue) in C++

						}
					}
				}
			}
		}
	}

	void ScriptManager::SaveField() {

	}
}

/*
MonoClassField* field = mono_class_get_field_from_name(carscriptClass, "fieldName"); // Replace "fieldName" with the actual field name you want to access
void* fieldValue;
if (field) {
	MonoType* fieldType = mono_field_get_type(field);
	fieldValue = (void*)((char*)carscriptInstance + mono_field_get_offset(field));
}

// Unload the appDomain and reload again...

// Apply the fieldValue again
mono_field_set_value(monoObject, field, fieldValue);
*/