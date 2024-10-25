#pragma once
#include "../Component.h"
#include "Engine/Components/Component.h"
#include "Engine/Core/Scripting/Mono.h"
#include <cereal/archives/json.hpp>
#include "Engine/Core/Engine.h"

namespace Plaza {
	class PLAZA_API PlazaScriptClass {
	public:
		PlazaScriptClass(MonoClass* klass, std::string namespaceName, std::string className, std::string dllPath, uint64_t uuid, MonoDomain* domain) {
			Init(klass, namespaceName, className, dllPath, uuid, domain);
			//this->name = className;
			//this->klass = klass;
			//this->monoObject = Mono::InstantiateClass(namespaceName.c_str(), className.c_str(), Mono::LoadCSharpAssembly(dllPath), domain, uuid);
		}
		void Init(MonoClass* klass, std::string namespaceName, std::string className, std::string dllPath, uint64_t uuid, MonoDomain* domain) {
			this->name = className;
			// Allocate an instance of our class
			MonoObject* classInstance = mono_object_new(Mono::mAppDomain, klass);
			unsigned int gcHandle = mono_gchandle_new(classInstance, true);
			if (uuid) {
				MonoClassField* uuidField = mono_class_get_field_from_name(klass, "Uuid");
				mono_field_set_value(classInstance, uuidField, &uuid);
			}

			// Call the parameterless (default) constructor
			mono_runtime_object_init(classInstance);
			this->monoObject = classInstance;

			// Get fields
			MonoClassField* field;
			void* iter = NULL;
			while ((field = mono_class_get_fields(klass, &iter))) {
				fields.emplace(mono_field_get_name(field), field);
			}
		}
		unordered_map<std::string, MonoClassField*> fields = unordered_map<std::string, MonoClassField*>();
		MonoObject* monoObject = nullptr;
		MonoMethod* onStartMethod = nullptr;
		MonoMethod* onUpdateMethod = nullptr;
		MonoClass* klass;
		std::string name;
		std::unordered_map<std::string, MonoMethod*> methods = std::unordered_map<std::string, MonoMethod*>();

		void GetMethods() {
			methods.clear();
			methods.emplace("OnCollide", GetMethod(this->monoObject, "OnCollide"));
			methods.emplace("OnTrigger", GetMethod(this->monoObject, "OnTrigger"));
		}

	private:
		MonoMethod* GetMethod(MonoObject* monoObject, std::string methodName) {
			return Mono::GetMethod(monoObject, methodName, 2);
		}
	};

	class PLAZA_API CsScriptComponent : public  Component {
	public:
		CsScriptComponent() {}
		CsScriptComponent(uint64_t uuid);
		uint64_t mScriptUuid = 0;
		std::string scriptPath;
		std::map<std::string, PlazaScriptClass*> scriptClasses;;
		MonoImage* image;
		MonoAssembly* assembly;
		MonoDomain* domain;
		/// <summary>
		/// Instanties the class, get the start and update methods and set some other starting parameters
		/// </summary>
		/// <param name="csScriptPath">string path to the .cs file</param>
		void Init();
		void UpdateMethods() {
			for (auto& [key, value] : scriptClasses) {
				value->onStartMethod = Mono::GetMethod(value->monoObject, "OnStart", 0);
				value->onUpdateMethod = Mono::GetMethod(value->monoObject, "OnUpdate", 0);
				value->GetMethods();
			}
		}
		void FreeMethods() {
			//mono_domain_set(domain, false);

			mono_assembly_close(assembly);
			//mono_domain_unload(domain);
			////mono_free(this->image);
			//for (auto& [key, value] : this->scriptClasses) {
			//	mono_free_method(value->onStartMethod);
			//	mono_free_method(value->onUpdateMethod);
			//}
			//mono_domain_set(Mono::mAppDomain, false);
		}
		~CsScriptComponent();

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Component>(this), PL_SER(scriptPath), PL_SER(mScriptUuid));
		}
	};
}