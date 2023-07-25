#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectManager.h"

#include "Editor/GUI/MenuBar.h"
#include "Editor/GUI/ProjectManager/ProjectItem.h"
#include "Editor/GUI/Style/EditorStyle.h"
#include "Engine/Application/Serializer/ProjectSerializer.h"
namespace Engine {
	namespace Editor {
		void ProjectManagerGui::ProjectManagerContent::UpdateContent(ProjectManagerGui& projectManagerGui) {
			projectManagerGui.NewProjectButton();
			ProjectManagerGui::SetupProjectsTreeNode();
		}
		/// <summary>
		/// Update the content of dockspace to show the New Project window
		/// </summary>
		/// <param name="projectManagerGui"></param>
		void ProjectManagerGui::NewProjectContent::UpdateContent(ProjectManagerGui& projectManagerGui){
			ImGuiWindowFlags containerFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus;
			if (ImGui::Begin("New Project Container", nullptr, containerFlags)) {
				// Make the: input text, create project button and cancel button
				static char name[256] = "";
				ImGui::InputText("##InputText", name, IM_ARRAYSIZE(name));
				if (ImGui::Button("Create Project") && std::string(name) != "") {
					Application->activeProject->name = std::string(name);

					Application->runEngine = true;
					Application->runProjectManagerGui = false;

					ProjectSerializer::Serialize(Application->activeProject->directory + "\\" + Application->activeProject->name + ".engprj");
				}
				if (ImGui::Button("Cancel")) {
					projectManagerGui.currentContent = new ProjectManagerContent();
				}
				
				ImGui::End();
			}
		}

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
			bool showProjectManager = true;
			if (ImGui::Begin("Main DockSpace", &showProjectManager, windowFlags)) {
				// Create the dockspace
				ImGuiID dockspace_id = ImGui::GetID("Main DockSpace");
				ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoDockingInCentralNode;
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

				Gui::MainMenuBar::Begin(); // Title bar

				this->currentContent->UpdateContent(*this);

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
					ProjectManagerGui::NewProjectClick();
				}

			}
			ImGui::End();
		}

		void ProjectManagerGui::OpenProjectButton() {
			ImGuiWindowFlags containerFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::SetNextWindowPos(ImVec2(800, 500));
			if (ImGui::Begin("Buttons Container", nullptr, containerFlags)) {

				bool openProjectButton = ImGui::Button("Open Project", ImVec2(500, 100));

				if (openProjectButton) {
					ProjectManagerGui::NewProjectClick();
				}

			}
			ImGui::End();
		}
	}
}