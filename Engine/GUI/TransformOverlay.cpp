#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/GUI/TransformOverlay.h";
namespace Engine::Editor {
	
	ImGuizmo::MODE Editor::Overlay::activeMode = ImGuizmo::MODE::WORLD;
	ImGuizmo::OPERATION Editor::Overlay::activeOperation = ImGuizmo::OPERATION::TRANSLATE;

	void Editor::Overlay::beginTransformOverlay(Camera camera) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Transform Overlay", new bool(true), window_flags))
		{
			ImGui::SetWindowPos(ImVec2(400, Application->appSizes->sceneStart.y + ImGui::GetWindowPos().y), ImGuiCond_Once);
			Button* worldButton = new Button{ "World", ImGuizmo::OPERATION(-1), ImGuizmo::MODE::WORLD };
			createButton(worldButton, activeMode, activeOperation);

			Button* localButton = new Button{ "Local", ImGuizmo::OPERATION(-1), ImGuizmo::MODE::LOCAL };
			createButton(localButton, activeMode, activeOperation);

			Button* translateButton = new Button{ "Translate", ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE(-1) };
			createButton(translateButton, activeMode, activeOperation);

			Button* rotationButton = new Button{ "Rotate", ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE(-1) };
			createButton(rotationButton, activeMode, activeOperation);

			Button* scaleButton = new Button{ "Scale", ImGuizmo::OPERATION::SCALE, ImGuizmo::MODE(-1) };
			createButton(scaleButton, activeMode, activeOperation);

			Button* universalButton = new Button{ "Universal", ImGuizmo::OPERATION::UNIVERSAL, ImGuizmo::MODE(-1) };
			createButton(universalButton, activeMode, activeOperation);

			ImGui::End();
		}
	}
}