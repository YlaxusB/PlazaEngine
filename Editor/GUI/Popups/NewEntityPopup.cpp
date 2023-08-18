#include "Engine/Core/PreCompiledHeaders.h"
#include "NewEntityPopup.h"

#include "Editor/DefaultAssets/DefaultAssets.h"
namespace Engine::Editor {
	GameObject* NewEntity(string name, GameObject* parent, shared_ptr<Mesh> mesh, bool instanced = true, bool addToScene = true) {
		GameObject* obj = new GameObject(name, parent, addToScene);
		obj->GetComponent<Transform>()->UpdateChildrenTransform();
		MeshRenderer* meshRenderer = new MeshRenderer();
		meshRenderer->instanced = true;
		meshRenderer->mesh = mesh;
		obj->AddComponent<MeshRenderer>(meshRenderer);
		Editor::selectedGameObject = obj;
		return obj;
	}
	void Popup::NewEntityPopup::Init(GameObject* obj, GameObject* parent) {
		if (!parent) {
			parent = Application->activeScene->mainSceneEntity;
		}
		if (ImGui::BeginMenu("New Entity"))
		{
			if (ImGui::MenuItem("Empty Entity"))
			{
				obj = new GameObject("New Entity", parent, true);
				obj->GetComponent<Transform>()->UpdateChildrenTransform();
				Editor::selectedGameObject = obj;
			}

			if (ImGui::MenuItem("Cube"))
			{
				NewEntity("Cube", parent, DefaultModels::Cube(), true, true);
			}

			if (ImGui::MenuItem("Sphere"))
			{
				NewEntity("Sphere", parent, DefaultModels::Sphere(), true, true);
			}

			if (ImGui::MenuItem("Plane"))
			{
				GameObject* obj = NewEntity("Plane", parent, DefaultModels::Plane(), true, true);
				obj->GetComponent<Transform>()->scale = glm::vec3(10.0f, 1.0f, 10.0f);
				obj->GetComponent<Transform>()->UpdateChildrenTransform();
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				NewEntity("Cylinder", parent, DefaultModels::Cylinder(), true, true);
			}

			ImGui::EndMenu();
		}
	}
}