#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Scripting.h"
#include "Mono.h"
#include "CppScript.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"

namespace Plaza {
	void Scripting::Update() {
		PLAZA_PROFILE_SECTION("Scripting: Update");
		Mono::Update();

		for (auto& [key, value] : Scene::GetActiveScene()->cppScriptComponents) {
			for (auto& script : value.mScripts) {
				script->OnUpdate();
			}
		}
	}
}