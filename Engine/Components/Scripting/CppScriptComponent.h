#pragma once
#include "../Component.h"
#include "Engine/Components/Component.h"
#include "Engine/Core/Scripting/Mono.h"
namespace Plaza {
	class CsScriptComponent : public  Component {
	public:
		CsScriptComponent() {}
		CsScriptComponent(uint64_t uuid);
		std::string scriptPath;
		MonoObject* monoObject = nullptr;
		MonoMethod* onStartMethod = nullptr;
		MonoMethod* onUpdateMethod = nullptr;
		/// <summary>
		/// Instanties the class, get the start and update methods and set some other starting parameters
		/// </summary>
		/// <param name="csScriptPath">string path to the .cs file</param>
		void Init(std::string csScriptPath);
		void UpdateMethods() {
			onStartMethod = Mono::GetMethod(monoObject, "OnStart", 0);
			onUpdateMethod = Mono::GetMethod(monoObject, "OnUpdate", 0);
		}
		~CsScriptComponent();
	};
}