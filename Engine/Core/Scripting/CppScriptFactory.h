#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <type_traits>
#include "Script.h"
#include <Windows.h>

typedef void (*RegisterScriptsFunc)();
namespace Plaza {
	typedef std::function<Script* ()> ScriptCreator;

	typedef std::unordered_map<std::string, Plaza::ScriptCreator>& (*GetRegisterScriptsFunc)();

	class PLAZA_API ScriptFactory {
	public:
		static void RegisterScript(const std::string& name, ScriptCreator creator);
		static Script* CreateScript(const std::string name);

		static std::unordered_map<std::string, ScriptCreator>& GetRegistry();
	private:
		static inline std::unordered_map<std::string, ScriptCreator> s_Registry = std::unordered_map<std::string, ScriptCreator>();
	};
	// Macro to automate the script registration
#define PL_REGISTER_SCRIPT(T) \
	static struct T##Registration { \
        T##Registration() { \
            Plaza::ScriptFactory::RegisterScript(#T, []() -> Script* { return new T(); }); \
        } \
    } T##Reg;
}