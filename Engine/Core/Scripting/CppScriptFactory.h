#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <type_traits>
#include "Script.h"
#include <Windows.h>
#include "Engine/Core/Engine.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Core/Scripting/CppScript.h"

typedef void (*RegisterScriptsFunc)();
namespace Plaza {
	typedef std::function<CppScript* ()> CppScriptCreator;

	typedef std::unordered_map<std::string, Plaza::CppScriptCreator>& (*GetRegisterScriptsFunc)();

	extern class PLAZA_API ScriptFactory {
	public:
		static void InitializeRegistry();
		static void RegisterScript(const std::string& name, CppScriptCreator creator);
		static CppScript* CreateScript(const std::string name);

		static std::unordered_map<std::string, CppScriptCreator>& GetRegistry();
	private:
		static inline std::unordered_map<std::string, CppScriptCreator> sRegistry = std::unordered_map<std::string, CppScriptCreator>();
	};
	// Macro to automate the script registration
#define PL_REGISTER_SCRIPT(T) \
	static struct T##Registration { \
        T##Registration() { \
            Plaza::ScriptFactory::RegisterScript(#T, []() -> CppScript* { return new T(); }); \
        } \
    } T##Reg;
}