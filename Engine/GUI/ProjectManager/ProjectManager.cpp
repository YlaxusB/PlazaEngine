#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectManager.h"

#include "Engine/GUI/MenuBar.h"
#include "Engine/GUI/ProjectManager/ProjectItem.h"
#include "Engine/GUI/Style/EditorStyle.h"
namespace Engine {
	namespace Editor {
		void ProjectManagerGui::Update() {
			ProjectManagerGui::SetupDockspace();
		}

		void ProjectManagerGui::SetupDockspace() {
			// Imgui New Frame
			Gui::NewFrame();

			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

			//ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.hierarchyBackgroundColor);

			//	ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));

			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.hierarchyBackgroundColor);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

			if (ImGui::Begin("Main DockSpace", new bool(true), windowFlags)) {
				// Create the dockspace
				ImGuiID dockspace_id = ImGui::GetID("Main DockSpace");
				ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoDockingInCentralNode;
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

				Gui::MainMenuBar::Begin(); // Title bar

				NewProjectButton();

				SetupProjectsTreeNode();

				ImGui::End();

			}
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		void ProjectManagerGui::SetupProjectsTreeNode() {
			ImGuiWindowFlags containerFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			if (ImGui::Begin("Projects Container", nullptr, containerFlags)) {
				for (unsigned int i = 0; i < 10; i++) {
					std::unique_ptr<ProjectManagerGui::ProjectItem> guiItem = std::make_unique<ProjectManagerGui::ProjectItem>("nomeFoda" + std::to_string(i), "caminho/caminho2\\caminho3\\caminho4");
				}
			}
			ImGui::End();
		}

		void ProjectManagerGui::NewProjectButton() {
			ImGuiWindowFlags containerFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::SetNextWindowPos(ImVec2(800, 500));
			if (ImGui::Begin("Buttons Container", nullptr, containerFlags)) {

				bool newProjectButton = ImGui::Button("New Project", ImVec2(500, 100));

				if (newProjectButton) {
					NewProjectClick();
				}

			}
			ImGui::End();

		}
	}
}