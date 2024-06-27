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

				for (Animation& animation : component->mAnimations) {
					if (ImGui::TreeNodeEx(std::string("beg" + animation.mName).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
						BoneParentShipTree(animation.mRootBone);
						ImGui::TreePop();
					}
				}

				ImGui::PopID();
			}

		}
	private:
		void BoneParentShipTree(Bone* bone) {
			if (!bone)
				return;
			if (ImGui::TreeNodeEx(bone->mName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
				for(uint64_t childUuid : bone->mChildren)
				BoneParentShipTree(&VulkanRenderer::GetRenderer()->mBones.at(childUuid));
				ImGui::TreePop();
			}
		}
	};
}