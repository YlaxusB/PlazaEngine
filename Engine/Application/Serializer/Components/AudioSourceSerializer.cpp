#include "Engine/Core/PreCompiledHeaders.h"
#include "AudioSourceSerializer.h"
namespace Plaza {
	void ComponentSerializer::AudioSourceSerializer::Serialize(YAML::Emitter& out, AudioSource& audioSource) {
		out << YAML::Key << "AudioSourceComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << audioSource.uuid;
		out << YAML::Key << "SourcePath" << YAML::Value << audioSource.mSourcePath;
		out << YAML::Key << "Gain" << YAML::Value << audioSource.mGain;
		out << YAML::Key << "Pitch" << YAML::Value << audioSource.mPitch;

		out << YAML::EndMap;
	}

	AudioSource* ComponentSerializer::AudioSourceSerializer::DeSerialize(YAML::Node data) {
		AudioSource* audioSource = new AudioSource();
		audioSource->uuid = data["Uuid"].as<uint64_t>();
		if (data["SourcePath"])
			audioSource->mSourcePath = data["SourcePath"].as<std::string>();
		if (data["Gain"])
			audioSource->mGain = data["Gain"].as<float>();
		if (data["Pitch"])
			audioSource->mPitch = data["Pitch"].as<float>();
		if (!audioSource->mSourcePath.empty()) {
			audioSource->LoadFile(audioSource->mSourcePath);
		}
		return audioSource;
	}
}