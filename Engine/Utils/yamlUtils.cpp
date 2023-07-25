#include "Engine/Core/PreCompiledHeaders.h"
#include "yamlUtils.h"
#include "Engine/Components/Core/Mesh.h"

namespace YAML {
	YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
}