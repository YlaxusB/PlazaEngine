#include "Engine/Core/PreCompiledHeaders.h"
#include "Mono.h"
#include "Engine/Core/Input/Input.h"
#include "Engine/Core/Input/Cursor.h"

#include "Editor/Filewatcher.h"

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Physics/Collider.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Scripting/FieldManager.h"
#include "Engine/Core/Physics.h"

namespace Plaza {

	void GetComponentMap(uint64_t uuid, std::string name, Component* component) {
		if (name == typeid(Transform).name()) {
			Application->activeScene->transformComponents.emplace(uuid, *dynamic_cast<Transform*>(component));
		}
		else if (name == typeid(MeshRenderer).name()) {
			Application->activeScene->meshRendererComponents.emplace(uuid, *dynamic_cast<MeshRenderer*>(component));
		}
		else if (name == typeid(RigidBody).name()) {
			dynamic_cast<RigidBody*>(component)->Init();
			Application->activeScene->rigidBodyComponents.emplace(uuid, *dynamic_cast<RigidBody*>(component));
		}
		else if (name == typeid(Collider).name()) {
			Application->activeScene->colliderComponents.emplace(uuid, *dynamic_cast<Collider*>(component));
		}
		else if (name == typeid(Camera).name()) {
			Application->activeScene->cameraComponents.emplace(uuid, *dynamic_cast<Camera*>(component));
		}
		else if (name == typeid(Plaza::Drawing::UI::TextRenderer).name()) {
			Application->activeScene->UITextRendererComponents.emplace(uuid, *dynamic_cast<Plaza::Drawing::UI::TextRenderer*>(component));
		}
	}

	void RemoveComponentFromEntity(uint64_t uuid, std::string name, Component* component) {
		Entity& ent = Application->activeScene->entities.at(uuid);
		if (name == typeid(Transform).name()) {
			ent.RemoveComponent<Transform>();
		}
		else if (name == typeid(MeshRenderer).name()) {
			ent.RemoveComponent<MeshRenderer>();
		}
		else if (name == typeid(RigidBody).name()) {
			ent.RemoveComponent<RigidBody>();
		}
		else if (name == typeid(Collider).name()) {
			ent.RemoveComponent<Collider>();
		}
		else if (name == typeid(Camera).name()) {
			ent.RemoveComponent<Camera>();
		}
	}

#define PL_ADD_INTERNAL_CALL(name) mono_add_internal_call("Plaza.InternalCalls::" #name, (void*)name)

	enum Axis {
		X,
		Y,
		Z
	};

	static bool HasComponent(uint64_t uuid, MonoReflectionType* componentType) {
		if (Application->activeScene->entities.find(uuid) == Application->activeScene->entities.end())
			return false;
		MonoType* monoType = mono_reflection_type_get_type(componentType);
		return Mono::mEntityHasComponentFunctions.at(monoType)(*Application->activeScene->GetEntity(uuid));
	}

	static const std::string GetSubclassName(Component* component) {
		const char* className = typeid(*component).name();

		// Strip the namespace from the class name.
		std::string classNameWithoutNamespace = className;
		size_t namespaceEndPos = classNameWithoutNamespace.find_last_of('.');
		if (namespaceEndPos != std::string::npos) {
			classNameWithoutNamespace = classNameWithoutNamespace.substr(namespaceEndPos + 1);
		}
		return classNameWithoutNamespace;
	}

	static Component* CreateComponentByName(uint64_t uuid, MonoReflectionType* componentType) {
		if (!uuid)
			return nullptr;
		MonoType* monoType = mono_reflection_type_get_type(componentType);
		if (Mono::mEntityHasComponentFunctions.find(monoType) != Mono::mEntityHasComponentFunctions.end()) {
			return Mono::mEntityAddComponentFunctions[monoType](Application->activeScene->entities.at(uuid));
		}
		return nullptr; // Component type not found
	}

	static void AddComponent(uint64_t uuid, MonoReflectionType* componentType) {
		if (uuid) {
			auto* component = CreateComponentByName(uuid, componentType);
			component->uuid = uuid;
			GetComponentMap(uuid, GetSubclassName(component), component);
			//Application->activeScene->entities.at(uuid).AddComp<typeid(component).name()>();
		}
	}

	static void RemoveComponent(uint64_t uuid, MonoReflectionType* componentType) {
		auto* component = CreateComponentByName(uuid, componentType);
		component->uuid = uuid;
		RemoveComponentFromEntity(uuid, GetSubclassName(component), component);
		//Application->activeScene->entities.at(uuid).AddComp<typeid(component).name()>();
	}



	static bool HasScript(uint64_t uuid, MonoReflectionType* componentType) {
		MonoType* monoType = mono_reflection_type_get_type(componentType);
		return Mono::mEntityHasComponentFunctions.at(monoType)(*Application->activeScene->GetEntity(uuid));
	}

	static MonoObject* GetScript(uint64_t uuid) {
		auto range = Application->activeScene->csScriptComponents.equal_range(uuid);

		for (auto it = range.first; it != range.second; ++it) {
			for (auto [key, value] : it->second.scriptClasses) {
				return value->monoObject;
			}
		}
	}

	static uint64_t FindEntityByNameCall(MonoString* name) {
		char* nameCStr = mono_string_to_utf8(name);

		Entity* entity = Application->activeScene->GetEntityByName(nameCStr);
		mono_free(nameCStr);
		if (!entity)
			return 0;
		return entity->uuid;
	}

	static uint64_t Instantiate(uint64_t uuid) {
		uint64_t newUuid = Entity::Instantiate(uuid);
		Application->activeScene->transformComponents.find(newUuid)->second.UpdateSelfAndChildrenTransform();
		return newUuid;
	}

#pragma region Input

	static bool InputIsKeyDown(int keyCode) {
		if (Application->focusedMenu == "Scene") {
			return glfwGetKey(Application->Window->glfwWindow, keyCode) == GLFW_PRESS;
		}
	}

	static bool IsKeyReleased(int keyCode) {
		if (Application->focusedMenu == "Scene") {
			return glfwGetKey(Application->Window->glfwWindow, keyCode) == GLFW_RELEASE;
		}
	}

	static bool InputIsMouseDown(int button) {
		if (Application->focusedMenu == "Scene") {
			return glfwGetMouseButton(Application->Window->glfwWindow, button) == GLFW_PRESS;
		}
	}

	static void GetMouseDelta(glm::vec2* out) {
		*out = glm::vec2(Input::Cursor::deltaX, Input::Cursor::deltaY);
	}

	static void CursorHide(bool val) {
		Editor::Filewatcher::mMainThreadQueue.push_back([val]() {
			if (val && glfwGetInputMode(Application->Window->glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_HIDDEN) {
				glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else if (!val && glfwGetInputMode(Application->Window->glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL)
				glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Input::Cursor::show = !val;
			});

	}
#pragma endregion Input

#pragma region Entity
	static MonoString* EntityGetName(uint64_t uuid) {
		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end()) {
			return mono_string_new(Mono::mAppDomain, it->second.name.c_str());
		}
	}
	static void EntitySetName(uint64_t uuid, MonoString* name) {
		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end()) {
			it->second.name = mono_string_to_utf8(name);
		}
	}
	static uint64_t EntityGetParent(uint64_t uuid) {
		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end()) {
			return it->second.uuid;
		}
	}
	static void EntitySetParent(uint64_t uuid, uint64_t parentUuid) {
		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end()) {
			if (parentUuid == 0) {
				it->second.ChangeParent(it->second.GetParent(), Application->activeScene->entities.at(Application->activeScene->mainSceneEntity->uuid));
			}
			else {
				auto parentIt = Application->activeScene->entities.find(parentUuid);
				if (parentIt != Application->activeScene->entities.end())
					it->second.ChangeParent(it->second.GetParent(), parentIt->second);
			}
		}
	}

	static vector<uint64_t> EntityGetChildren(uint64_t uuid) {

		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end())
			return Application->activeScene->entities.at(uuid).childrenUuid;
	}

	static void EntityDelete(uint64_t uuid) {
		if (uuid) {
			Editor::Filewatcher::AddToMainThread([uuid]() {

				auto it = Application->activeScene->entities.find(uuid); // Find the iterator for the key
				if (it != Application->activeScene->entities.end()) {
					Application->activeScene->entities.at(uuid).Delete();
				}
				});
		}
	}
#pragma endregion Entity

#pragma region Components

#pragma region Transform Component
	static void SetPosition(uint64_t uuid, glm::vec3* vec3) {
		PLAZA_PROFILE_SECTION("Mono: Set Position");
		Application->activeScene->transformComponents.at(uuid).SetRelativePosition(*vec3);
	}
	static void GetPositionCall(uint64_t uuid, glm::vec3* out) {
		*out = Application->activeScene->transformComponents.at(uuid).relativePosition;
	}

	static void SetRotation(uint64_t uuid, glm::vec3* vec3) {
		Application->activeScene->transformComponents.at(uuid).SetRelativeRotation(glm::radians(*vec3));
	}
	static void GetRotationCall(uint64_t uuid, glm::vec3* out) {
		*out = glm::degrees(Application->activeScene->transformComponents.at(uuid).rotation);
	}

	static void SetScaleCall(uint64_t uuid, glm::vec3* vec3) {
		Application->activeScene->transformComponents.at(uuid).SetRelativeScale(*vec3);
	}
	static void GetScaleCall(uint64_t uuid, glm::vec3* out) {
		*out = Application->activeScene->transformComponents.at(uuid).scale;
	}
	/*
			glm::mat4 matrix = this->GetTransform();
		glm::vec3 currentPosition = glm::vec3(matrix[3]);
		// Extract the forward, left, and up vectors from the matrix
		glm::vec3 forwardVector = glm::normalize(glm::vec3(matrix[2]));
		glm::vec3 leftVector = glm::normalize(glm::cross(glm::vec3(matrix[1]), forwardVector));
		glm::vec3 upVector = glm::normalize(glm::vec3(matrix[1]));
	*/
	static void Transform_GetUpVector(uint64_t uuid, glm::vec3* out) {
		glm::mat4 matrix = Application->activeScene->transformComponents.at(uuid).GetTransform();
		*out = glm::normalize(glm::vec3(matrix[1]));
	}

	static void Transform_GetForwardVector(uint64_t uuid, glm::vec3* out) {
		*out = glm::normalize(glm::vec3(Application->activeScene->transformComponents.at(uuid).GetTransform()[2]));
	}

	static void Transform_GetLeftVector(uint64_t uuid, glm::vec3* out) {
		glm::mat4 matrix = Application->activeScene->transformComponents.at(uuid).GetTransform();
		glm::vec3 forwardVector = glm::normalize(glm::vec3(matrix[2]));
		*out = glm::normalize(glm::cross(glm::vec3(matrix[1]), forwardVector));
	}

	static void Transform_GetWorldMatrix(uint64_t uuid, float** out, int* size) {
		glm::mat4 matrix = Application->activeScene->transformComponents.find(uuid)->second.GetTransform();

		// Create a statically allocated array to store the matrix data
		float* data = new float[16]; // Assuming 4x4 matrix

		int index = 0;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				data[index] = matrix[i][j];
				++index;
			}
		}

		*size = 16; // Size of the float array
		*out = data;
	}

	static void MoveTowards(uint64_t uuid, glm::vec3 vector3) {
		auto transformIt = Application->activeScene->transformComponents.find(uuid);
		if (transformIt != Application->activeScene->transformComponents.end())
			transformIt->second.MoveTowards(vector3);

	}



#pragma endregion Transform Component

#pragma region Mesh Renderer Component
	static void MeshRenderer_GetVertices(uint64_t uuid, glm::vec3** out, int* size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			vector<glm::vec3> vertices = meshRendererIt->second.mesh.get()->vertices;
			*size = static_cast<int>(vertices.size());
			*out = vertices.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetVertices(uint64_t uuid, glm::vec3* vertices, int size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			shared_ptr<Mesh> oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh.get())
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh.get() && oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*Application->activeScene->meshes[newMesh->meshId].get() = *newMesh;
			}
			else {
				Application->activeScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
			}
			Application->activeScene->meshRendererComponents.at(uuid).mesh = Application->activeScene->meshes.at(newMesh->meshId);
			vector<glm::vec3>& meshVertices = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->vertices;
			meshVertices.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshVertices.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshVertices.push_back(vertices[i]);
			}

			Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			//RenderGroup* rend = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup.get();
			//Mesh* mes = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh.get();
			if (Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			//delete newMesh;
		}
	}

	static void MeshRenderer_GetIndices(uint64_t uuid, unsigned int** out, int* size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			vector<unsigned int> indices = meshRendererIt->second.mesh.get()->indices;
			*size = static_cast<int>(indices.size());
			*out = indices.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetIndices(uint64_t uuid, unsigned int* indices, int size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			shared_ptr<Mesh> oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh.get())
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*Application->activeScene->meshes[newMesh->meshId].get() = *newMesh;
			}
			else {
				Application->activeScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
			}
			Application->activeScene->meshRendererComponents.at(uuid).mesh = Application->activeScene->meshes.at(newMesh->meshId);
			vector<unsigned int>& meshIndices = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->indices;
			meshIndices.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshIndices.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshIndices.push_back(indices[i]);
			}

			Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			if (Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

	static void MeshRenderer_GetNormals(uint64_t uuid, glm::vec3** out, int* size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			vector<glm::vec3> vertices = meshRendererIt->second.mesh.get()->normals;
			*size = static_cast<int>(vertices.size());
			*out = vertices.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetNormals(uint64_t uuid, glm::vec3* normals, int size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			shared_ptr<Mesh> oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh.get())
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*Application->activeScene->meshes[newMesh->meshId].get() = *newMesh;
			}
			else {
				Application->activeScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
			}
			Application->activeScene->meshRendererComponents.at(uuid).mesh = Application->activeScene->meshes.at(newMesh->meshId);
			vector<glm::vec3>& meshNormals = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->normals;
			meshNormals.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshNormals.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshNormals.push_back(normals[i]);
			}
			Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			if (Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

	static void MeshRenderer_GetUvs(uint64_t uuid, glm::vec2** out, int* size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			vector<glm::vec2> uvs = meshRendererIt->second.mesh.get()->uvs;
			*size = static_cast<int>(uvs.size());
			*out = uvs.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetUvs(uint64_t uuid, glm::vec2* uvs, int size) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			shared_ptr<Mesh> oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh.get())
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*Application->activeScene->meshes[newMesh->meshId].get() = *newMesh;
			}
			else {
				Application->activeScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
			}
			Application->activeScene->meshRendererComponents.at(uuid).mesh = Application->activeScene->meshes.at(newMesh->meshId);
			vector<glm::vec2>& meshUvs = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->uvs;
			meshUvs.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshUvs.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshUvs.push_back(uvs[i]);
			}
			Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			if (Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

	static void MeshRenderer_SetMesh(uint64_t uuid, glm::vec3* vertices, int verticesSize, unsigned int* indices, int indicesSize, glm::vec3* normals, int normalsSize, glm::vec2* uvs, int uvsSize) {
		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			shared_ptr<Mesh> oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh.get())
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*Application->activeScene->meshes[newMesh->meshId].get() = *newMesh;
			}
			else {
				Application->activeScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
			}
			Application->activeScene->meshRendererComponents.at(uuid).mesh = Application->activeScene->meshes.at(newMesh->meshId);
			vector<glm::vec3>& meshVertices = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->vertices;
			meshVertices.clear();
			meshVertices.reserve(verticesSize);
			for (int i = 0; i < verticesSize; ++i) {
				meshVertices.push_back(vertices[i]);
			}
			vector<unsigned int>& meshIndices = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->indices;
			meshIndices.clear();
			meshIndices.reserve(indicesSize);
			for (int i = 0; i < indicesSize; ++i) {
				meshIndices.push_back(indices[i]);
			}
			vector<glm::vec3>& meshNormals = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->normals;
			meshNormals.clear();
			meshNormals.reserve(normalsSize);
			for (int i = 0; i < normalsSize; ++i) {
				meshNormals.push_back(normals[i]);
			}
			vector<glm::vec2>& meshUvs = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->uvs;
			meshUvs.clear();
			meshUvs.reserve(uvsSize);
			for (int i = 0; i < uvsSize; ++i) {
				meshUvs.push_back(uvs[i]);
			}


			Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			if (Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

#pragma endregion Mesh Renderer Component

#pragma region RigidBody
	static void RigidBody_LockAngular(uint64_t uuid, Axis axis, bool value) {
		auto it = Application->activeScene->rigidBodyComponents.find(uuid);
		if (it != Application->activeScene->rigidBodyComponents.end()) {
			physx::PxRigidDynamicLockFlag::Enum flag;
			if (axis == Axis::X)
				flag = physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
			else if (axis == Axis::Y)
				flag = physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
			else if (axis == Axis::Z)
				flag = physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
			it->second.SetRigidDynamicLockFlags(flag, value);
		}
	}

	struct Angular {
		bool X, Y, Z;
	};

	static Angular* RigidBody_IsAngularLocked(uint64_t uuid) {
		Angular* angular = new Angular();
		auto it = Application->activeScene->rigidBodyComponents.find(uuid);
		if (it != Application->activeScene->rigidBodyComponents.end()) {
			if (it->second.rigidDynamicLockFlags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X))
				angular->X = true;

			if (it->second.rigidDynamicLockFlags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y))
				angular->Y = true;

			if (it->second.rigidDynamicLockFlags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z))
				angular->Z = true;
		}
		return angular;
	}

	static void RigidBody_ApplyForce(uint64_t uuid, glm::vec3* vec3) {
		Application->activeScene->rigidBodyComponents.at(uuid).ApplyForce(*vec3);
	}

	static void RigidBody_AddForce(uint64_t uuid, glm::vec3* vec3, physx::PxForceMode::Enum mode, bool autowake) {
		if (Application->activeScene->HasComponent<RigidBody>(uuid))
			Application->activeScene->rigidBodyComponents.at(uuid).AddForce(*vec3, mode, autowake);
	}

	static void RigidBody_AddTorque(uint64_t uuid, glm::vec3* vec3, physx::PxForceMode::Enum mode, bool autowake) {
		if (Application->activeScene->HasComponent<RigidBody>(uuid))
			Application->activeScene->rigidBodyComponents.at(uuid).AddTorque(*vec3, mode, autowake);
	}
#pragma endregion RigidBody

#pragma region Collider
	static void Collider_AddShape(uint64_t uuid, ColliderShapeEnum shape) {
		auto it = Application->activeScene->colliderComponents.find(uuid);
		if (it != Application->activeScene->colliderComponents.end()) {
			if (shape == ColliderShapeEnum::CONVEX_MESH || shape == ColliderShapeEnum::MESH && Application->activeScene->HasComponent<MeshRenderer>(uuid)) {
				it->second.CreateShape(shape, &Application->activeScene->transformComponents.at(uuid), Application->activeScene->meshRendererComponents.at(uuid).mesh.get());
			}
			else
				it->second.CreateShape(shape, &Application->activeScene->transformComponents.at(uuid));
		}
	}
#pragma endregion Collider

#pragma region TextRenderer
	static string TextRenderer_GetText(uint64_t uuid) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			return Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mText;
		}
		return "";
	}
	static void TextRenderer_SetText(uint64_t uuid, MonoString* monoString) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			char* textCStr = mono_string_to_utf8(monoString);
			Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mText = textCStr;
			mono_free(textCStr);
		}
	}
	static void TextRenderer_GetPosition(uint64_t uuid, glm::vec2* out) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			Drawing::UI::TextRenderer* comp = Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid);
			*out = glm::vec2(comp->mPosX, comp->mPosY);
		}
	}
	static void TextRenderer_SetPosition(uint64_t uuid, glm::vec2* position) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			Drawing::UI::TextRenderer* comp = Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid);
			comp->mPosX = position->x;
			comp->mPosY = position->y;
		}
	}
	static void TextRenderer_SetFullText(uint64_t uuid, MonoString* monoString, float x, float y, float scale, glm::vec4 color) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			char* textCStr = mono_string_to_utf8(monoString);
			Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->SetFullText(textCStr, x, y, scale, color);
			mono_free(textCStr);
		}
	}

	static float TextRenderer_GetScale(uint64_t uuid) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			return Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mScale;
		}
	}
	static void TextRenderer_SetScale(uint64_t uuid, float scale) {
		if (Application->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			Application->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mScale = scale;
		}
	}
#pragma endregion TextRenderer

#pragma endregion Components

#pragma region Time
	static float Time_GetDeltaTime() {
		return Time::deltaTime;
	}
#pragma endregion Time

	void InternalCalls::Init() {
		//PL_ADD_INTERNAL_CALL(GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::FindEntityByNameCall", FindEntityByNameCall);
		mono_add_internal_call("Plaza.InternalCalls::Instantiate", Instantiate);


		mono_add_internal_call("Plaza.InternalCalls::InputIsKeyDown", InputIsKeyDown);
		mono_add_internal_call("Plaza.InternalCalls::IsKeyReleased", IsKeyReleased);
		mono_add_internal_call("Plaza.InternalCalls::InputIsMouseDown", InputIsMouseDown);
		mono_add_internal_call("Plaza.InternalCalls::GetMouseDelta", GetMouseDelta);
		mono_add_internal_call("Plaza.InternalCalls::CursorHide", CursorHide);

		mono_add_internal_call("Plaza.InternalCalls::EntityGetName", EntityGetName);
		mono_add_internal_call("Plaza.InternalCalls::EntitySetName", EntitySetName);
		mono_add_internal_call("Plaza.InternalCalls::EntityGetParent", EntityGetParent);
		mono_add_internal_call("Plaza.InternalCalls::EntitySetParent", EntitySetParent);
		mono_add_internal_call("Plaza.InternalCalls::EntityGetChildren", EntityGetChildren);
		mono_add_internal_call("Plaza.InternalCalls::EntityDelete", EntityDelete);
		mono_add_internal_call("Plaza.InternalCalls::HasComponent", HasComponent);
		mono_add_internal_call("Plaza.InternalCalls::AddComponent", AddComponent);
		mono_add_internal_call("Plaza.InternalCalls::RemoveComponent", RemoveComponent);
		mono_add_internal_call("Plaza.InternalCalls::HasScript", HasScript);
		mono_add_internal_call("Plaza.InternalCalls::GetScript", GetScript);



		mono_add_internal_call("Plaza.InternalCalls::GetPositionCall", GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::SetPosition", SetPosition);
		mono_add_internal_call("Plaza.InternalCalls::GetRotationCall", GetRotationCall);
		mono_add_internal_call("Plaza.InternalCalls::SetRotation", SetRotation);
		mono_add_internal_call("Plaza.InternalCalls::GetScaleCall", GetScaleCall);
		mono_add_internal_call("Plaza.InternalCalls::SetScaleCall", SetScaleCall);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetForwardVector", Transform_GetForwardVector);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetUpVector", Transform_GetUpVector);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetLeftVector", Transform_GetLeftVector);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetWorldMatrix", Transform_GetWorldMatrix);

		mono_add_internal_call("Plaza.InternalCalls::MoveTowards", MoveTowards);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetVertices", MeshRenderer_GetVertices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetVertices", MeshRenderer_SetVertices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetIndices", MeshRenderer_GetIndices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetIndices", MeshRenderer_SetIndices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetNormals", MeshRenderer_GetNormals);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetNormals", MeshRenderer_SetNormals);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetMesh", MeshRenderer_SetMesh);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetUvs", MeshRenderer_GetUvs);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetUvs", MeshRenderer_SetUvs);

		mono_add_internal_call("Plaza.InternalCalls::RigidBody_ApplyForce", RigidBody_ApplyForce);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_AddForce", RigidBody_AddForce);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_AddTorque", RigidBody_AddTorque);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_LockAngular", RigidBody_LockAngular);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_IsAngularLocked", RigidBody_IsAngularLocked);

		mono_add_internal_call("Plaza.InternalCalls::Collider_AddShape", Collider_AddShape);

		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_GetText", TextRenderer_GetText);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetText", TextRenderer_SetText);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_GetPosition", TextRenderer_GetPosition);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetPosition", TextRenderer_SetPosition);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_GetScale", TextRenderer_GetScale);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetScale", TextRenderer_SetScale);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetFullText", TextRenderer_SetFullText);


		mono_add_internal_call("Plaza.InternalCalls::Time_GetDeltaTime", Time_GetDeltaTime);


		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetUvs", MeshRenderer_GetUvs);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetUvs", MeshRenderer_SetUvs);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetIndices", MeshRenderer_GetIndices);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetIndices", MeshRenderer_SetIndices);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetNormals", MeshRenderer_GetNormals);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetNormals", MeshRenderer_SetNormals);
	}

}