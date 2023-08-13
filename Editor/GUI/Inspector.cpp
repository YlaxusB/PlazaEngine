#include "Engine/Core/PreCompiledHeaders.h"
#include "Inspector.h"

namespace Engine::Editor {
	std::vector<Component*> Inspector::ComponentInspector::components;
	void Inspector::ComponentInspector::CreateInspector() {
		if (ImGui::TreeNodeEx(Editor::selectedGameObject->name.c_str())) {
			ImGui::TreePop();
		}

		for (Component* component : components) {
			CreateRespectiveInspector(component);
		}
	}
	void Inspector::ComponentInspector::UpdateComponents() {
		components.clear();
		uint64_t uuid = Editor::selectedGameObject->uuid;
		Scene* activeScene = Application->activeScene;

		if (activeScene->transformComponents.contains(uuid))
			components.push_back(&activeScene->transformComponents.at(uuid));

		if (activeScene->meshRendererComponents.contains(uuid))
			components.push_back(&activeScene->meshRendererComponents.at(uuid));
	}

	void Inspector::ComponentInspector::CreateRespectiveInspector(Component* component) {
		if (Transform* transform = dynamic_cast<Transform*>(component)) {
			Editor::Gui::TransformInspector inspector{ Editor::selectedGameObject };
		}
		if (MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(component)) {
			Engine::Editor::MaterialInspector::MaterialInspector(Editor::selectedGameObject);
		}
	}
}