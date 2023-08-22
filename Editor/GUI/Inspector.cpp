#include "Engine/Core/PreCompiledHeaders.h"
#include "Inspector.h"

#include "Editor/GUI/Inspector/RigidBodyInspector.h"
#include "Editor/GUI/Inspector/SceneInspector.h"
#include "Editor/GUI/Inspector/ColliderInspector.h"

namespace Engine::Editor {
	std::vector<Component*> Inspector::ComponentInspector::components;
	void Inspector::ComponentInspector::CreateInspector() {
		ImGui::SetCursorPosY(50);
		ImGui::Indent(10);
		if (Editor::selectedGameObject->uuid == Application->activeScene->mainSceneEntity->uuid) {
			SceneInspector::SceneInspector(Application->activeScene);
		}
		else {
			ImGui::Text(Editor::selectedGameObject->name.c_str());
			for (Component* component : components) {
				CreateRespectiveInspector(component);
			}
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

		if (activeScene->rigidBodyComponents.contains(uuid))
			components.push_back(&activeScene->rigidBodyComponents.at(uuid));

		if (activeScene->colliderComponents.contains(uuid))
			components.push_back(&activeScene->colliderComponents.at(uuid));
	}

	void Inspector::ComponentInspector::CreateRespectiveInspector(Component* component) {
		if (Transform* transform = dynamic_cast<Transform*>(component)) {
			Editor::Gui::TransformInspector inspector{ Editor::selectedGameObject };
		}
		if (MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(component)) {
			Engine::Editor::MaterialInspector::MaterialInspector(Editor::selectedGameObject);
		}
		if (RigidBody* rigidBody = dynamic_cast<RigidBody*>(component)) {
			Engine::Editor::RigidBodyInspector::RigidBodyInspector(rigidBody);
		}
		if (Collider* collider = dynamic_cast<Collider*>(component)) {
			Engine::Editor::ColliderInspector::ColliderInspector(collider);
		}
	}
}