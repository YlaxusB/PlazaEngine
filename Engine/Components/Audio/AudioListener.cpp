#include "Engine/Core/PreCompiledHeaders.h"
#include "AudioListener.h"
namespace Plaza {
	void AudioListener::UpdateListener() {
		mPosition = Application->activeCamera->Position;
		// Init listener
		Application->activeCamera;
		ALfloat listenerPos[] = { mPosition.x, mPosition.y, mPosition.z };
		ALfloat listenerVel[] = { 0.0,0.0f,0.0 };

		glm::vec3 forward = Application->activeCamera->Front;
		glm::vec3 up = Application->activeCamera->Up;
		glm::vec3 right = Application->activeCamera->Right;
		float listenerOrientation[6] = {
			forward.x, forward.y, forward.z,
			up.x, up.y, up.z
		};
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOrientation);
	}
}