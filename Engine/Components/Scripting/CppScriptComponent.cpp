#include "Engine/Core/PreCompiledHeaders.h"
#include "CppScriptComponent.h"
namespace Plaza {
	CsScriptComponent::CsScriptComponent(uint64_t uuid) {
		this->uuid = uuid;
	}
	void CsScriptComponent::Init(std::string csScriptPath) {
		this->scriptPath = csScriptPath;
		std::string dllPath = std::filesystem::path{ csScriptPath }.replace_extension(".dll").string();

		std::string scriptName = std::filesystem::path{ csScriptPath }.stem().string();
		MonoClass* klass = mono_class_from_name(Mono::mScriptImage, "", scriptName.c_str()); // Replace with your class name
		if (klass != nullptr) {
			PlazaScriptClass* script = new PlazaScriptClass(klass, "", scriptName, dllPath, this->uuid, Mono::mAppDomain);
			//script->monoObject = mono_object_new(Mono::mAppDomain, klass);
			script->klass = klass;
			this->scriptClasses.emplace(scriptName, script);
			script->onStartMethod = mono_class_get_method_from_name(klass, "OnStart", 0);
			script->onUpdateMethod = mono_class_get_method_from_name(klass, "OnUpdate", 0);
			script->GetMethods();
		}
		Application->activeProject->scripts[csScriptPath].entitiesUsingThisScript.emplace(this->uuid);

		Application->activeProject->scripts[csScriptPath].entitiesUsingThisScript.emplace(this->uuid);
	}
	CsScriptComponent::~CsScriptComponent() {
		//Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript.erase(this->uuid);
	}
}