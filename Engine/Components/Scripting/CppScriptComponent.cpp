#include "Engine/Core/PreCompiledHeaders.h"
#include "CppScriptComponent.h"
namespace Plaza {
	CsScriptComponent::CsScriptComponent(uint64_t uuid) {
		this->uuid = uuid;
	}
	void CsScriptComponent::Init(std::string csScriptPath) {
		this->scriptPath = csScriptPath;
		std::string dllPath = std::filesystem::path{ csScriptPath }.replace_extension(".dll").string();
		this->monoObject = Mono::InstantiateClass("", "Unnamed", Mono::LoadCSharpAssembly(dllPath), Mono::mAppDomain, this->uuid);
		this->UpdateMethods();
		Application->activeProject->scripts[csScriptPath].entitiesUsingThisScript.emplace(this->uuid);
	}
	CsScriptComponent::~CsScriptComponent() {
		Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript.erase(this->uuid);
	}
}