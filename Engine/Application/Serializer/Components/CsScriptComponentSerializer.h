#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
namespace Plaza {
	class ComponentSerializer::CsScriptComponentSerializer {
	public:
		static void SerializeAll(YAML::Emitter& out, uint64_t uuid);
		static void Serialize(YAML::Emitter& out, CsScriptComponent& script);
		static CsScriptComponent* DeSerialize(YAML::Node data);
	};
}