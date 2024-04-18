#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include "Editor/GUI/Utils/Utils.h"
#include <functional>
#include <iostream>

namespace Plaza::Editor {
	static class TextRendererInspector {
	public:
		TextRendererInspector(Drawing::UI::TextRenderer* textRenderer) {
			if (Utils::ComponentInspectorHeader(textRenderer, "Text Renderer Component")) {
				ImGui::PushID("TextRendererInspector");

				ImGui::Text("Text:");
				char buffer[1024];
				strcpy(buffer, textRenderer->mText.c_str());
				if (ImGui::InputText("##TextEditor", buffer, sizeof(buffer)))
					textRenderer->mText = buffer;

				ImGui::DragFloat("Scale", &textRenderer->mScale);
				ImGui::DragFloat("Position X", &textRenderer->mPosX);
				ImGui::DragFloat("Position Y", &textRenderer->mPosY);

				ImGui::PopID();
			}

		}
	};
}