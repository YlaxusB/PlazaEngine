#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
#include "Engine/Application/Serializer/Components/CsScriptComponentSerializer.h"
#include "Engine/Core/Scripting/Mono.h"

/*
		 Get all fields and group them in a map
std::map<std::string, Field*> fields = std::map<std::string, Field*>();
if (data["SerializedFields"]) {
	for (auto dataField : data["SerializedFields"]) {
		GetValuesFromSerializedFields(dataField, fields);
	}
}

 Apply the values got from the yaml
for (auto [scriptName, scriptClass] : script->scriptClasses) {
	for (auto [key, value] : fields) {
		if (value->mType != MONO_TYPE_CLASS && value->mType == MONO_TYPE_R4) {
			mono_gchandle_new(scriptClass->monoObject, true);
			MonoClassField* classField = mono_class_get_field_from_name(mono_object_get_class(scriptClass->monoObject), key.c_str());
			// float val = 1.0f;
			//mono_field_set_value(scriptClass->monoObject, classField, &val);
			FieldSetValue(value->mType, value->mValue, scriptClass->monoObject, classField);

		}
		else if (value->mType == MONO_TYPE_CLASS) {
			for (auto [name, child] : value->mChildren) {
				MonoObject* newMonoObject = nullptr;
				mono_field_get_value(scriptClass->monoObject, mono_class_get_field_from_name(mono_object_get_class(scriptClass->monoObject), value->mName.c_str()), &newMonoObject);
				//SetFieldValue(newMonoObject, child);
				//SetFieldValue(scriptClass->monoObject, child);
			}
		}
	}
}
*/



namespace Plaza::Editor {


	MonoObject* CopyMonoObject(MonoObject* objectToCopy);
	MonoDomain* newDomain = nullptr;
	MonoObject* objectTest = nullptr;
	void ScriptManager::ReloadScriptsAssembly(std::string dllPath) {

		/* Load the new domain */
		mono_set_assemblies_path("lib/mono");
		//mono_set_assemblies_path((Application::Get()->editorPath + "/lib/mono").c_str());
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

		Mono::ReloadAppDomain();
		Mono::mAppDomain = newDomain;

		mono_domain_set(Mono::mAppDomain, true);

		// Load the PlazaScriptCore.dll assembly
		Mono::mCoreAssembly = mono_domain_assembly_open(Mono::mAppDomain, (Application::Get()->dllPath + "\\PlazaScriptCore.dll").c_str());
		Mono::mCoreImage = mono_image_open((Application::Get()->dllPath + "\\PlazaScriptCore.dll").c_str(), nullptr);

		Mono::RegisterComponents();
		InternalCalls::Init();

		Mono::mEntityObject = Mono::InstantiateClass("Plaza", "Entity", Mono::LoadCSharpAssembly(Application::Get()->dllPath + "\\PlazaScriptCore.dll"), Mono::mAppDomain);
		Mono::mEntityClass = mono_object_get_class(Mono::mEntityObject);

		mono_domain_set(Mono::mAppDomain, true);
		Mono::mScriptAssembly = mono_domain_assembly_open(Mono::mAppDomain, dllPath.c_str());
		if (Mono::mScriptAssembly) {
			Mono::mScriptImage = mono_assembly_get_image(Mono::mScriptAssembly);

			// Initialize all scripts again
			for (auto& [key, value] : Application::Get()->activeScene->csScriptComponents) {
				std::string scriptPath = value.scriptPath;
				value = *new CsScriptComponent(key);
				value.Init(scriptPath);
			}
		}
	}

	void ScriptManager::ReloadScriptsAssembly() {
		bool scriptDllExists = std::filesystem::exists(Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->name }.stem().string() + ".dll");
		if (scriptDllExists) {
#ifdef  GAME_REL
			ScriptManager::ReloadScriptsAssembly((Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->name }.stem().string() + ".dll").c_str());
#else
			ScriptManager::ReloadScriptsAssembly((Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->name }.stem().string() + "copy.dll").c_str());
#endif //  GAME_REL
	}
}

	void ScriptManager::ReloadSpecificAssembly(std::string scriptPath) {
		const auto& script = Application::Get()->activeProject->scripts.find(scriptPath);
		if (script != Application::Get()->activeProject->scripts.end()) {
			std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> allFields = FieldManager::GetAllScritpsFields();
			std::filesystem::path dllPath = std::filesystem::path{ scriptPath }.replace_extension(".dll");

			/* Load the new domain */
			mono_set_assemblies_path("lib/mono");
			//mono_set_assemblies_path((Application::Get()->editorPath + "/lib/mono").c_str());
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
			Mono::mCoreAssembly = mono_domain_assembly_open(Mono::mAppDomain, (Application::Get()->dllPath + "\\PlazaScriptCore.dll").c_str());
			Mono::mCoreImage = mono_image_open((Application::Get()->dllPath + "\\PlazaScriptCore.dll").c_str(), nullptr);

			Mono::RegisterComponents();
			InternalCalls::Init();

			Mono::mEntityObject = Mono::InstantiateClass("Plaza", "Entity", Mono::LoadCSharpAssembly(Application::Get()->dllPath + "\\PlazaScriptCore.dll"), Mono::mAppDomain);
			Mono::mEntityClass = mono_object_get_class(Mono::mEntityObject);

			mono_domain_set(Mono::mAppDomain, true);

			/* Get and save variables of these scripts */
			std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> fields;
			/* fields = ScriptManager::GetAllFields() */;
			Mono::ReloadAppDomain();
			Mono::mAppDomain = newDomain;

			mono_domain_set(Mono::mAppDomain, true);
			ScriptManager::RecompileDll(dllPath, scriptPath);
			/* Load a copy of the script dll if its running the editor, so it can recompile the dll without breaking anything (its temporary, since I dont have hot reloading yet)*/
#ifdef GAME_MODE
			std::string scriptDllPath = (Application::Get()->projectPath + "\\Binaries\\" + Application::Get()->activeProject->name + ".dll");
#else
			std::string scriptDllPath = (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->name }.stem().string() + "copy.dll");
#endif
			Mono::mScriptAssembly = mono_domain_assembly_open(Mono::mAppDomain, scriptDllPath.c_str());
			Mono::mScriptImage = mono_assembly_get_image(Mono::mScriptAssembly);
			if (!Mono::mScriptAssembly) {
				// Handle assembly loading error
				std::cout << "Failed to load assembly when recompiling on path: " << (Application::Get()->projectPath + "\\Binaries\\" + Application::Get()->activeProject->name + ".dll").c_str() << "\n";
			}
			// Initialize all scripts again
			for (auto& [key, value] : Application::Get()->activeScene->csScriptComponents) {
				std::string scriptPath = value.scriptPath;
				value = *new CsScriptComponent(key);
				value.Init(scriptPath);
			}

			/* Reapply fields values */
			FieldManager::ApplyAllScritpsFields(allFields);
			/* Todo: Fix it */
			//if (Application::Get()->runningScene) {
			//	for (auto& [key, value] : Application::Get()->editorScene->csScriptComponents) {
			//		std::string scriptPath = value.scriptPath;
			//		value = *new CsScriptComponent(key);
			//		value.Init(scriptPath);
			//	}
			//}
			/* ScriptManager::SaveAllFields(fields); */
		}
	}

	void ScriptManager::RecompileDll(std::filesystem::path dllPath, std::string scriptPath) {
		// Recompile the C# script to .dll
		//std::string compileCommand = "mcs -target:library -out:\"" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll\" " + "\"" + std::string(scriptPath) + "\"";
		//compileCommand += " -reference:\"" + Application::Get()->dllPath + "\\PlazaScriptCore.dll\"";
		std::string compileCommand = "dotnet build ";
		compileCommand += "\"" + Application::Get()->projectPath + "\\" + std::filesystem::path{ Application::Get()->activeProject->name }.stem().string() + ".csproj \"";
		int result = system(compileCommand.c_str());
	}

	std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> ScriptManager::GetAllFields() {
		// Unordered map of Uuid with a map of script name with an unordered map of classes names and theirs values
		std::unordered_map<uint64_t, std::map<std::string, std::unordered_map<std::string, uint32_t>>> fields;
		for (auto& [scriptPath, script] : Application::Get()->activeProject->scripts) {
			for (uint64_t entityUuid : script.entitiesUsingThisScript) {
				auto range = Application::Get()->activeScene->csScriptComponents.equal_range(entityUuid);
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
		for (auto& [scriptPath, script] : Application::Get()->activeProject->scripts) {
			for (uint64_t entityUuid : script.entitiesUsingThisScript) {
				auto range = Application::Get()->activeScene->csScriptComponents.equal_range(entityUuid);
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

									MonoType* fieldType = mono_field_get_type(field);
									if (mono_type_get_type(fieldType) == MONO_TYPE_BOOLEAN ||
										mono_type_get_type(fieldType) == MONO_TYPE_CHAR ||
										mono_type_get_type(fieldType) == MONO_TYPE_I1 ||
										mono_type_get_type(fieldType) == MONO_TYPE_U1 ||
										mono_type_get_type(fieldType) == MONO_TYPE_I2 ||
										mono_type_get_type(fieldType) == MONO_TYPE_U2 ||
										mono_type_get_type(fieldType) == MONO_TYPE_I4 ||
										mono_type_get_type(fieldType) == MONO_TYPE_U4 ||
										mono_type_get_type(fieldType) == MONO_TYPE_I8 ||
										mono_type_get_type(fieldType) == MONO_TYPE_U8 ||
										mono_type_get_type(fieldType) == MONO_TYPE_R4 ||
										mono_type_get_type(fieldType) == MONO_TYPE_R8 ||
										mono_type_get_type(fieldType) == MONO_TYPE_STRING) {
										std::cout << "Primitive: " << fieldName << "\n";
										const char* fieldValueStr = mono_string_to_utf8(mono_object_to_string(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)), nullptr));
										float floatValue = atof(fieldValueStr);
										std::cout << "Float: " << fieldValueStr << std::endl;
										mono_field_set_value(classScript->monoObject, field, &floatValue);
									}
									else {
										std::cout << "Not Primitive: " << fieldName << "\n";
										MonoClass* transformClass = mono_object_get_class(classScript->monoObject);
										MonoObject* valueObject = CopyMonoObject(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)));
										uint64_t value = 550;

										MonoClass* monoClass = mono_object_get_class(classScript->monoObject);
										MonoObject* classInstance = mono_object_new(Mono::mAppDomain, monoClass);
										MonoClassField* uuidField = mono_class_get_field_from_name(monoClass, "Uuid");
										mono_field_set_value(classInstance, uuidField, &value);
										mono_field_set_value(classInstance, mono_class_get_field_from_name(mono_object_get_class(classInstance), "Uuid"), &value);
										mono_gchandle_new(classInstance, true);
										mono_runtime_object_init(classInstance);
										//mono_field_set_value(classScript->monoObject, field, mono_object_unbox(classInstance));
										uint64_t value2;
										mono_field_get_value(classInstance, mono_class_get_field_from_name(mono_object_get_class(classInstance), "Uuid"), &value2);

										std::cout << "Vaulor2222: " << value2 << "\n";

										/*mono_runtime_object_init(valueObject);
										mono_field_set_value(valueObject, mono_class_get_field_from_name(mono_object_get_class(valueObject), "Uuid"), &value);
										mono_field_set_value(classScript->monoObject, field, (void*)mono_object_unbox(valueObject));

										mono_field_set_value(classScript->monoObject, mono_class_get_field_from_name(mono_object_get_class(valueObject), "Uuid"), &value);*/
										/*

											// Get a reference to the class we want to instantiate
											MonoClass* monoClass = GetClassInAssembly(assembly, namespaceName, className);

											// Allocate an instance of our class
											MonoObject* classInstance = mono_object_new(appDomain, monoClass);
											unsigned int gcHandle = mono_gchandle_new(classInstance, true);
											if (uuid) {
												MonoClassField* uuidField = mono_class_get_field_from_name(monoClass, "Uuid");
												mono_field_set_value(classInstance, uuidField, &uuid);
											}

											// Call the parameterless (default) constructor
											mono_runtime_object_init(classInstance);
											return classInstance;
										*/
									}

									/*
										MonoClass* transformClass = mono_object_get_class(classScript->monoObject);
										MonoClassField* uuidField = mono_class_get_field_from_name(transformClass, "Uuid");
										int64_t uuidValue;
										mono_field_get_value(fieldValue, uuidField, &uuidValue);
									*/

									//if (std::string(fieldName) == "a") {
									//	const char* fieldValueStr = mono_string_to_utf8(mono_object_to_string(mono_gchandle_get_target(fields.at(entityUuid).at(className).at(fieldName)), nullptr));
									//	std::cout << "Saving heim: " << fieldValueStr << std::endl;
									//	mono_field_set_value(classScript->monoObject, field, &floatValue);
									//}



								}
							}



							// Now you have the field name (fieldName) and its value (fieldValue) in C++

						}
					}
				}
			}
		}
	}

	MonoObject* CopyMonoObject(MonoObject* objectToCopy) {
		MonoObject* objectToPaste = mono_object_clone(objectToCopy);
		uint64_t value;
		mono_field_get_value(objectToPaste, mono_class_get_field_from_name(mono_object_get_class(objectToPaste), "Uuid"), &value);

		std::cout << "Vaulor: " << value << "\n";

		return objectToPaste;
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