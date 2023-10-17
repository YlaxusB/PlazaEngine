#include "Engine/Core/PreCompiledHeaders.h"
#include "AudioListenerSerializer.h"
namespace Plaza {
	void ComponentSerializer::AudioListenerSerializer::Serialize(YAML::Emitter& out, AudioListener& audioListener) {
		out << YAML::Key << "AudioListenerComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << audioListener.uuid;
		out << YAML::Key << "Volume" << YAML::Value << audioListener.volume;

		out << YAML::EndMap; // 
	}

	AudioListener* ComponentSerializer::AudioListenerSerializer::DeSerialize(YAML::Node data) {
		AudioListener* audioListener = new AudioListener();
		audioListener->uuid = data["Uuid"].as<uint64_t>();
		if (data["Volume"])
			audioListener->volume = data["Volume"].as<int>();
		return audioListener;
	}
}