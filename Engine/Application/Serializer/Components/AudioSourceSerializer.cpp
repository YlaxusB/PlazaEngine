#include "Engine/Core/PreCompiledHeaders.h"
#include "AudioSourceSerializer.h"
namespace Plaza {
	std::string removeDirectoryFromString(const std::string& directory, const std::string& file) {
		// Find the position where the directory string ends in the file string
		size_t pos = file.find(directory);

		// If directory string is found in the file string
		if (pos != std::string::npos) {
			// Return the substring starting right after the directory string
			return file.substr(pos + directory.length());
		}

		// If directory string is not found, return the original file string
		return file;
	}

	void ComponentSerializer::AudioSourceSerializer::Serialize(YAML::Emitter& out, AudioSource& audioSource) {
		out << YAML::Key << "AudioSourceComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << audioSource.mUuid;
		out << YAML::Key << "SourcePath" << YAML::Value << removeDirectoryFromString(Application::Get()->projectPath, audioSource.mSourcePath);
		out << YAML::Key << "Gain" << YAML::Value << audioSource.mGain;
		out << YAML::Key << "Pitch" << YAML::Value << audioSource.mPitch;
		out << YAML::Key << "Spatial" << YAML::Value << audioSource.mSpatial;
		out << YAML::Key << "Loop" << YAML::Value << audioSource.mLoop;

		out << YAML::EndMap;
	}

	AudioSource* ComponentSerializer::AudioSourceSerializer::DeSerialize(YAML::Node data) {
		AudioSource* audioSource = new AudioSource();
		audioSource->mUuid = data["Uuid"].as<uint64_t>();
		if (data["SourcePath"])
			audioSource->mSourcePath = Application::Get()->projectPath + "\\" + data["SourcePath"].as<std::string>();
		if (!audioSource->mSourcePath.empty()) {
			audioSource->LoadFile(audioSource->mSourcePath);
		}
		if (data["Gain"])
			audioSource->SetGain(data["Gain"].as<float>());
		if (data["Pitch"])
			audioSource->SetPitch(data["Pitch"].as<float>());
		if (data["Spatial"])
			audioSource->SetSpatial(data["Spatial"].as<bool>());
		if (data["Loop"])
			audioSource->SetLoop(data["Loop"].as<bool>());
		return audioSource;
	}
}