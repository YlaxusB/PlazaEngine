#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporterWindow.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"

namespace Plaza {
	namespace Editor {
		void AssetsImporterWindow::Init() {
			auto onKeyPressLambda = [this](int key, int scancode, int action, int mods) {
				this->OnKeyPress(key, scancode, action, mods);
				};
			ApplicationClass::Callbacks::AddFunctionToKeyCallback({ onKeyPressLambda, GuiLayer::ASSETS_IMPORTER });
		}
		void AssetsImporterWindow::Update() {
			bool t = true;
			ImGui::ShowDebugLogWindow();
			//ImGui::ShowFontAtlas();
			//ImGui::StyleColorsDark();
			ImGui::ShowFontSelector("selec");
			ImGui::ShowMetricsWindow();
			ImGui::ShowStackToolWindow();
			ImGui::ShowStyleEditor();
			ImGui::ShowUserGuide();

			if (!this->IsOpen())
				return;
			ImGui::OpenPopup("Assets Importer");
			if (ImGui::BeginPopupModal("Assets Importer", NULL, ImGuiWindowFlags_MenuBar)) {
				if (ImGui::Button("Cancel")) {
					this->SetOpen(false);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

		}

		void AssetsImporterWindow::OnKeyPress(int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				this->SetOpen(false);
			}
		}
	}
}