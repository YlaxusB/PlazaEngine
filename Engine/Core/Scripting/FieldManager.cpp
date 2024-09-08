#include "Engine/Core/PreCompiledHeaders.h"
#include "FieldManager.h"
namespace Plaza {
	std::any FieldManager::GetFieldValue(MonoClassField* field, MonoObject* monoObject) {
		std::string a = mono_field_get_name(field);
		if (a == "truck") {
			std::cout << "this is a truck \n";
		}
		int type = mono_type_get_type(mono_field_get_type(field));
		if (type == MONO_TYPE_I4) {
			int val;
			mono_field_get_value(monoObject, field, &val);
			//std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << val << std::endl;
			return val;
		}
		else if (type == MONO_TYPE_R4) {
			float val;
			mono_field_get_value(monoObject, field, &val);
			//std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << val << std::endl;
			return val;
		}
		else if (type == MONO_TYPE_R8) {
			double val;
			mono_field_get_value(monoObject, field, &val);
			//std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << val << std::endl;
			return val;
		}
		else if (type == MONO_TYPE_STRING) {
			MonoString* str = nullptr;
			mono_field_get_value(monoObject, field, &str);
			if (str != nullptr) {
				const char* val = mono_string_to_utf8(str);
				std::string stringVal = val;
				//std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << stringVal << std::endl;
				return stringVal;
			}
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val;
			mono_field_get_value(monoObject, field, &val);
			//	std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << val << std::endl;
			return val;
		}
		else if (type == MONO_TYPE_ENUM) {
			int val;
			mono_field_get_value(monoObject, field, &val);
			//std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << val << std::endl;
			return val;
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val;
			mono_field_get_value(monoObject, field, &val);
			//std::cout << "Get Name: " << mono_field_get_name(field) << " Get Value: " << val << std::endl;
			return val;
		}
		else if (type == MONO_TYPE_CLASS) {
			//bool val;
			//mono_field_get_value(monoObject, field, &val);
			//return val;
		}
		else {
			std::cout << "Get Field Unsupported " << type << " Name: " << mono_field_get_name(field) << "\n";
		}
		return std::any();
	}

	void FieldManager::FieldSetValue(int type, std::any& value, MonoObject* monoObject, MonoClassField* monoField, Field* field) {
		std::string a = mono_field_get_name(monoField);
		if (a == "truck") {
			std::cout << "this is a truck \n";
		}
		if (type == MONO_TYPE_I4) {
			int val = std::any_cast<int>(value);
			mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_R4) {
			float val = std::any_cast<float>(value);
			mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_R8) {
			double val = std::any_cast<double>(value);
			mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_STRING) {
			//std::string val = std::any_cast<std::string>(value);
			//mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val = std::any_cast<uint64_t>(value);
			mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_ENUM) {
			int val = std::any_cast<int>(value);
			mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val = std::any_cast<bool>(value);
			mono_field_set_value(monoObject, monoField, &val);
		}
		else if (type == MONO_TYPE_CLASS) {
			MonoClassField* subField = NULL;
			void* iter = NULL;
			MonoObject* currentFieldMonoObject = nullptr;
			std::string fieldName = mono_field_get_name(monoField);
			mono_field_get_value(monoObject, mono_class_get_field_from_name(mono_object_get_class(monoObject), mono_field_get_name(monoField)), &currentFieldMonoObject);
			if (currentFieldMonoObject) {
				while ((subField = mono_class_get_fields(mono_object_get_class(currentFieldMonoObject), &iter)) != NULL)
				{
					std::string b = mono_field_get_name(subField);
					if (b == "truck") {
						std::cout << "this is a truck \n";
					}
					int newType = mono_type_get_type(mono_field_get_type(subField));
					std::string newName = mono_field_get_name(subField);
					if (newType == MONO_TYPE_CLASS) {
						MonoObject* newMonoObject = nullptr;
						mono_field_get_value(currentFieldMonoObject, mono_class_get_field_from_name(mono_object_get_class(currentFieldMonoObject), mono_field_get_name(subField)), &newMonoObject);
						FieldManager::FieldSetValue(newType, field->mChildren.at(newName)->mValue, newMonoObject, subField, field->mChildren.at(newName));
					}
					else {
						if (field->mChildren.find(newName) != field->mChildren.end())
							FieldManager::FieldSetValue(field->mChildren.at(newName)->mType, field->mChildren.at(newName)->mValue, currentFieldMonoObject, subField, field->mChildren.at(newName));
					}
				}

				// Recursively collect fields from the base class if available
				MonoClass* baseClass = mono_class_get_parent(mono_object_get_class(currentFieldMonoObject));
				if (baseClass) {
					void* iter2 = NULL;
					MonoClassField* parentField;
					while ((parentField = mono_class_get_fields(baseClass, &iter2)) != NULL) {
						std::string newName = mono_field_get_name(subField);
						int newType = mono_type_get_type(mono_field_get_type(parentField));
						FieldManager::FieldSetValue(newType, field->mChildren.at(newName)->mValue, currentFieldMonoObject, parentField, field->mChildren.at(newName));
						//CreateRespectiveInteractor(newMonoObject, parentField, sliderIndex);
						//CreateRespectiveInteractor(nullptr, parentField, sliderIndex);
					}
				}
			}

		}
		else {
			std::cout << "Save Field Unsupported " << type << " Name: " << mono_field_get_name(monoField) << "\n";
		}
	}


	std::map<std::string, Field*> FieldManager::GetFieldsValues(MonoObject* monoObject, Field* parent) {
		std::map<std::string, Field*> fields = std::map<std::string, Field*>();
		MonoClassField* monoField = NULL;
		void* iter = NULL;
		while ((monoField = mono_class_get_fields(mono_object_get_class(monoObject), &iter)) != NULL)
		{
			std::cout << mono_field_get_name(monoField) << "\n";
			Field* field = new Field();
			field->mType = mono_type_get_type(mono_field_get_type(monoField));
			field->mName = mono_field_get_name(monoField);
			if (field->mType != MONO_TYPE_CLASS && field->mType != MONO_TYPE_ARRAY && field->mType != MONO_TYPE_GENERICINST) {
				auto result = GetFieldValue(monoField, monoObject);
				field->mValue = result;
				fields.emplace(field->mName, field);
			}
			else if (field->mType == MONO_TYPE_CLASS) {
				if (!parent)
					parent = field;
				MonoObject* newMonoObject = nullptr;
				mono_field_get_value(monoObject, mono_class_get_field_from_name(mono_object_get_class(monoObject), field->mName.c_str()), &newMonoObject);
				MonoObject* fieldObject = mono_field_get_value_object(Mono::mAppDomain, monoField, monoObject);
				if (newMonoObject) {
					field->mChildren = FieldManager::GetFieldsValues(newMonoObject, field);
					fields.emplace(field->mName, field);
				}

			}
		}

		// Recursively collect fields from the base class if available
		MonoClass* baseClass = mono_class_get_parent(mono_object_get_class(monoObject));
		if (baseClass) {
			void* iter2 = NULL;
			MonoClassField* parentField;
			while ((parentField = mono_class_get_fields(baseClass, &iter2)) != NULL) {
				std::map<std::string, Field*> parentFields = FieldManager::GetFieldsValues(mono_field_get_value_object(Mono::mAppDomain, parentField, monoObject));
				for (auto& [key, value] : parentFields) {
					fields.emplace(value->mName, value);
				}
				//fields.emplace(mono_field_get_name(parentField), FieldManager::GetFieldsValues(monoObject));
					//CreateRespectiveInteractor(newMonoObject, parentField, sliderIndex);
					//CreateRespectiveInteractor(nullptr, parentField, sliderIndex);
			}
		}

		return fields;
	}

	std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> FieldManager::GetAllScritpsFields()
	{
		std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> allFields = std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>>();
		for (auto& [key, value] : Application::Get()->activeScene->csScriptComponents) {
			for (auto& [scriptClassName, scriptClassValue] : value.scriptClasses) {
				std::map<std::string, Field*> map = FieldManager::GetFieldsValues(scriptClassValue->monoObject);
				std::map<std::string, std::map<std::string, Field*>> subMap = std::map<std::string, std::map<std::string, Field*>>();
				subMap.emplace(scriptClassValue->name, map);
				allFields.emplace(key, subMap);
			}
		}
		return allFields;
	}

	void FieldManager::ApplyAllScritpsFields(std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> allFields)
	{
		for (auto& [key, value] : Application::Get()->activeScene->csScriptComponents) {
			for (auto& [scriptClassKey, scriptClassValue] : value.scriptClasses) {
				MonoClassField* monoField = NULL;
				void* iter = NULL;
				if (allFields.find(key) != allFields.end() && allFields.at(key).find(scriptClassValue->name) != allFields.at(key).end()) {
					std::map<std::string, Field*> fields = allFields.at(key).at(scriptClassValue->name);
					while ((monoField = mono_class_get_fields(mono_object_get_class(scriptClassValue->monoObject), &iter)) != NULL)
					{
						int type = mono_type_get_type(mono_field_get_type(monoField));
						if (type != MONO_TYPE_ARRAY) {
							if (fields.find(mono_field_get_name(monoField)) != fields.end())
								FieldManager::FieldSetValue(type, fields.at(mono_field_get_name(monoField))->mValue, scriptClassValue->monoObject, monoField, fields.at(mono_field_get_name(monoField)));
						}
					}
				}
			}
		}
	}
}