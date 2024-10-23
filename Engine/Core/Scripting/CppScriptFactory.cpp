#include "CppScriptFactory.h"
#include <unordered_map>
#include <string>

namespace Plaza {
	void ScriptFactory::InitializeRegistry() {
		//sRegistry = new std::unordered_map<std::string, CppScriptCreator>();
	}

	void ScriptFactory::RegisterScript(const std::string& name, CppScriptCreator creator) {
		GetRegistry()[name] = creator;
	}

	CppScript* ScriptFactory::CreateScript(const std::string name) {
		auto it = GetRegistry().find(name);
		if (it != GetRegistry().end()) {
			return it->second();  // Call the creation function
		}
		return nullptr;
	}

	std::unordered_map<std::string, CppScriptCreator>& ScriptFactory::GetRegistry() {
		return sRegistry;
	};
}