#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Editor/GUI/Utils/Utils.h"

namespace Plaza::Editor {
	static class AudioListenerInspector {
	public:
		AudioListenerInspector(AudioListener* component) {
			if (Utils::ComponentInspectorHeader(component, "Audio Listener")) {
				ImGui::PushID("AudioListenerInspector");


				ImGui::PopID();
			}

		}
	};
}