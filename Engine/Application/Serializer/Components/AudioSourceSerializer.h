#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Audio/AudioSource.h"
namespace Plaza {
	class ComponentSerializer::AudioSourceSerializer {
	public:
		static void Serialize(YAML::Emitter& out, AudioSource& audioSource);
		static AudioSource* DeSerialize(YAML::Node data);
	};
}