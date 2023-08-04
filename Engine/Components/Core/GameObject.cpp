#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/GameObject.h"

GameObject::GameObject(std::string objName, GameObject* parent) {
	this->transform = this->AddComponent<Transform>(new Transform());
	transform->gameObject = this;
	//transform->gameObject = this;
	//this->AddComponent<Transform>(this->transform);
	UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	name = objName;
	id = Application->activeScene->gameObjects.size() > 0 ? Application->activeScene->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE
	uuid = Engine::UUID::NewUUID();

	// Set the new parent
	this->parent = parent;
	if (parent != nullptr) {
		parent->children.push_back(this);
	}	// Add to the gameobjects list
	Application->activeScene->gameObjects.push_back(std::unique_ptr<GameObject>(this));
}

void GameObjectList::push_back(std::unique_ptr<GameObject> obj) {
	std::variant<uint64_t, string> variant; //= { obj.get()->uuid, obj.get()->name };
	variant = obj.get()->uuid;
	Application->activeScene->gameObjectsMap.emplace(variant, obj.get());
	//Application->activeScene->gameObjectsMap.emplace(obj.get()->name, obj.get());
	std::vector<std::unique_ptr<GameObject>>::push_back(std::move(obj));
}
GameObject* GameObjectList::find(std::string findName) {
	auto it = Application->activeScene->gameObjectsMap.find(findName);
	if (it != Application->activeScene->gameObjectsMap.end()) {
		GameObject* obj = it->second;
		return obj;
	}
	return nullptr;
}

GameObject* GameObjectList::find(uint64_t findUuid) {
	return Application->activeScene->gameObjectsMap[findUuid];
}

void GameObject::Delete() {
	uint64_t uuid = this->uuid;
	
	if (this->children.size() > 0) {
		for (GameObject* child : this->children) {
			child->parent = nullptr;
			child->Delete();
		}
	}
	if (this->parent) {
		auto it = std::find_if(this->parent->children.begin(), this->parent->children.end(), [uuid](GameObject* gameObject) {
			return gameObject->uuid == uuid;
			});

		if (it != this->parent->children.end()) {
			this->parent->children.erase(it);
		}

	}
	// Remove from the active scene game objects vector
	auto it = std::find_if(Application->activeScene->gameObjects.begin(), Application->activeScene->gameObjects.end(), [uuid](const std::unique_ptr<GameObject>& gameObject) {
		if (gameObject != nullptr)
			return gameObject->uuid == uuid;
		});

	if (it != Application->activeScene->gameObjects.end()) {
		Application->activeScene->gameObjectsMap.erase(uuid);
		Application->activeScene->gameObjects.erase(it);
		std::variant<uint64_t, std::string> uuidVariant = uuid;

	}

	//delete(this);
}

GameObjectList gameObjects;
std::unordered_map<std::string, GameObject*> gameObjectsMap;

std::vector<MeshRenderer*> meshRenderers;

GameObject* sceneObject = new GameObject("Scene Objects", nullptr);

MeshRenderer::MeshRenderer(Engine::Mesh initialMesh) {
	this->mesh = std::make_unique<Mesh>(initialMesh);
	Application->activeScene->meshRenderers.emplace_back(this);
}

MeshRenderer::~MeshRenderer() {
	Application->activeScene->RemoveMeshRenderer(this->gameObjectUUID);
}

/*

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(meshRenderers.begin(), meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->gameObjectUUID == uuid;
			});

		if (it != meshRenderers.end()) {
			meshRenderers.erase(it);
		}
	}
*/