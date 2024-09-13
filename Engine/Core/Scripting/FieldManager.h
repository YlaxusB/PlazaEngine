#pragma once
#include <any>

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
	class FieldManager {
	public:
		static std::map<std::string, Field*> GetFieldsValues(MonoObject* monoObject, Field* parent = nullptr);
		static std::any GetFieldValue(MonoClassField* field, MonoObject* monoObject);
		static void FieldSetValue(int type, std::any& value, MonoObject* monoObject, MonoClassField* monoField, Field* field);
		static std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> GetAllScritpsFields();
		static void ApplyAllScritpsFields(std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> allFields);
	};
}