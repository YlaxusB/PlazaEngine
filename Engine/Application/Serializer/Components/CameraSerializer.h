#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Core/Camera.h"
namespace Plaza {
	class ComponentSerializer::CameraSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Camera& camera);
		static Camera* DeSerialize(YAML::Node data);
	};
}