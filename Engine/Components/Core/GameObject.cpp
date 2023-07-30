#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/GameObject.h"

GameObject::GameObject(std::string objName, GameObject* parent) {
	this->transform = this->AddComponent<Transform>(new Transform());
	transform->gameObject = this;
	//transform->gameObject = this;
	//this->AddComponent<Transform>(this->transform);
	UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	name = objName;
	id = Application->actScn->gameObjects.size() > 0 ? Application->actScn->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

	// Set the new parent
	this->parent = parent;
	if (parent != nullptr) {
		parent->children.push_back(this);
	}
	// Add to the gameobjects list
	Application->actScn->gameObjects.push_back(this);
}

void GameObjectList::push_back(GameObject* obj) {
	Application->actScn->gameObjectsMap.emplace(obj->name, obj);
	std::vector<GameObject*>::push_back(obj);
}
GameObject* GameObjectList::find(std::string findName) {;
	auto it = Application->actScn->gameObjectsMap.find(findName);
	if (it != Application->actScn->gameObjectsMap.end()) {
		GameObject* obj = it->second;
		return obj;
	}
	return nullptr;
}


GameObjectList gameObjects;
std::unordered_map<std::string, GameObject*> gameObjectsMap;

std::vector<MeshRenderer*> meshRenderers;

GameObject* sceneObject = new GameObject("Scene Objects", nullptr);

MeshRenderer::MeshRenderer(Engine::Mesh* initialMesh) : mesh(initialMesh) {
	this->mesh = new Engine::Mesh(initialMesh->vertices, initialMesh->indices, initialMesh->material);
	Application->actScn->meshRenderers.emplace_back(this);
}