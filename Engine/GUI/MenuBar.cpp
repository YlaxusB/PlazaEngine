#include "Engine/Core/PreCompiledHeaders.h"
#include "MenuBar.h"

namespace Engine {
	namespace Editor {
		void Gui::MainMenuBar::Begin() {
			// MenuBar / TitleBar
			if (ImGui::BeginMainMenuBar()) {
				// Create an invisible margin to align the buttons to the right
				float spacingX = ImGui::GetWindowSize().x - ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x * 3;
				spacingX -= 300;
				ImGui::Dummy(ImVec2(spacingX, 0));

				// Create the right corner buttons
				ImGui::SameLine();
				if (ImGui::Button("Minimize")) {
					glfwIconifyWindow(Application->Window->glfwWindow);
				}
				ImGui::SameLine();
				if (ImGui::Button("Maximize")) {
					glfwMaximizeWindow(Application->Window->glfwWindow);
				}
				ImGui::SameLine();
				if (ImGui::Button("Close")) {
					glfwSetWindowShouldClose(Application->Window->glfwWindow, true);
				}

				ImGui::EndMainMenuBar();

			}
		}
	}
}