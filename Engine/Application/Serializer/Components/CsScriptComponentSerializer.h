#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
namespace Plaza {
	class Field {
	public:
		Field() {};
		Field(std::string name, std::any value, int type) : mName(name), mValue(value), mType(type) {}
		std::any mValue;
		std::string mName;
		int mType;
		std::map<std::string, Field*> mChildren = std::map<std::string, Field*>();
	};
	class ComponentSerializer::CsScriptComponentSerializer {
	public:
		static void SerializeObject(YAML::Emitter& out, MonoClassField* field, MonoObject* monoObject);
		static void SerializeValue(YAML::Emitter& out, MonoClassField* field, MonoObject* monoObject);
		static void SerialieField(YAML::Emitter& out, MonoClassField* field, MonoObject* monoObject);
		static void SerializeAll(YAML::Emitter& out, uint64_t uuid);
		static void Serialize(YAML::Emitter& out, CsScriptComponent& script);
		static CsScriptComponent* DeSerialize(YAML::Node data);
		static void DeSerializeField(YAML::Node data, MonoClassField* field, MonoObject* monoObject);
		static void DeSerializeFieldValue(YAML::Node data, MonoClassField* field, MonoObject* monoObject, int type);

		static void SetFieldValue(MonoObject* monoObject, Field* field);
		static std::any GetValue(YAML::Node data, int type);
	};
}