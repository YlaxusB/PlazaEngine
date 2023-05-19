#pragma once
#include "Engine/GUI/Inspector/TransformInspector.h"

namespace Gui {
    class Inspector {
    public:
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