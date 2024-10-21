#include "CppScriptFactory.h"
#include <unordered_map>
#include <string>

namespace Plaza {
	void ScriptFactory::RegisterScript(const std::string& name, ScriptCreator creator) {
		GetRegistry()[name] = creator;
	}

	Script* ScriptFactory::CreateScript(const std::string name) {
		auto it = GetRegistry().find(name);
		if (it != GetRegistry().end()) {
			return it->second();  // Call the creation function
		}
		return nullptr;
	}

	std::unordered_map<std::string, ScriptCreator>& ScriptFactory::GetRegistry() {
		return s_Registry;
	};
}