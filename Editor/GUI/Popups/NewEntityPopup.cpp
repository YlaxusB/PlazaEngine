#include "Engine/Core/PreCompiledHeaders.h"
#include "NewEntityPopup.h"

#include "Editor/DefaultAssets/DefaultAssets.h"
#include "Engine/Core/Physics.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
namespace Plaza::Editor {
	Entity* NewEntity(string name, Entity* parent, shared_ptr<Mesh> mesh, bool instanced = true, bool addToScene = true) {
		Entity* obj = new Entity(name, parent, addToScene);
		obj->changingName = true;
		Application->activeScene->entities.at(obj->uuid).changingName = true;
		Gui::Hierarchy::Item::firstFocus = true;
		obj->GetComponent<Transform>()->UpdateChildrenTransform();
		MeshRenderer* meshRenderer = new MeshRenderer();
		meshRenderer->instanced = true;
		meshRenderer->mesh = mesh;
		obj->AddComponent<MeshRenderer>(meshRenderer);
		Editor::selectedGameObject = obj;
		return obj;
	}
	void Popup::NewEntityPopup::Init(Entity* obj, Entity* parent) {
		if (!parent) {
			parent = Application->activeScene->mainSceneEntity;
		}
		if (ImGui::BeginMenu("New Entity"))
		{
			if (ImGui::MenuItem("Empty Entity"))
			{
				obj = new Entity("New Entity", parent, true);
				obj->GetComponent<Transform>()->UpdateChildrenTransform();
				Editor::selectedGameObject = obj;
			}

			if (ImGui::MenuItem("Cube"))
			{
				Entity* obj = NewEntity("Cube", parent, DefaultModels::Cube(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				Collider* collider = new Collider(obj->uuid);
				physx::PxBoxGeometry geometry(transform->scale.x / 2.1, transform->scale.y / 2.1, transform->scale.z / 2.1);
				collider->AddShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial));
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Sphere"))
			{
				Entity* obj = NewEntity("Sphere", parent, DefaultModels::Sphere(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				Collider* collider = new Collider(obj->uuid);
				physx::PxSphereGeometry geometry(1.0f);
				collider->AddShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial));
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Plane"))
			{
				Entity* obj = NewEntity("Plane", parent, DefaultModels::Plane(), true, true);
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