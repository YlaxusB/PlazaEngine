#include "Engine/Core/PreCompiledHeaders.h"
#include "NewEntityPopup.h"

#include "Editor/DefaultAssets/DefaultAssets.h"
#include "Engine/Core/Physics.h"
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
				GameObject* obj = NewEntity("Cube", parent, DefaultModels::Cube(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				Collider* collider = new Collider(obj->uuid);
				physx::PxBoxGeometry geometry(transform->scale.x / 2.1, transform->scale.y / 2.1, transform->scale.z / 2.1);
				collider->AddShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial));
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Sphere"))
			{
				GameObject* obj = NewEntity("Sphere", parent, DefaultModels::Sphere(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				Collider* collider = new Collider(obj->uuid);
				physx::PxSphereGeometry geometry(1.0f);
				collider->AddShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial));
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Plane"))
			{
				GameObject* obj = NewEntity("Plane", parent, DefaultModels::Plane(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				transform->scale = glm::vec3(10.0f, 0.05f, 10.0f);
				transform->UpdateChildrenTransform();
				Collider* collider = new Collider(obj->uuid);
				physx::PxBoxGeometry geometry(transform->scale.x / 2.1, transform->scale.y / 2.1, transform->scale.z / 2.1);
				collider->AddShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial));
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				NewEntity("Cylinder", parent, DefaultModels::Cylinder(), true, true);
			}

			ImGui::EndMenu();
		}
	}
}