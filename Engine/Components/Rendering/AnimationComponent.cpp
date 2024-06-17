#include "Engine/Core/PreCompiledHeaders.h"
#include "AnimationComponent.h"

namespace Plaza {
	void Animation::Play() {
		this->mIsPlaying = true;
		Application->activeScene->mPlayingAnimations.emplace(this->mUuid, *this);
	}
}