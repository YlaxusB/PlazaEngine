#include "Engine/Core/PreCompiledHeaders.h"
#include "CsScriptComponent.h"
namespace Plaza {
	void CsScriptComponent::OnInstantiate(Component* componentToInstantiate) {
		// FIX: Move this to a manager
		//PLAZA_PROFILE_SECTION("CsScriptComponent");
		//auto range = Scene::GetActiveScene()->csScriptComponents.equal_range(mUuid);
		//vector<CsScriptComponent*> scriptsToAdd = vector<CsScriptComponent*>();
		//for (auto it = range.first; it != range.second && it != Scene::GetActiveScene()->csScriptComponents.end(); ++it) {
		//	if (it->second.mUuid == mUuid) {
		//		CsScriptComponent* newScript = new CsScriptComponent(mUuid);
		//		newScript->mScriptUuid = static_cast<CsScriptComponent*>(componentToInstantiate)->mScriptUuid;
		//		newScript->Init();
		//
		//		std::map<std::string, Field*> fields = std::map<std::string, Field*>();
		//
		//		uint64_t key = newScript->mUuid;
		//
		//		Application::Get()->activeProject->scripts.at(it->second.scriptPath).entitiesUsingThisScript.emplace(mUuid);
		//		if (Application::Get()->runningScene) {
		//			for (auto& [key, value] : newScript->scriptClasses) {
		//				Mono::OnStart(value->monoObject);
		//			}
		//		}
		//
		//		scriptsToAdd.push_back(newScript);
		//	}
		//}
		//for (CsScriptComponent* script : scriptsToAdd) {
		//	this->GetGameObject()->AddComponent<CsScriptComponent>(script);
		//}
	}

	CsScriptComponent::CsScriptComponent(uint64_t uuid) {
		this->mUuid = uuid;
	}
	void CsScriptComponent::Init() {
		std::string csScriptPath = AssetsManager::GetScript(mScriptUuid)->mAssetPath.string();
		this->scriptPath = csScriptPath;
		std::string dllPath = std::filesystem::path{ csScriptPath }.replace_extension(".dll").string();

		std::string scriptName = std::filesystem::path{ csScriptPath }.stem().string();
		MonoClass* klass = mono_class_from_name(Mono::mScriptImage, "", scriptName.c_str()); // Replace with your class name
		if (klass != nullptr) {
			PlazaScriptClass* script = new PlazaScriptClass(klass, "", scriptName, dllPath, this->mUuid, Mono::mAppDomain);
			//script->monoObject = mono_object_new(Mono::mAppDomain, klass);
			script->klass = klass;
			this->scriptClasses.emplace(scriptName, script);
			script->onStartMethod = mono_class_get_method_from_name(klass, "OnStart", 0);
			script->onUpdateMethod = mono_class_get_method_from_name(klass, "OnUpdate", 0);
			script->GetMethods();
		}
		Application::Get()->activeProject->scripts[csScriptPath].entitiesUsingThisScript.emplace(this->mUuid);

		Application::Get()->activeProject->scripts[csScriptPath].entitiesUsingThisScript.emplace(this->mUuid);
	}
	CsScriptComponent::~CsScriptComponent() {
		//Application::Get()->activeProject->scripts.at(scriptPath).entitiesUsingThisScript.erase(this->uuid);
	}
}