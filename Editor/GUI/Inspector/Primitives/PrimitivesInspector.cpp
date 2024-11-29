#include "Engine/Core/PreCompiledHeaders.h"
#include "PrimitivesInspector.h"
#include "ThirdParty/magic_enum/magic_enum.hpp"
#include "Engine/Core/EnumReflection.h"
#include "Engine/Core/Any.h"

namespace Plaza::Editor {
	void PrimitivesInspector::Init() {
	}
	bool PrimitivesInspector::InspectAny(Any& any, const std::string& fieldName, const std::string& comboPopupName) {
		std::string_view rawName = any.type().raw_name();
		try {
			if (rawName == typeid(float).raw_name()) {
				float* value = any.GetValue<float>();
				return InspectFloat(fieldName, *value);
			}
			else if (rawName == typeid(int).raw_name()) {
				int* value = any.GetValue<int>();
				return InspectInt(fieldName, *value);
			}
			else if (rawName == typeid(unsigned int).raw_name()) {
				unsigned int* value = any.GetValue<unsigned int>();
				return InspectUInt(fieldName, *value);
			}
			else if (rawName == typeid(uint32_t).raw_name()) {
				uint32_t* value = any.GetValue<uint32_t>();
				return InspectUInt32(fieldName, *value);
			}
			else if (rawName == typeid(uint64_t).raw_name()) {
				uint64_t* value = any.GetValue<uint64_t>();
				return InspectUInt64(fieldName, *value);
			}
			else if (rawName == typeid(std::string).raw_name()) {
				std::string* value = any.GetValue<string>();
				return InspectString(fieldName, *value);
			}
			else if (rawName == typeid(bool).raw_name()) {
				bool* value = any.GetValue<bool>();
				return InspectBool(fieldName, *value);
			}
			else if (rawName == typeid(glm::vec2).raw_name()) {
				glm::vec2* value = any.GetValue<glm::vec2>();
				return InspectVector2(fieldName, *value);
			}
			else if (rawName == typeid(glm::vec3).raw_name()) {
				glm::vec3* value = any.GetValue<glm::vec3>();
				return InspectVector3(fieldName, *value);
			}
			else if (rawName == typeid(glm::vec4).raw_name()) {
				glm::vec4* value = any.GetValue<glm::vec4>();
				return InspectVector4(fieldName, *value);
			}
			else if (EnumReflection::HasTypeRawName(std::string(rawName).c_str())) { // Enum
				return PrimitivesInspector::InspectEnum(any, comboPopupName);
			}
			else if (std::string(any.type().name()).starts_with("struct")) {
				std::cerr << "Struct inspection not implemented " << any.type().name() << std::endl;
			}
			else { // Struct or Class
				// Unknown type



				std::cerr << "Unknown type: " << any.type().name() << std::endl;
			}
		}
		catch (const std::bad_any_cast& e) {
			std::cerr << "Failed to cast type: " << e.what() << std::endl;
			return false;
		}
		return false;
	}

	bool PrimitivesInspector::InspectString(const std::string& name, std::string& value) {
		char buffer[256];
		std::strncpy(buffer, value.c_str(), sizeof(buffer));
		if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer))) {
			value = buffer;
			return true;
		}
		return false;
	}

	bool PrimitivesInspector::InspectBool(const std::string& name, bool& value) {
		return ImGui::Checkbox(name.c_str(), &value);
	}

	bool PrimitivesInspector::InspectEnum(Any& any, const std::string& comboPopupName) {
		const char* typeRawName = any.type().raw_name();
		const std::vector<const char*>& enumNames = EnumReflection::GetEnumNames(typeRawName);

		if (EnumReflection::IsBitmask(typeRawName)) {
			// Handle bitmask enums
			int currentValue = *any.GetValue<int>();
			bool modified = false;

			// Display checkboxes for each bitmask value
			if (ImGui::TreeNode(any.type().name())) {
				for (int i = 0; i < enumNames.size(); ++i) {
					if (enumNames[i] == nullptr)
						continue;
					int flag = 1 << i; // Get the bitmask value for this index
					bool isSet = (currentValue & flag) != 0;

					if (ImGui::Checkbox(enumNames[i], &isSet)) {
						if (isSet) {
							currentValue |= flag; // Set the flag
						}
						else {
							currentValue &= ~flag; // Clear the flag
						}
						modified = true;
					}
				}
				ImGui::TreePop();
			}

			if (modified) {
				any.SetValue(currentValue, false);
				return true;
			}
			return false;
		}
		else {
			// Handle regular enums
			int enumValue = *any.GetValue<int>();
			int currentIndex = static_cast<int>(enumValue);

			if (!comboPopupName.empty()) {
				if (ImGui::Button(any.type().name())) {
					ImGui::OpenPopup(comboPopupName.c_str());
					NodeEditor::sEnumToShowOnPopup = &any;
					NodeEditor::sOpenNodeEditorPopup = true;
				}
				return false;
			}

			if (ImGui::Combo("Enum", &currentIndex, enumNames.data(), static_cast<int>(enumNames.size()))) {
				any.SetValue(currentIndex, false);
				return true;
			}
			return false;
		}
	}

	bool PrimitivesInspector::InspectFloat(const std::string& name, float& value) {
		return ImGui::DragFloat(name.c_str(), &value);
	}

	bool PrimitivesInspector::InspectInt(const std::string& name, int& value) {
		return ImGui::DragInt(name.c_str(), &value);
	}

	bool PrimitivesInspector::InspectUInt(const std::string& name, unsigned int& value) {
		return ImGui::DragScalar(name.c_str(), ImGuiDataType_U32, &value);
	}

	bool PrimitivesInspector::InspectUInt32(const std::string& name, uint32_t& value) {
		return ImGui::DragScalar(name.c_str(), ImGuiDataType_U32, &value);
	}

	bool PrimitivesInspector::InspectUInt64(const std::string& name, uint64_t& value) {
		return ImGui::DragScalar(name.c_str(), ImGuiDataType_U64, &value);
	}

	bool PrimitivesInspector::InspectVector2(const std::string& name, glm::vec2& value) {
		return ImGui::DragFloat2(name.c_str(), &value.x);
	}

	bool PrimitivesInspector::InspectVector3(const std::string& name, glm::vec3& value) {
		return ImGui::DragFloat3(name.c_str(), &value.x);
	}

	bool PrimitivesInspector::InspectVector4(const std::string& name, glm::vec4& value) {
		return ImGui::DragFloat3(name.c_str(), &value.x);
	}
}