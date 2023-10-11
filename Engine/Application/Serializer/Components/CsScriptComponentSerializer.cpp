#include "Engine/Core/PreCompiledHeaders.h"
#include "CsScriptComponentSerializer.h"
namespace Plaza {
	void ComponentSerializer::CsScriptComponentSerializer::SerializeObject(YAML::Emitter& out, MonoClassField* field, MonoObject* monoObject) {
		out << YAML::Key << "Value" << YAML::BeginSeq;
		MonoClass* klass = mono_object_get_class(monoObject);
		MonoClassField* subField;
		void* iter = NULL;
		while ((subField = mono_class_get_fields(klass, &iter))) {
			ComponentSerializer::CsScriptComponentSerializer::SerialieField(out, subField, monoObject);
		}
		out << YAML::EndSeq;
	}
	void ComponentSerializer::CsScriptComponentSerializer::SerializeValue(YAML::Emitter& out, MonoClassField* field, MonoObject* monoObject) {
		MonoType* fieldType = mono_field_get_type(field);
		if (mono_type_get_type(fieldType) == MONO_TYPE_I4) {
			int val = 0;
			mono_field_get_value(monoObject, field, &val);
			out << YAML::Key << "Value" << YAML::Value << val;
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_R4) {
			float val = 0.0f;
			mono_field_get_value(monoObject, field, &val);
			out << YAML::Key << "Value" << YAML::Value << val;
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_R8) {
			double val = 0.0;
			mono_field_get_value(monoObject, field, &val);
			out << YAML::Key << "Value" << YAML::Value << val;
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_STRING) {
			MonoString* str = nullptr;
			mono_field_get_value(monoObject, field, &str);
			if (str != nullptr) {
				const char* val = mono_string_to_utf8(str);
				out << YAML::Key << "Value" << YAML::Value << val;
				//mono_free(str);
			}
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_U8) {
			uint64_t val = 0;
			mono_field_get_value(monoObject, field, &val);
			out << YAML::Key << "Value" << YAML::Value << val;
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_ENUM) {
			int val = 0;
			mono_field_get_value(monoObject, field, &val);
			out << YAML::Key << "Value" << YAML::Value << val;
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_BOOLEAN) {
			bool val = 0;
			mono_field_get_value(monoObject, field, &val);
			out << YAML::Key << "Value" << YAML::Value << val;
		}
		else if (mono_type_get_type(fieldType) == MONO_TYPE_CLASS) {
			MonoObject* val;
			mono_field_get_value(monoObject, field, &val);
			if (val)
				ComponentSerializer::CsScriptComponentSerializer::SerializeObject(out, field, val);
		}
		else {
			std::cout << "Unsupported Mono Type for Serialization: " << mono_type_get_type(fieldType) << "\n";
			out << YAML::Key << "Value" << YAML::Value << "";
		}
	}
	void ComponentSerializer::CsScriptComponentSerializer::SerialieField(YAML::Emitter& out, MonoClassField* field, MonoObject* monoObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << mono_field_get_name(field);
		SerializeValue(out, field, monoObject);
		out << YAML::Key << "Access" << YAML::Value << mono_field_get_flags(field);
		out << YAML::Key << "Type" << YAML::Value << mono_type_get_type(mono_field_get_type(field));
		out << YAML::EndMap;
	}

	void ComponentSerializer::CsScriptComponentSerializer::SerializeAll(YAML::Emitter& out, uint64_t uuid) {
		out << YAML::Key << "CsScriptComponent" << YAML::BeginMap;
		out << YAML::Key << "Uuid" << YAML::Value << uuid;
		out << YAML::Key << "Scripts" << YAML::BeginSeq;
		auto range = Application->activeScene->csScriptComponents.equal_range(uuid);
		for (auto it = range.first; it != range.second; ++it) {
			ComponentSerializer::CsScriptComponentSerializer::Serialize(out, it->second);
		}
		out << YAML::EndSeq; // Scripts

		out << YAML::EndMap; // CsScriptComponent
	}
	void ComponentSerializer::CsScriptComponentSerializer::Serialize(YAML::Emitter& out, CsScriptComponent& script) {
		out << YAML::BeginMap;

		out << YAML::Key << "Name" << YAML::Value << std::filesystem::path{ script.scriptPath }.stem().string();
		if (script.scriptPath.starts_with(Application->projectPath)) {
			out << YAML::Key << "Path" << YAML::Value << script.scriptPath.substr(Application->projectPath.size(), script.scriptPath.size() - Application->projectPath.size());
		}
		else {
			out << YAML::Key << "Path" << YAML::Value << script.scriptPath;
		}

		out << YAML::Key << "Uuid" << YAML::Value << script.uuid;

		out << YAML::Key << "SerializedFields" << YAML::BeginSeq;
		for (auto& [scriptClassKey, scriptClass] : script.scriptClasses) {
			for (auto& [key, value] : scriptClass->fields) {
				CsScriptComponentSerializer::SerialieField(out, value, scriptClass->monoObject);
			}
		}

		out << YAML::EndSeq;

		out << YAML::EndMap;
	}

	void ComponentSerializer::CsScriptComponentSerializer::DeSerializeFieldValue(YAML::Node data, MonoClassField* field, MonoObject* monoObject, int type) {
		MonoType* fieldType = mono_field_get_type(field);
		if (type == MONO_TYPE_I4) {
			int val = data["Value"].as<int>();
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_R4) {
			float val = data["Value"].as<float>();
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_R8) {
			double val = data["Value"].as<double>();
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_STRING) {
			std::string val = data["Value"].as<std::string>();
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val = data["Value"].as<uint64_t>();
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_ENUM) {
			int val = data["Value"].as<int>();
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val = data["Value"].as<bool>();
			mono_field_set_value(monoObject, field, &val);
		}
		//else if (type == MONO_TYPE_CLASS) {
		//	ComponentSerializer::CsScriptComponentSerializer::DeSerializeField(data, mono_class_get_field_from_name(mono_object_get_class(monoObject), data["Name"].as<std::string>().c_str()), monoObject);
		//}
	}

	void ComponentSerializer::CsScriptComponentSerializer::DeSerializeField(YAML::Node data, MonoClassField* field, MonoObject* monoObject) {
		for (auto dataField : data["Value"]) {
			std::cout << data["Name"].as<std::string>() << std::endl;
			DeSerializeFieldValue(dataField, field, monoObject, dataField["Type"].as<int>());
		}
	}



	std::any ComponentSerializer::CsScriptComponentSerializer::GetValue(YAML::Node data, int type) {
		if (type == MONO_TYPE_I4) {
			int val = data["Value"].as<int>();
			return val;
		}
		else if (type == MONO_TYPE_R4) {
			float val = data["Value"].as<float>();
			return val;
		}
		else if (type == MONO_TYPE_R8) {
			double val = data["Value"].as<double>();
			return val;
		}
		else if (type == MONO_TYPE_STRING) {
			std::string val = data["Value"].as<std::string>();
			return val;
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val = data["Value"].as<uint64_t>();
			return val;
		}
		else if (type == MONO_TYPE_ENUM) {
			int val = data["Value"].as<int>();
			return val;
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val = data["Value"].as<bool>();
			return val;
		}
		else if (type == MONO_TYPE_CLASS) {
			bool val = data["Value"].as<bool>();
			return val;
		}
		//else if (type == MONO_TYPE_CLASS) {
		//	ComponentSerializer::CsScriptComponentSerializer::DeSerializeField(data, mono_class_get_field_from_name(mono_object_get_class(monoObject), data["Name"].as<std::string>().c_str()), monoObject);
		//}
	}

	void GetValuesFromSerializedFields(YAML::Node data, std::map<std::string, Field*>& fields, Field* parent = nullptr) {
		if (data["Type"].as<int>() == MONO_TYPE_CLASS) {
			Field* field = new Field();
			field->mName = data["Name"].as<std::string>();
			field->mType = data["Type"].as<int>();
			for (auto dataField : data["Value"]) {
				GetValuesFromSerializedFields(dataField, fields, field);
			}
			fields.emplace(data["Name"].as<std::string>(), field);
			//GetValuesFromSerializedFields(data["Value"], fields, parent);
		}
		else {
			auto result = ComponentSerializer::CsScriptComponentSerializer::GetValue(data, data["Type"].as<int>());
			if (!parent)
				fields.emplace(data["Name"].as<std::string>(), new Field(data["Name"].as<std::string>(), result, data["Type"].as<int>()));
			else
				parent->mChildren.emplace(data["Name"].as<std::string>(), new Field(data["Name"].as<std::string>(), result, data["Type"].as<int>()));
		}
	}

	void FieldSetValue(int type, std::any& value, MonoObject* monoObject, MonoClassField* field) {
		if (type == MONO_TYPE_I4) {
			int val = std::any_cast<int>(value);
			std::cout << "Name" << mono_field_get_name(field) << " Val: " << val << std::endl;
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_R4) {
			float val = std::any_cast<float>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_R8) {
			double val = std::any_cast<double>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_STRING) {
			//std::string val = std::any_cast<std::string>(value);
			//mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val = std::any_cast<uint64_t>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_ENUM) {
			int val = std::any_cast<int>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val = std::any_cast<bool>(value);
			mono_field_set_value(monoObject, field, &val);
		}
	}

	void ComponentSerializer::CsScriptComponentSerializer::SetFieldValue(MonoObject* monoObject, Field* field) {
		if (field) {
			if (field->mType != MONO_TYPE_CLASS) {
				MonoClassField* classField = mono_class_get_field_from_name(mono_object_get_class(monoObject), field->mName.c_str());
				//mono_field_set_value(monoObject, classField, &field->mValue);
				FieldSetValue(field->mType, field->mValue, monoObject, classField);
			}
			else {
				for (auto [name, child] : field->mChildren) {
					MonoObject* newMonoObject = nullptr;
					mono_field_get_value(monoObject, mono_class_get_field_from_name(mono_object_get_class(monoObject), field->mName.c_str()), &newMonoObject);
					SetFieldValue(newMonoObject, child);
				}
			}
		}
	}

	/*
					MonoObject* newMonoObject = nullptr;
				mono_field_get_value(monoObject, field, &newMonoObject);
				if (newMonoObject != nullptr) {
					void* iter = NULL;
					MonoClass* classInstance = mono_object_get_class(newMonoObject);
					MonoClassField* subField;
					while ((subField = mono_class_get_fields(classInstance, &iter)) != NULL) {
						ImGui::Text(mono_field_get_name(subField));
						CreateRespectiveInteractor(newMonoObject, subField);
					}
				}
	*/

	CsScriptComponent* ComponentSerializer::CsScriptComponentSerializer::DeSerialize(YAML::Node data) {
		CsScriptComponent* script = new CsScriptComponent(data["Uuid"].as<uint64_t>());
		std::string csFileName = Application->projectPath + data["Path"].as<std::string>();
		script->Init(csFileName);
		Application->activeProject->scripts.at(csFileName).entitiesUsingThisScript.emplace(data["Uuid"].as<uint64_t>());

		/* Get all fields and group them in a map*/
		std::map<std::string, Field*> fields = std::map<std::string, Field*>();
		if (data["SerializedFields"]) {
			for (auto dataField : data["SerializedFields"]) {
				GetValuesFromSerializedFields(dataField, fields);
			}
		}

		/* Apply the values got from the yaml */
		for (auto [scriptName, scriptClass] : script->scriptClasses) {
			for (auto [key, value] : fields) {
				if (value->mType != MONO_TYPE_CLASS && value->mType == MONO_TYPE_R4) {
					//mono_gchandle_new(scriptClass->monoObject, true);
					MonoClassField* classField = mono_class_get_field_from_name(mono_object_get_class(scriptClass->monoObject), value->mName.c_str());
					// float val = 1.0f;
					//mono_field_set_value(scriptClass->monoObject, classField, &val);
					FieldSetValue(value->mType, value->mValue, scriptClass->monoObject, classField);

				}
				else if (value->mType == MONO_TYPE_CLASS) {
					for (auto [name, child] : value->mChildren) {
						MonoObject* newMonoObject = nullptr;
						mono_field_get_value(scriptClass->monoObject, mono_class_get_field_from_name(mono_object_get_class(scriptClass->monoObject), value->mName.c_str()), &newMonoObject);
						SetFieldValue(newMonoObject, child);
						//SetFieldValue(scriptClass->monoObject, child);
					}
				}
			}
		}
		return script;
	}
}

/*
					MonoClassField* field = NULL;
					void* iter = NULL;
					std::unordered_map<std::string, uint32_t> classFields;
					if (scriptClass->monoObject->vtable) {
						while ((field = mono_class_get_fields(mono_object_get_class(scriptClass->monoObject), &iter)) != NULL)
						{
							ImGui::Text(mono_field_get_name(field));
							CreateRespectiveInteractor(scriptClass->monoObject, field);
*/

//for (auto& [key, scriptClass] : script->scriptClasses) {
//	MonoClass* klass = mono_object_get_class(scriptClass->monoObject);
//	MonoClassField* field;
//	void* iter = NULL;
//	while ((field = mono_class_get_fields(klass, &iter))) {
//		ComponentSerializer::CsScriptComponentSerializer::DeSerializeField(data, field, scriptClass->monoObject);
//	}
//}