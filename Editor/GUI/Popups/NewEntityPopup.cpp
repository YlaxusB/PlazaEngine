#include "Engine/Core/PreCompiledHeaders.h"
#include "NewEntityPopup.h"

#include "Editor/DefaultAssets/DefaultAssets.h"
#include "Engine/Core/Physics.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
namespace Plaza::Editor {
	Entity* NewEntity(string name, Entity* parent, Mesh* mesh, bool instanced = true, bool addToScene = true) {
		Entity* obj = new Entity(name, parent, addToScene);
		obj->changingName = true;
		Scene::GetActiveScene()->entities.at(obj->uuid).changingName = true;
		HierarchyWindow::Item::firstFocus = true;
		obj->GetComponent<Transform>()->UpdateChildrenTransform();
		MeshRenderer* meshRenderer = new MeshRenderer(mesh, { AssetsManager::GetDefaultMaterial() }, true);
		meshRenderer->instanced = true;
		//meshRenderer->mesh = new Mesh(*mesh);
		//meshRenderer->mMaterials.push_back(AssetsManager::GetDefaultMaterial());
		//RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->mMaterials);
		//meshRenderer->renderGroup = Scene::GetActiveScene()->AddRenderGroup(newRenderGroup);
		//meshRenderer->renderGroup->material = make_shared<Material>(*AssetsManager::GetDefaultMaterial());
		obj->AddComponent<MeshRenderer>(meshRenderer);
		Editor::selectedGameObject = obj;

		return obj;
	}
	void Popup::NewEntityPopup::Init(Entity* obj, Entity* parent) {
		if (!parent) {
			parent = Scene::GetActiveScene()->mainSceneEntity;
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
				collider->CreateShape(ColliderShape::ColliderShapeEnum::BOX, transform);
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Sphere"))
			{
				Entity* obj = NewEntity("Sphere", parent, DefaultModels::Sphere(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				Collider* collider = new Collider(obj->uuid);
				collider->CreateShape(ColliderShape::ColliderShapeEnum::SPHERE, transform);
				obj->AddComponent<Collider>(collider);
			}

			if (ImGui::MenuItem("Plane"))
			{
				Entity* obj = NewEntity("Plane", parent, DefaultModels::Plane(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				transform->scale = glm::vec3(10.0f, 0.05f, 10.0f);
				transform->UpdateChildrenTransform();
				Collider* collider = new Collider(obj->uuid);
				collider->CreateShape(ColliderShape::ColliderShapeEnum::PLANE, transform);
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