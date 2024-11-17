#include "Engine/Core/PreCompiledHeaders.h"
#include "PrimitivesInspector.h"
#include "ThirdParty/magic_enum/magic_enum.hpp"
#include "Engine/Core/EnumReflection.h"
#include "Engine/Core/Any.h"

namespace Plaza::Editor {
	void PrimitivesInspector::Init() {
	}
    bool PrimitivesInspector::InspectAny(Any& any) {
        try {
            if (any.type().raw_name() == typeid(float).raw_name()) {
                float* value = any.GetValue<float>();//std::any_cast<float&>(any);
                return InspectFloat("Float", *value);
            }
            else if (any.type().raw_name() == typeid(int).raw_name()) {
                int* value = any.GetValue<int>();//std::any_cast<int&>(any);
                return InspectInt("Int", *value);
            }
            else if (any.type().raw_name() == typeid(unsigned int).raw_name()) {
                unsigned int* value = any.GetValue<unsigned int>();//std::any_cast<unsigned int&>(any);
                return InspectUInt("Unsigned Int", *value);
            }
            else if (any.type().raw_name() == typeid(uint32_t).raw_name()) {
                uint32_t* value = any.GetValue<uint32_t>();//std::any_cast<uint32_t&>(any);
                return InspectUInt32("Unsigned Int 32", *value);
            }
            else if (any.type().raw_name() == typeid(std::string).raw_name()) {
                std::string* value = any.GetValue<string>();//std::any_cast<std::string&>(any);
                return InspectString("String", *value);
            }
            else if (any.type().raw_name() == typeid(bool).raw_name()) {
                bool* value = any.GetValue<bool>();//std::any_cast<bool&>(any);
                return InspectBool("Boolean", *value);
            }
            else if (EnumReflection::HasTypeRawName(any.type().raw_name())) { // Enum
                return PrimitivesInspector::InspectEnum(any);
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

    bool PrimitivesInspector::InspectEnum(Any& any) {
        const std::vector<const char*>& enumNames = EnumReflection::GetEnumNames(any.type().raw_name());
        int enumValue = *any.GetValue<int>();// to_underlyingw<int>(any.);//static_cast<int>(any);

        int currentIndex = static_cast<int>(enumValue);

        // Use ImGui::Combo to present enum options
        if (ImGui::Combo("Enum", &currentIndex, enumNames.data(), static_cast<int>(enumNames.size()))) {
            any = currentIndex;
            return true;
        }

        return false;
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
}