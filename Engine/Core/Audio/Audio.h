#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include "AL/alext.h"
namespace Plaza {
	class Audio {
	public:
		static ALCdevice* sAudioDevice;
		static void Init();
		static void UpdateListener();
	};
}