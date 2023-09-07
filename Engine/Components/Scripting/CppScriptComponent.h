#pragma once
#include "../Component.h"
#include "Engine/Components/Component.h"
#include "Engine/Core/Scripting/Mono.h"
namespace Plaza {
	class PlazaScriptClass {
	public:
		PlazaScriptClass(MonoClass* klass, std::string namespaceName, std::string className, std::string dllPath, uint64_t uuid, MonoDomain* domain) {
				Init(klass, namespaceName, className, dllPath, uuid, domain);
			//this->name = className;
			//this->klass = klass;
			//this->monoObject = Mono::InstantiateClass(namespaceName.c_str(), className.c_str(), Mono::LoadCSharpAssembly(dllPath), domain, uuid);
		}
		void Init(MonoClass* klass, std::string namespaceName, std::string className, std::string dllPath, uint64_t uuid, MonoDomain* domain) {
			this->name = className;
			this->monoObject = Mono::InstantiateClass(namespaceName.c_str(), className.c_str(), Mono::LoadCSharpAssembly(dllPath), domain, uuid);
		}
		MonoObject* monoObject = nullptr;
		MonoMethod* onStartMethod = nullptr;
		MonoMethod* onUpdateMethod = nullptr;
		MonoClass* klass;
		std::string name;
	};

	class CsScriptComponent : public  Component {
	public:
		CsScriptComponent() {}
		CsScriptComponent(uint64_t uuid);
		std::string scriptPath;
		std::map<std::string, PlazaScriptClass*> scriptClasses;;
		MonoImage* image;
		MonoAssembly* assembly;
		MonoDomain* domain;
		/// <summary>
		/// Instanties the class, get the start and update methods and set some other starting parameters
		/// </summary>
		/// <param name="csScriptPath">string path to the .cs file</param>
		void Init(std::string csScriptPath);
		void UpdateMethods() {
			for (auto& [key, value] : scriptClasses) {
				value->onStartMethod = Mono::GetMethod(value->monoObject, "OnStart", 0);
				value->onUpdateMethod = Mono::GetMethod(value->monoObject, "OnUpdate", 0);
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
	};
}