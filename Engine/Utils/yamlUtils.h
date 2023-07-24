#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Vendor/yaml/include/yaml-cpp/yaml.h"
namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}