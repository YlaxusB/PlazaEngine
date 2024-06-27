#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Engine/Components/Rendering/AnimationComponent.h"
#include "Engine/Application/FileDialog/FileDialog.h"

namespace Plaza::Editor {
	static class AnimationComponentInspector {
	public:
		static inline int index = 0;
		AnimationComponentInspector(AnimationComponent* component) {
			if (Utils::ComponentInspectorHeader(component, "Animation Component")) {
				ImGui::PushID("AnimationComponentInspector");

				ImGui::InputInt("Animation Index: ", &index);

				if (ImGui::Button("Load animations from FBX")) {
					std::string path = FileDialog::OpenFileDialog(".fbx");
					component->GetAnimation(path, VulkanRenderer::GetRenderer()->mBones, index);
				}

				for (Animation& animation : component->mAnimations) {
					if (ImGui::Button(std::string("Play" + animation.mName).c_str())) {
						Application->activeScene->mPlayingAnimations.clear();
						animation.Play();
					}
				}

				ImGui::PopID();
			}

		}
	};
}