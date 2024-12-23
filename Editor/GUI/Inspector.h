#pragma once
#include "Editor/GUI/Inspector/TransformInspector.h"

using namespace Plaza::Editor;
namespace Plaza::Editor {
	class Inspector {
	public:
		void addComponentButton() {
			ImVec2 button_size = ImVec2{ 150, 0 };

			// obtain width of window
			float width = ImGui::GetWindowSize().x;

			// figure out where we need to move the button to. It's good if you understand why this formula works!
			float centre_position_for_button = (width - button_size.x) / 2;

			// tell Dear ImGui to render the button at the current y pos, but with the new x pos
			ImGui::SetCursorPosX(centre_position_for_button);
			if (ImGui::Button("Hello!", button_size)) {
				std::cout << "clcioy" << std::endl;
			}
		}
		//class TransformInspector;

		class FileInspector {
		public:
			static void CreateInspector();
			static void CreateRespectiveInspector(File* file);
		};

		class ComponentInspector {
		public:
			static vector<Component*> components;

			static void CreateInspector(Scene* scene);
			static void UpdateComponents();

			static void CreateRespectiveInspector(Scene* scene, Component* component);
		};


	};
}