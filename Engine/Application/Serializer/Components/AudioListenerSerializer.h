#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Audio/AudioSource.h"
namespace Plaza {
	class ComponentSerializer::AudioListenerSerializer {
	public:
		static void Serialize(YAML::Emitter& out, AudioListener& audioListener);
		static AudioListener* DeSerialize(YAML::Node data);
	};
}