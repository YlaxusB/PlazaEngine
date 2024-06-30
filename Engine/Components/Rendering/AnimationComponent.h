#pragma once

namespace Plaza {
	class Animation {
	public:
		uint64_t mUuid = 0;
		std::string mName = "";
		float mStartTime = 0.0f;
		float mEndTime = 0.0f;
		float mCurrentTime = 0.0f;
		float mAnimationSpeed = 1.0f;
		Bone* mRootBone = nullptr;
		std::map<uint64_t, std::vector<Bone::Keyframe>> mKeyframes = std::map<uint64_t, std::vector<Bone::Keyframe>>();

		void Play();
		void Stop() {};
		void UpdateTime(float deltaTime) {
			mCurrentTime += deltaTime * mAnimationSpeed;
			if (mCurrentTime > mEndTime)
				SetTime(mStartTime);
		}
		void SetTime(float time) { this->mCurrentTime = time; }
		bool GetState() { return mIsPlaying; };

	private:
		bool mIsPlaying = false;
		std::vector<int64_t> mBonesIds = std::vector<int64_t>();
	};

	class AnimationComponent : public Component {
	public:
		float mImportScale = 1.0f;//0.01f;
		std::vector<Animation> mAnimations = std::vector<Animation>();
		void GetAnimation(std::string filePath, std::map<uint64_t, Plaza::Bone>& bonesMap, unsigned int index = 0); //{
	private:
		friend class Animation;
	};
}