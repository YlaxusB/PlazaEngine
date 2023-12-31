#pragma once
#include "Engine/Core/PreCompiledHeaders.h"

namespace YAML {
	extern YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v);
	extern YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& v);


	template<>
	struct convert<glm::vec3> {
		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4> {
		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			rhs.a = node[3].as<double>();
			return true;
		}
	};
}