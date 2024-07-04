#pragma once
#include "Engine/Components/Component.h"
#include <ThirdParty/AL/include/AL/al.h>
namespace Plaza {
	enum class AudioFileFormat
	{
		None = 0,
		MP3
	};
	class AudioSource : public Component {
	public:
		float mPosition[3] = { 0.0f, 0.0f, 0.0f };
		std::string mSourcePath;
		ALuint mSource = 0;
		void Play();
		void Stop();
		void LoadFile(std::string filePath);

		bool mLoop = false;
		bool mSpatial = true;
		float mPitch = 1.0f;
		float mGain = 1.0f;

		void SetPosition(float x, float y, float z);
		void SetPosition(glm::vec3 vec3);
		void SetGain(float value);
		void SetPitch(float value);
		void SetLoop(bool loop);
		void SetSpatial(bool spatial);

		//AudioSource(AudioSource& other) {
		//	this->SetGain(other.mGain);
		//	this->SetPitch(other.mPitch);
		//	this->SetLoop(other.mLoop);
		//	this->SetSpatial(other.mSpatial);
		//}

		~AudioSource() override {
			//this->Stop();
		}

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Component>(this), mPosition, mSourcePath, mSource, mLoop, mSpatial, mPitch, mGain);
		}
	};
}