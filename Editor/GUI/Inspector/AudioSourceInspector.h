#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Core/Audio/Audio.h"
namespace Plaza::Editor {
	static class AudioSourceInspector {
	public:
		AudioSourceInspector(AudioSource* component) {
			if (Utils::ComponentInspectorHeader(component, "Audio Source")) {
				ImGui::PushID("AudioSourceInspector");

				if (ImGui::Button("Select File")) {
					std::string filePath = FileDialog::OpenFileDialog(".mp3");
					Audio::Init();
					component->LoadFile(filePath);
				}

				if (ImGui::Button("Play Sound")) {
					component->SetPosition(component->GetGameObject()->GetComponent<Transform>()->GetWorldPosition());
					component->SetSpatial(true);
					component->Play();
				}

				if (ImGui::Button("Stop Sound")) {
					component->Stop();
				}

				if (ImGui::DragFloat("Gain", &component->mGain)) {
					component->SetGain(component->mGain);
				}

				if (ImGui::DragFloat("Pitch", &component->mPitch)) {
					component->SetPitch(component->mPitch);
				}

				ImGui::PopID();
			}

		}
	};
}