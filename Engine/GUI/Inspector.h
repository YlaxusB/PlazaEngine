#pragma once
#include "Engine/GUI/Inspector/TransformInspector.h"

namespace Gui {
    class Inspector {
    public:
        void addComponentButton(AppSizes appSizes) {
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

        class ComponentInspector {


        public:

            GameObject*& gameObject; // The selected object

            ComponentInspector(GameObject*& gameObject) : gameObject(gameObject) {

                if (ImGui::TreeNodeEx(gameObject->name.c_str())) {
                    ImGui::TreePop();
                }

                for (Component*& component : gameObject->components) {
                    CreateRespectiveInspector(component);
                }
            }

            void CreateRespectiveInspector(Component* component) {
                if (Transform* transform = dynamic_cast<Transform*>(component)) {
                    Gui::TransformInspector inspector{ gameObject };
                }
            }
        };


    };
}