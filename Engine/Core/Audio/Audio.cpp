#include "Engine/Core/PreCompiledHeaders.h"
#include "Audio.h"
namespace Plaza {
	ALCdevice* Audio::sAudioDevice = nullptr;
	int InitAL(ALCdevice*& device, char*** argv, int* argc)
	{
		ALCcontext* ctx;

		/* Open and initialize a device */
		device = NULL;
		if (argc && argv && *argc > 1 && strcmp((*argv)[0], "-device") == 0)
		{
			device = alcOpenDevice((*argv)[1]);
			if (!device)
				fprintf(stderr, "Failed to open \"%s\", trying default\n", (*argv)[1]);
			(*argv) += 2;
			(*argc) -= 2;
		}
		if (!device)
			device = alcOpenDevice(NULL);
		if (!device)
		{
			fprintf(stderr, "Could not open a device!\n");
			return 1;
		}

		ctx = alcCreateContext(device, NULL);
		if (ctx == NULL || alcMakeContextCurrent(ctx) == ALC_FALSE)
		{
			if (ctx != NULL)
				alcDestroyContext(ctx);
			alcCloseDevice(device);
			fprintf(stderr, "Could not set a context!\n");
			return 1;
		}

		return 0;
	}

	void Audio::Init() {
		PL_CORE_INFO("Initializating Audio");
		// Init OpenAL
		if (InitAL(sAudioDevice, nullptr, 0) != 0)
			std::cout << "Audio device error!\n";

		// Init listener
		Application::Get()->activeCamera;
		ALfloat listenerPos[] = { 0.0f,0.0f,0.0f };
		ALfloat listenerVel[] = { 0.0,0.0f,0.0 };
		ALfloat listenerOri[] = { 0.0,0.0, -1.0, 0.0,1.0,0.0 };
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOri);
	}

	void Audio::UpdateListener() {
		PLAZA_PROFILE_SECTION("Update Audio");
		if (Application::Get()->runningScene && Application::Get()->activeCamera != Application::Get()->editorCamera) {
			if (Scene::GetActiveScene()->entities.find(Application::Get()->activeCamera->mUuid) != Scene::GetActiveScene()->entities.end() && Scene::GetActiveScene()->GetEntity(Application::Get()->activeCamera->mUuid)->HasComponent<AudioListener>())
				Scene::GetActiveScene()->GetEntity(Application::Get()->activeCamera->mUuid)->GetComponent<AudioListener>()->UpdateListener();
		}
		else {
			glm::vec3 position = Application::Get()->activeCamera->Position;
			// Init listener
			Application::Get()->activeCamera;
			ALfloat listenerPos[] = { position.x, position.y, position.z };
			ALfloat listenerVel[] = { 0.0,0.0f,0.0 };

			glm::vec3 forward = Application::Get()->activeCamera->Front;
			glm::vec3 up = Application::Get()->activeCamera->Up;
			glm::vec3 right = Application::Get()->activeCamera->Right;
			float listenerOrientation[6] = {
				forward.x, forward.y, forward.z,
				up.x, up.y, up.z
			};
			alListenerfv(AL_POSITION, listenerPos);
			alListenerfv(AL_VELOCITY, listenerVel);
			alListenerfv(AL_ORIENTATION, listenerOrientation);
		}
	}
}