#include "Engine/Core/PreCompiledHeaders.h"
#include "EditorInspector.h"
#include "Editor/GUI/Utils/Utils.h"
#include<iostream>
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Settings/SettingsSerializer.h"
#include "Engine/Core/Renderer/Vulkan/VulkanBloom.h"

namespace Plaza::Editor {
	void EditorInspector::Update() {
		if (Utils::ComponentInspectorHeader(nullptr, "Editor")) {
			ImGui::PushID("EditorInspector");

			/* Editor Settings */
			if (Utils::ComponentInspectorHeader(nullptr, "Settings")) {
				if (ImGui::Checkbox("VSync", &Editor::Settings::vsync)) {
					Settings::ReapplyAllSettings();
				}

				
				ImGui::DragFloat("Bloom Intensity", &VulkanRenderer::GetRenderer()->mBloom.mBloomIntensity);
				ImGui::DragFloat("Bloom Knee", &VulkanRenderer::GetRenderer()->mBloom.mKnee);
				ImGui::DragFloat("Bloom Threshold", &VulkanRenderer::GetRenderer()->mBloom.mThreshold);
				ImGui::DragInt("Bloom Mip Count", &VulkanRenderer::GetRenderer()->mBloom.mMipCount);
				
				ImGui::DragFloat("Exposure", &VulkanRenderer::GetRenderer()->exposure);
				ImGui::DragFloat("Gamma", &VulkanRenderer::GetRenderer()->gamma);
				ImGui::ColorPicker3("Directional Light Color", &VulkanRenderer::GetRenderer()->sunColor.x);

				if (ImGui::Button("Save Settings")) {
					Editor::EditorSettingsSerializer::Serialize();
				}
			}

			/* Colors */
			if (Utils::ComponentInspectorHeader(nullptr, "Colors", NULL)) {
				int index = 0;
				for (auto& color : ImGui::GetStyle().Colors) {
					ImGui::PushID("EditorInspectorlabel" + index);
					float col[3];
					col[0] = color.x;
					col[1] = color.y;
					col[2] = color.z;
					if (ImGui::ColorPicker3(ImGui::GetStyleColorName(index), col)) {
						color.x = col[0];
						color.y = col[1];
						color.z = col[2];
					};
					index++;
					ImGui::PopID();
				}
			}
			ImGui::PopID();
		}
	}
}