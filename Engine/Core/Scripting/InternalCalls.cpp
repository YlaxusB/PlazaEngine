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
#include "Engine/Core/AssetsManager/AssetsManager.h"

namespace Plaza {

	void GetComponentMap(uint64_t uuid, std::string name, Component* component) {
		if (name == typeid(Transform).name()) {
			Application::Get()->activeScene->transformComponents.emplace(uuid, *dynamic_cast<Transform*>(component));
		}
		else if (name == typeid(MeshRenderer).name()) {
			Application::Get()->activeScene->meshRendererComponents.emplace(uuid, *dynamic_cast<MeshRenderer*>(component));
		}
		else if (name == typeid(RigidBody).name()) {
			dynamic_cast<RigidBody*>(component)->Init();
			Application::Get()->activeScene->rigidBodyComponents.emplace(uuid, *dynamic_cast<RigidBody*>(component));
		}
		else if (name == typeid(Collider).name()) {
			Application::Get()->activeScene->colliderComponents.emplace(uuid, *dynamic_cast<Collider*>(component));
		}
		else if (name == typeid(CharacterController).name()) {
			Application::Get()->activeScene->characterControllerComponents.emplace(uuid, *dynamic_cast<CharacterController*>(component));
		}
		else if (name == typeid(Camera).name()) {
			Application::Get()->activeScene->cameraComponents.emplace(uuid, *dynamic_cast<Camera*>(component));
		}
		else if (name == typeid(Plaza::Drawing::UI::TextRenderer).name()) {
			Application::Get()->activeScene->UITextRendererComponents.emplace(uuid, *dynamic_cast<Plaza::Drawing::UI::TextRenderer*>(component));
		}
		else if (name == typeid(Light).name()) {
			Application::Get()->activeScene->lightComponents.emplace(uuid, *dynamic_cast<Light*>(component));
		}
	}

	void RemoveComponentFromEntity(uint64_t uuid, std::string name, Component* component) {
		Entity& ent = Application::Get()->activeScene->entities.at(uuid);
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

	static uint64_t NewEntity() {
		Entity* newEntity = new Entity("New Entity", Application::Get()->activeScene->mainSceneEntity, true);
		newEntity->GetComponent<Transform>()->UpdateChildrenTransform();
		return newEntity->uuid;
	}

	enum Axis {
		X,
		Y,
		Z
	};

	static bool HasComponent(uint64_t uuid, MonoReflectionType* componentType) {
		if (Application::Get()->activeScene->entities.find(uuid) == Application::Get()->activeScene->entities.end())
			return false;
		MonoType* monoType = mono_reflection_type_get_type(componentType);
		return Mono::mEntityHasComponentFunctions.at(monoType)(*Application::Get()->activeScene->GetEntity(uuid));
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
			return Mono::mEntityAddComponentFunctions[monoType](Application::Get()->activeScene->entities.at(uuid));
		}
		return nullptr; // Component type not found
	}

	static void AddComponent(uint64_t uuid, MonoReflectionType* componentType) {
		if (uuid) {
			auto* component = CreateComponentByName(uuid, componentType);
			component->mUuid = uuid;
			GetComponentMap(uuid, GetSubclassName(component), component);
			//Application::Get()->activeScene->entities.at(uuid).AddComp<typeid(component).name()>();
		}
	}

	static void RemoveComponent(uint64_t uuid, MonoReflectionType* componentType) {
		auto* component = CreateComponentByName(uuid, componentType);
		component->mUuid = uuid;
		RemoveComponentFromEntity(uuid, GetSubclassName(component), component);
		//Application::Get()->activeScene->entities.at(uuid).AddComp<typeid(component).name()>();
	}



	static bool HasScript(uint64_t uuid, MonoReflectionType* componentType) {
		MonoType* monoType = mono_reflection_type_get_type(componentType);
		return Mono::mEntityHasComponentFunctions.at(monoType)(*Application::Get()->activeScene->GetEntity(uuid));
	}

	static MonoObject* GetScript(uint64_t uuid) {
		auto range = Application::Get()->activeScene->csScriptComponents.equal_range(uuid);

		for (auto it = range.first; it != range.second; ++it) {
			for (auto [key, value] : it->second.scriptClasses) {
				return value->monoObject;
			}
		}
	}

	static void AddScript(uint64_t uuid, MonoReflectionType* scriptType) {
		if (uuid) {
			auto* script = CreateComponentByName(uuid, scriptType);
			script->mUuid = uuid;
			GetComponentMap(uuid, GetSubclassName(script), script);
			//Application::Get()->activeScene->entities.at(uuid).AddComp<typeid(component).name()>();
		}
	}

	static uint64_t FindEntityByNameCall(MonoString* name) {
		char* nameCStr = mono_string_to_utf8(name);

		Entity* entity = Application::Get()->activeScene->GetEntityByName(nameCStr);
		mono_free(nameCStr);
		if (!entity)
			return 0;
		return entity->uuid;
	}

	static uint64_t Instantiate(uint64_t uuid) {
		uint64_t newUuid = Entity::Instantiate(uuid);
		if (newUuid)
			Application::Get()->activeScene->transformComponents.find(newUuid)->second.UpdateSelfAndChildrenTransform();
		return newUuid;
	}

#pragma region Input

	static bool InputIsAnyKeyPressed() {
		if (Application::Get()->focusedMenu == "Scene") {
			return Input::isAnyKeyPressed;
		}
	}

	static bool InputIsKeyDown(int keyCode) {
		if (Application::Get()->focusedMenu == "Scene") {
			return glfwGetKey(Application::Get()->mWindow->glfwWindow, keyCode) == GLFW_PRESS;
		}
	}

	static bool IsKeyReleased(int keyCode) {
		if (Application::Get()->focusedMenu == "Scene") {
			return glfwGetKey(Application::Get()->mWindow->glfwWindow, keyCode) == GLFW_RELEASE;
		}
	}

	static bool InputIsMouseDown(int button) {
		if (Application::Get()->focusedMenu == "Scene") {
			return glfwGetMouseButton(Application::Get()->mWindow->glfwWindow, button) == GLFW_PRESS;
		}
	}

	static void GetMouseDelta(glm::vec2* out) {
		*out = glm::vec2(Input::Cursor::deltaX, Input::Cursor::deltaY);
	}

	static void CursorHide(bool val) {
		Editor::Filewatcher::AddToMainThread([val]() {
			if (val && glfwGetInputMode(Application::Get()->mWindow->glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_HIDDEN) {
				glfwSetInputMode(Application::Get()->mWindow->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else if (!val && glfwGetInputMode(Application::Get()->mWindow->glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL)
				glfwSetInputMode(Application::Get()->mWindow->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			double currentX;
			double currentY;
			glfwGetCursorPos(Application::Get()->mWindow->glfwWindow, &currentX, &currentY);
			Input::Cursor::deltaX = 0;
			Input::Cursor::deltaY = 0;
			Input::Cursor::lastX = currentX;
			Input::Cursor::lastY = currentY;
			Input::Cursor::show = !val;
			});

	}
#pragma endregion Input

#pragma region Entity
	static MonoString* EntityGetName(uint64_t uuid) {
		auto it = Application::Get()->activeScene->entities.find(uuid);
		if (it != Application::Get()->activeScene->entities.end()) {
			return mono_string_new(Mono::mAppDomain, it->second.name.c_str());
		}
	}
	static void EntitySetName(uint64_t uuid, MonoString* name) {
		std::string nameStr = mono_string_to_utf8(name);
		Editor::Filewatcher::AddToMainThread([uuid, nameStr]() {
			auto it = Application::Get()->activeScene->entities.find(uuid);
			if (it != Application::Get()->activeScene->entities.end()) {
				it->second.Rename(nameStr);
			}
			});
	}
	static uint64_t EntityGetParent(uint64_t uuid) {
		auto it = Application::Get()->activeScene->entities.find(uuid);
		if (it != Application::Get()->activeScene->entities.end()) {
			return it->second.uuid;
		}
	}
	static void EntitySetParent(uint64_t uuid, uint64_t parentUuid) {
		auto it = Application::Get()->activeScene->entities.find(uuid);
		if (it != Application::Get()->activeScene->entities.end()) {
			if (parentUuid == 0) {
				it->second.ChangeParent(it->second.GetParent(), Application::Get()->activeScene->entities.at(Application::Get()->activeScene->mainSceneEntity->uuid));
			}
			else {
				auto parentIt = Application::Get()->activeScene->entities.find(parentUuid);
				if (parentIt != Application::Get()->activeScene->entities.end())
					it->second.ChangeParent(it->second.GetParent(), parentIt->second);
			}
		}
	}

	static vector<uint64_t> EntityGetChildren(uint64_t uuid) {

		auto it = Application::Get()->activeScene->entities.find(uuid);
		if (it != Application::Get()->activeScene->entities.end())
			return Application::Get()->activeScene->entities.at(uuid).childrenUuid;
	}

	static void EntityDelete(uint64_t uuid) {
		if (uuid) {
			Editor::Filewatcher::AddToMainThread([uuid]() {

				auto it = Application::Get()->activeScene->entities.find(uuid); // Find the iterator for the key
				if (it != Application::Get()->activeScene->entities.end()) {
					Application::Get()->activeScene->entities.at(uuid).Delete();
				}
				});
		}
	}
#pragma endregion Entity

#pragma region Physics
	class MyQueryFilterCallback : public physx::PxQueryFilterCallback {
	public:
		virtual physx::PxQueryHitType::Enum preFilter(
			const physx::PxFilterData& filterData,
			const physx::PxShape* shape,
			const physx::PxRigidActor* actor,
			physx::PxHitFlags& queryFlags) override
		{
			if (actor && actor->userData == mEntityToIgnore) {
				return physx::PxQueryHitType::eNONE;
			}
			return physx::PxQueryHitType::eBLOCK;
		}

		// Set the entity to ignore
		void setEntityToIgnore(void* entityToIgnore) {
			mEntityToIgnore = entityToIgnore;
		}

		physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override {
			return physx::PxQueryHitType::eTOUCH;
		}

	private:
		void* mEntityToIgnore = nullptr;
	};

	struct RaycastHit {
		uint64_t hitUuid;
		glm::vec3 point;
		glm::vec3 normal;
	};
	physx::PxVec3 glmToPx(glm::vec3 vector) {
		return physx::PxVec3(vector.x, vector.y, vector.z);
	}
	static void Physics_Raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance, RaycastHit* hit, uint64_t ignoredUuid) {
		MyQueryFilterCallback filterCallback{};
		filterCallback.setEntityToIgnore(reinterpret_cast<void*>(ignoredUuid));
		physx::PxQueryFilterData filterData(physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC);

		physx::PxRaycastBuffer hitPhysx;
		bool status = Physics::m_scene->raycast(glmToPx(origin), glmToPx(direction), maxDistance, hitPhysx, physx::PxHitFlag::eDEFAULT, filterData, &filterCallback);
		if (status) {
			hit->hitUuid = (uint64_t)hitPhysx.block.actor->userData;
			hit->point = glm::vec3(hitPhysx.block.position.x, hitPhysx.block.position.y, hitPhysx.block.position.z);
			hit->normal = glm::vec3(hitPhysx.block.normal.x, hitPhysx.block.normal.y, hitPhysx.block.normal.z);
		}
	}

	// This callback goes through anything except a specific collider uuid
	class SpecificQueryFilterCallback : public physx::PxQueryFilterCallback {
	public:
		virtual physx::PxQueryHitType::Enum preFilter(
			const physx::PxFilterData& filterData,
			const physx::PxShape* shape,
			const physx::PxRigidActor* actor,
			physx::PxHitFlags& queryFlags) override
		{
			if (actor && actor->userData != mEntityToCollide) {
				return physx::PxQueryHitType::eNONE;
			}
			return physx::PxQueryHitType::eBLOCK;
		}

		// Set the entity to ignore
		void setEntityToCollide(void* entityToCollide) {
			mEntityToCollide = entityToCollide;
		}

		physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override {
			return physx::PxQueryHitType::eTOUCH;
		}

	private:
		void* mEntityToCollide = nullptr;
	};

	static void Physics_RaycastSpecific(glm::vec3 origin, glm::vec3 direction, float maxDistance, RaycastHit* hit, uint64_t specificUuid) {
		SpecificQueryFilterCallback filterCallback{};
		filterCallback.setEntityToCollide(reinterpret_cast<void*>(specificUuid));
		physx::PxQueryFilterData filterData(physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC);

		physx::PxRaycastBuffer hitPhysx;
		bool status = Physics::m_scene->raycast(glmToPx(origin), glmToPx(direction), maxDistance, hitPhysx, physx::PxHitFlag::eDEFAULT, filterData, &filterCallback);
		if (status) {
			hit->hitUuid = (uint64_t)hitPhysx.block.actor->userData;
			hit->point = glm::vec3(hitPhysx.block.position.x, hitPhysx.block.position.y, hitPhysx.block.position.z);
			hit->normal = glm::vec3(hitPhysx.block.normal.x, hitPhysx.block.normal.y, hitPhysx.block.normal.z);
		}
	}
#pragma endregion Physics

#pragma region Components

#pragma region Transform Component
	static void SetPosition(uint64_t uuid, glm::vec3* vec3) {
		PLAZA_PROFILE_SECTION("Mono: Set Position");
		Application::Get()->activeScene->transformComponents.find(uuid)->second.SetRelativePosition(*vec3);
	}
	static void GetPositionCall(uint64_t uuid, glm::vec3* out) {
		*out = Application::Get()->activeScene->transformComponents.at(uuid).relativePosition;
	}

	static void SetRotation(uint64_t uuid, glm::vec3* vec3) {
		glm::vec3 radVector = glm::normalize(*vec3) * ((float)(glm::pi<float>() / 180.0f));
		float w = std::sqrt(1.0f - radVector.x * radVector.x - radVector.y * radVector.y - radVector.z * radVector.z);
		if (glm::isnan(w))
			w = 0.0f;
		radVector = *vec3 * ((float)(glm::pi<float>() / 180.0f));
		glm::quat rotationQuat = glm::quat(radVector);
		glm::quat quaternion = Application::Get()->activeScene->transformComponents.find(uuid)->second.rotation;

		Application::Get()->activeScene->transformComponents.find(uuid)->second.SetRelativeRotation(rotationQuat);
	}
	static void GetRotationCall(uint64_t uuid, glm::vec3* out) {
		*out = glm::degrees(glm::normalize(Application::Get()->activeScene->transformComponents.find(uuid)->second.GetWorldRotation()));
	}

	static void SetRotationQuaternion(uint64_t uuid, glm::vec4* quat) {
		Application::Get()->activeScene->transformComponents.find(uuid)->second.SetRelativeRotation(glm::quat(quat->w, quat->x, quat->y, quat->z));
	}
	static void GetRotationQuaternionCall(uint64_t uuid, glm::vec4* out) {
		glm::quat localQuat = Application::Get()->activeScene->transformComponents.find(uuid)->second.GetLocalQuaternion();
		*out = glm::vec4(localQuat.x, localQuat.y, localQuat.z, localQuat.w);
	}

	static void GetWorldRotationQuaternionCall(uint64_t uuid, glm::vec4* out) {
		glm::quat worldQuat = glm::quat(Application::Get()->activeScene->transformComponents.find(uuid)->second.GetWorldRotation());//Application::Get()->activeScene->transformComponents.find(uuid)->second.GetWorldQuaternion();
		*out = glm::vec4(worldQuat.x, worldQuat.y, worldQuat.z, worldQuat.w);
	}

	static void SetScaleCall(uint64_t uuid, glm::vec3* vec3) {
		Application::Get()->activeScene->transformComponents.find(uuid)->second.SetRelativeScale(*vec3);
	}
	static void GetScaleCall(uint64_t uuid, glm::vec3* out) {
		*out = Application::Get()->activeScene->transformComponents.find(uuid)->second.scale;
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
		glm::mat4 matrix = Application::Get()->activeScene->transformComponents.at(uuid).GetTransform();
		*out = glm::normalize(glm::vec3(matrix[1][0], matrix[1][1], matrix[1][2]));
	}

	static void Transform_GetForwardVector(uint64_t uuid, glm::vec3* out) {
		glm::mat4 matrix = Application::Get()->activeScene->transformComponents.at(uuid).GetTransform();
		*out = glm::normalize(glm::vec3(matrix[2][0], matrix[2][1], matrix[2][2]));
	}

	static void Transform_GetLeftVector(uint64_t uuid, glm::vec3* out) {
		glm::mat4 matrix = Application::Get()->activeScene->transformComponents.at(uuid).GetTransform();
		*out = -glm::normalize(glm::vec3(matrix[0][0], matrix[0][1], matrix[0][2]));
	}

	static void Transform_GetWorldMatrix(uint64_t uuid, float** out, int* size) {
		glm::mat4 matrix = Application::Get()->activeScene->transformComponents.find(uuid)->second.GetTransform();

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
		auto transformIt = Application::Get()->activeScene->transformComponents.find(uuid);
		if (transformIt != Application::Get()->activeScene->transformComponents.end())
			transformIt->second.MoveTowards(vector3);
	}

	static void MoveTowardsReturn(uint64_t uuid, glm::vec3 vector3, glm::vec3* outVector) {
		auto transformIt = Application::Get()->activeScene->transformComponents.find(uuid);
		if (transformIt != Application::Get()->activeScene->transformComponents.end())
			*outVector = transformIt->second.MoveTowardsReturn(vector3);
	}



#pragma endregion Transform Component

#pragma region Mesh Renderer Component
	static void MeshRenderer_SetMaterial(uint64_t uuid, uint64_t materialUuid) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end() && Application::Get()->activeScene->materials.find(materialUuid) != Application::Get()->activeScene->materials.end()) {
			meshRendererIt->second.ChangeMaterial(Application::Get()->activeScene->materials.find(materialUuid)->second.get());
			//meshRendererIt->second.ChangeMaterial(Application::Get()->activeScene->materials.find(materialUuid)->second.get());
			//meshRendererIt->second.material = Application::Get()->activeScene->materials.find(materialUuid)->second.get();
			//if (!meshRendererIt->second.renderGroup) {
			//	meshRendererIt->second.renderGroup = new RenderGroup(meshRendererIt->second.mesh, meshRendererIt->second.material);
			//	Application::Get()->activeScene->AddRenderGroup(meshRendererIt->second.renderGroup);
			//}

		}
	}
	static void MeshRenderer_GetVertices(uint64_t uuid, glm::vec3** out, int* size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			vector<glm::vec3> vertices = meshRendererIt->second.mesh->vertices;
			*size = static_cast<int>(vertices.size());
			*out = vertices.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetVertices(uint64_t uuid, glm::vec3* vertices, int size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			Mesh* oldMesh = (Mesh*)meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh)
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh && oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*AssetsManager::GetMesh(newMesh->meshId) = *newMesh;
			}
			else {
				AssetsManager::AddMesh(newMesh);
				//Application::Get()->activeScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
			}
			Application::Get()->activeScene->meshRendererComponents.at(uuid).mesh = AssetsManager::GetMesh(newMesh->meshId);
			vector<glm::vec3>& meshVertices = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->vertices;
			meshVertices.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshVertices.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshVertices.push_back(vertices[i]);
			}

			//    Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			Application::Get()->mRenderer->RestartMesh(Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh);
			//RenderGroup* rend = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup.get();
			//Mesh* mes = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh.get();
			if (Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = (Mesh*)(Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh);
			//delete newMesh;
		}
	}

	static void MeshRenderer_GetIndices(uint64_t uuid, unsigned int** out, int* size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			vector<unsigned int> indices = meshRendererIt->second.mesh->indices;
			*size = static_cast<int>(indices.size());
			*out = indices.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetIndices(uint64_t uuid, unsigned int* indices, int size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			Mesh* oldMesh = (Mesh*)meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh)
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*AssetsManager::GetMesh(newMesh->meshId) = *newMesh;
			}
			else {
				AssetsManager::AddMesh(newMesh);
			}
			Application::Get()->activeScene->meshRendererComponents.at(uuid).mesh = AssetsManager::GetMesh(newMesh->meshId);
			vector<unsigned int>& meshIndices = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->indices;
			meshIndices.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshIndices.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshIndices.push_back(indices[i]);
			}

			//    Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			Application::Get()->mRenderer->RestartMesh(Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh);
			if (Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

	static void MeshRenderer_GetNormals(uint64_t uuid, glm::vec3** out, int* size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			vector<glm::vec3> vertices = meshRendererIt->second.mesh->normals;
			*size = static_cast<int>(vertices.size());
			*out = vertices.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetNormals(uint64_t uuid, glm::vec3* normals, int size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			Mesh* oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh)
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*AssetsManager::GetMesh(newMesh->meshId) = *newMesh;
			}
			else {
				AssetsManager::AddMesh(newMesh);
			}
			Application::Get()->activeScene->meshRendererComponents.at(uuid).mesh = AssetsManager::GetMesh(newMesh->meshId);
			vector<glm::vec3>& meshNormals = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->normals;
			meshNormals.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshNormals.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshNormals.push_back(normals[i]);
			}
			//    Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			Application::Get()->mRenderer->RestartMesh(Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh);
			if (Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

	static void MeshRenderer_GetUvs(uint64_t uuid, glm::vec2** out, int* size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			vector<glm::vec2> uvs = meshRendererIt->second.mesh->uvs;
			*size = static_cast<int>(uvs.size());
			*out = uvs.data();
		}
		else {
			*size = 0;
			*out = nullptr;
		}
	}

	static void MeshRenderer_SetUvs(uint64_t uuid, glm::vec2* uvs, int size) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			Mesh* oldMesh = meshRendererIt->second.mesh;
			Mesh* newMesh;
			if (oldMesh)
				newMesh = new Mesh(*oldMesh);
			else
				newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*AssetsManager::GetMesh(newMesh->meshId) = *newMesh;
			}
			else {
				AssetsManager::AddMesh(newMesh);
			}
			Application::Get()->activeScene->meshRendererComponents.at(uuid).mesh = AssetsManager::GetMesh(newMesh->meshId);//Application::Get()->activeScene->meshes.at(newMesh->meshId).get();
			vector<glm::vec2>& meshUvs = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->uvs;
			meshUvs.clear();

			// Reserve space for the new vertices (optional but can improve performance)
			meshUvs.reserve(size);

			// Copy the provided vertices into the meshVertices vector
			for (int i = 0; i < size; ++i) {
				meshUvs.push_back(uvs[i]);
			}
			//    Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();
			Application::Get()->mRenderer->RestartMesh(Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh);
			if (Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup)
				Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
			delete newMesh;
		}
	}

	static void MeshRenderer_SetMesh(uint64_t uuid, glm::vec3* vertices, int verticesSize, unsigned int* indices, int indicesSize, glm::vec3* normals, int normalsSize, glm::vec2* uvs, int uvsSize) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			// Assuming you have a method to convert an array of glm::vec3 to your desired vector type.
			Mesh* oldMesh = (Mesh*)meshRendererIt->second.mesh;
			Mesh* newMesh;
			//if (oldMesh.get())
			//	newMesh = new Mesh(*oldMesh);
			//else
			newMesh = new Mesh();
			newMesh->uuid = Plaza::UUID::NewUUID();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			if (oldMesh && oldMesh->temporaryMesh) {
				newMesh->meshId = oldMesh->meshId;
				*AssetsManager::GetMesh(newMesh->meshId) = *newMesh;
			}
			else {
				AssetsManager::AddMesh(newMesh);
			}
			newMesh->vertices.clear();
			newMesh->vertices.reserve(verticesSize);
			newMesh->vertices.assign(vertices, vertices + verticesSize);

			newMesh->indices.clear();
			newMesh->indices.reserve(indicesSize);
			newMesh->indices.assign(indices, indices + indicesSize);

			newMesh->normals.clear();
			newMesh->normals.reserve(normalsSize);
			newMesh->normals.assign(normals, normals + normalsSize);

			newMesh->uvs.clear();
			newMesh->uvs.reserve(uvsSize);
			newMesh->uvs.assign(uvs, uvs + uvsSize);

			uint64_t oldMeshUuid = oldMesh->uuid;
			//Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh->Restart();

			Editor::Filewatcher::AddToMainThread([uuid, oldMeshUuid, meshRendererIt, newMesh]() {
				Application::Get()->activeScene->meshRendererComponents.find(uuid)->second.mesh = AssetsManager::GetMesh(newMesh->meshId);
				meshRendererIt->second.mesh = newMesh;

				Application::Get()->activeScene->meshRendererComponents.find(uuid)->second.mesh = Application::Get()->mRenderer->RestartMesh(Application::Get()->activeScene->meshRendererComponents.find(uuid)->second.mesh);

				if (oldMeshUuid == meshRendererIt->second.mesh->uuid && Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup) {
					Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->mesh = Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh;
				}
				else {
					RenderGroup* newRenderGroup = new RenderGroup(Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->mesh, Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup->materials);
					Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup = Application::Get()->activeScene->AddRenderGroup(newRenderGroup);
				}
				});

			//delete newMesh;
			//Application::Get()->activeScene->meshRendererComponents.find(uuid)->second.renderGroup->mesh = meshRendererIt->second.mesh;
		}
	}

	static float MeshRenderer_GetHeight(uint64_t uuid, float pixelX, float pixelY) {
		auto meshRendererIt = Application::Get()->activeScene->meshRendererComponents.find(uuid);
		if (meshRendererIt != Application::Get()->activeScene->meshRendererComponents.end()) {
			return meshRendererIt->second.GetHeight(pixelX, pixelY);
		}
	}

#pragma endregion Mesh Renderer Component

#pragma region RigidBody
	static void RigidBody_LockAngular(uint64_t uuid, Axis axis, bool value) {
		auto it = Application::Get()->activeScene->rigidBodyComponents.find(uuid);
		if (it != Application::Get()->activeScene->rigidBodyComponents.end()) {
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
		auto it = Application::Get()->activeScene->rigidBodyComponents.find(uuid);
		if (it != Application::Get()->activeScene->rigidBodyComponents.end()) {
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
		Application::Get()->activeScene->rigidBodyComponents.at(uuid).ApplyForce(*vec3);
	}

	static void RigidBody_AddForce(uint64_t uuid, glm::vec3* vec3, physx::PxForceMode::Enum mode, bool autowake) {
		if (Application::Get()->activeScene->HasComponent<RigidBody>(uuid))
			Application::Get()->activeScene->rigidBodyComponents.at(uuid).AddForce(*vec3, mode, autowake);
	}

	static void RigidBody_AddTorque(uint64_t uuid, glm::vec3* vec3, physx::PxForceMode::Enum mode, bool autowake) {
		if (Application::Get()->activeScene->HasComponent<RigidBody>(uuid))
			Application::Get()->activeScene->rigidBodyComponents.at(uuid).AddTorque(*vec3, mode, autowake);
	}

	static float RigidBody_GetDrag(uint64_t uuid, float drag) {
		if (Application::Get()->activeScene->HasComponent<RigidBody>(uuid))
			return Application::Get()->activeScene->rigidBodyComponents.at(uuid).GetDrag();
	}

	static void RigidBody_SetDrag(uint64_t uuid, float drag) {
		if (Application::Get()->activeScene->HasComponent<RigidBody>(uuid))
			Application::Get()->activeScene->rigidBodyComponents.at(uuid).SetDrag(drag);
	}

	static void RigidBody_GetVelocity(uint64_t uuid, glm::vec3* out) {
		if (Application::Get()->activeScene->HasComponent<RigidBody>(uuid)) {
			RigidBody* comp = Application::Get()->activeScene->GetComponent<RigidBody>(uuid);
			*out = comp->GetVelocity();
		}
	}

	static void RigidBody_SetVelocity(uint64_t uuid, glm::vec3* vec3) {
		if (Application::Get()->activeScene->HasComponent<RigidBody>(uuid))
			Application::Get()->activeScene->rigidBodyComponents.at(uuid).SetVelocity(*vec3);
	}
#pragma endregion RigidBody

#pragma region Collider
	static void Collider_AddShape(uint64_t uuid, ColliderShape::ColliderShapeEnum shape) {
		auto it = Application::Get()->activeScene->colliderComponents.find(uuid);
		if (it != Application::Get()->activeScene->colliderComponents.end()) {
			Editor::Filewatcher::AddToMainThread([it, uuid, shape]() {

				if ((shape == ColliderShape::ColliderShapeEnum::CONVEX_MESH || shape == ColliderShape::ColliderShapeEnum::MESH) && Application::Get()->activeScene->HasComponent<MeshRenderer>(uuid)) {
					it->second.CreateShape(shape, &Application::Get()->activeScene->transformComponents.at(uuid), Application::Get()->activeScene->meshRendererComponents.at(uuid).mesh);
				}
				else
					it->second.CreateShape(shape, &Application::Get()->activeScene->transformComponents.at(uuid));
				it->second.Init(nullptr);
				});
		}
	}

	static void Collider_AddShapeMeshCall(uint64_t uuid, ColliderShape::ColliderShapeEnum shape, glm::vec3* vertices, int verticesSize, unsigned int* indices, int indicesSize, glm::vec3* normals, int normalsSize, glm::vec2* uvs, int uvsSize) {
		auto it = Application::Get()->activeScene->colliderComponents.find(uuid);
		if (it != Application::Get()->activeScene->colliderComponents.end()) {
			Mesh* newMesh = new Mesh();
			newMesh->meshId = Plaza::UUID::NewUUID();
			newMesh->temporaryMesh = true;
			newMesh->vertices.clear();
			newMesh->vertices.reserve(verticesSize);
			newMesh->vertices.assign(vertices, vertices + verticesSize);

			newMesh->indices.clear();
			newMesh->indices.reserve(indicesSize);
			newMesh->indices.assign(indices, indices + indicesSize);

			newMesh->normals.clear();
			newMesh->normals.reserve(normalsSize);
			newMesh->normals.assign(normals, normals + normalsSize);

			newMesh->uvs.clear();
			newMesh->uvs.reserve(uvsSize);
			newMesh->uvs.assign(uvs, uvs + uvsSize);


			it->second.CreateShape(shape, &Application::Get()->activeScene->transformComponents.at(uuid), newMesh);
			Editor::Filewatcher::AddToMainThread([uuid]() {
				Application::Get()->activeScene->colliderComponents.find(uuid)->second.Init(nullptr);
				});
		}
	}
	static void Collider_AddShapeHeightFieldCall(uint64_t uuid, ColliderShape::ColliderShapeEnum shape, MonoArray* floatArray, int size) {
		auto it = Application::Get()->activeScene->colliderComponents.find(uuid);
		if (it != Application::Get()->activeScene->colliderComponents.end()) {
			float min = 0;
			if (shape == ColliderShape::ColliderShapeEnum::HEIGHT_FIELD && Application::Get()->activeScene->HasComponent<MeshRenderer>(uuid)) {
				float** data = new float* [size];
				for (int i = 0; i < size; ++i) {
					data[i] = new float[size];
					for (int j = 0; j < size; ++j) {
						mono_array_get(floatArray, int, i * size + j);
						data[i][j] = *reinterpret_cast<float*>(mono_array_addr_with_size(floatArray, sizeof(float), i * size + j));
						min = data[i][j] < min ? data[i][j] : min;
					}
				}
				it->second.AddHeightShape(data, size);//.CreateShape(shape, &Application::Get()->activeScene->transformComponents.at(uuid), Application::Get()->activeScene->meshRendererComponents.at(uuid).mesh.get());
			}
			it->second.Init(nullptr);
		}
	}

#pragma endregion Collider

#pragma region Character Controller
	static void CharacterController_MoveCall(uint64_t uuid, glm::vec3 position, float minimumDistance, bool followOrientation, float elapsedTime) {
		if (Application::Get()->activeScene->HasComponent<CharacterController>(uuid)) {
			Application::Get()->activeScene->GetComponent<CharacterController>(uuid)->Move(position, minimumDistance, followOrientation, elapsedTime);
		}
	}
#pragma endregion Character Controller

#pragma region TextRenderer
	static string TextRenderer_GetText(uint64_t uuid) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			return Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mText;
		}
		return "";
	}
	static void TextRenderer_SetText(uint64_t uuid, MonoString* monoString) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			char* textCStr = mono_string_to_utf8(monoString);
			Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mText = textCStr;
			mono_free(textCStr);
		}
	}
	static void TextRenderer_GetPosition(uint64_t uuid, glm::vec2* out) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			Drawing::UI::TextRenderer* comp = Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid);
			*out = glm::vec2(comp->mPosX, comp->mPosY);
		}
	}
	static void TextRenderer_SetPosition(uint64_t uuid, glm::vec2* position) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			Drawing::UI::TextRenderer* comp = Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid);
			comp->mPosX = position->x;
			comp->mPosY = position->y;
		}
	}
	static void TextRenderer_SetFullText(uint64_t uuid, MonoString* monoString, float x, float y, float scale, glm::vec4 color) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			char* textCStr = mono_string_to_utf8(monoString);
			Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->SetFullText(textCStr, x, y, scale, color);
			mono_free(textCStr);
		}
	}

	static float TextRenderer_GetScale(uint64_t uuid) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			return Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mScale;
		}
	}
	static void TextRenderer_SetScale(uint64_t uuid, float scale) {
		if (Application::Get()->activeScene->HasComponent<Drawing::UI::TextRenderer>(uuid)) {
			Application::Get()->activeScene->GetComponent<Drawing::UI::TextRenderer>(uuid)->mScale = scale;
		}
	}
#pragma endregion TextRenderer

#pragma region AudioSource
	static void AudioSource_Play(uint64_t uuid) {
		Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.Play();
	}
	static void AudioSource_Stop(uint64_t uuid) {
		Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.Stop();
	}
	static bool AudioSource_GetSpatial(uint64_t uuid) {
		return Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.mSpatial;
	}
	static void AudioSource_SetSpatial(uint64_t uuid, bool value) {
		Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.mSpatial = value;
	}
	static float AudioSource_GetVolume(uint64_t uuid) {
		return Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.mGain;
	}
	static void AudioSource_SetVolume(uint64_t uuid, bool value) {
		Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.mGain = value;
	}
	static float AudioSource_GetPitch(uint64_t uuid) {
		return Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.mPitch;
	}
	static void AudioSource_SetPitch(uint64_t uuid, bool value) {
		Application::Get()->activeScene->audioSourceComponents.find(uuid)->second.mPitch = value;
	}
#pragma endregion AudioSource

#pragma endregion Components

#pragma region Time
	static float Time_GetDeltaTime() {
		return Time::deltaTime;
	}
#pragma endregion Time

	void InternalCalls::Init() {
		//PL_ADD_INTERNAL_CALL(GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::FindEntityByNameCall", FindEntityByNameCall);
		mono_add_internal_call("Plaza.InternalCalls::NewEntity", NewEntity);
		mono_add_internal_call("Plaza.InternalCalls::Instantiate", Instantiate);


		mono_add_internal_call("Plaza.InternalCalls::InputIsAnyKeyPressed", InputIsAnyKeyPressed);
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
		mono_add_internal_call("Plaza.InternalCalls::AddScript", AddScript);

		mono_add_internal_call("Plaza.InternalCalls::Physics_Raycast", Physics_Raycast);
		mono_add_internal_call("Plaza.InternalCalls::Physics_RaycastSpecific", Physics_RaycastSpecific);
		//PL_ADD_INTERNAL_CALL("Physics_Raycast");

		mono_add_internal_call("Plaza.InternalCalls::GetPositionCall", GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::SetPosition", SetPosition);
		mono_add_internal_call("Plaza.InternalCalls::GetRotationCall", GetRotationCall);
		mono_add_internal_call("Plaza.InternalCalls::SetRotation", SetRotation);
		mono_add_internal_call("Plaza.InternalCalls::SetRotationQuaternion", SetRotationQuaternion);
		mono_add_internal_call("Plaza.InternalCalls::GetRotationQuaternionCall", GetRotationQuaternionCall);
		mono_add_internal_call("Plaza.InternalCalls::GetWorldRotationQuaternionCall", GetWorldRotationQuaternionCall);
		mono_add_internal_call("Plaza.InternalCalls::GetScaleCall", GetScaleCall);
		mono_add_internal_call("Plaza.InternalCalls::SetScaleCall", SetScaleCall);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetForwardVector", Transform_GetForwardVector);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetUpVector", Transform_GetUpVector);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetLeftVector", Transform_GetLeftVector);
		mono_add_internal_call("Plaza.InternalCalls::Transform_GetWorldMatrix", Transform_GetWorldMatrix);

		mono_add_internal_call("Plaza.InternalCalls::MoveTowards", MoveTowards);
		mono_add_internal_call("Plaza.InternalCalls::MoveTowardsReturn", MoveTowardsReturn);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetMaterial", MeshRenderer_SetMaterial);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetVertices", MeshRenderer_GetVertices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetVertices", MeshRenderer_SetVertices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetIndices", MeshRenderer_GetIndices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetIndices", MeshRenderer_SetIndices);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetNormals", MeshRenderer_GetNormals);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetNormals", MeshRenderer_SetNormals);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetMesh", MeshRenderer_SetMesh);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetUvs", MeshRenderer_GetUvs);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetUvs", MeshRenderer_SetUvs);
		mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetHeight", MeshRenderer_GetHeight);

		mono_add_internal_call("Plaza.InternalCalls::RigidBody_ApplyForce", RigidBody_ApplyForce);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_AddForce", RigidBody_AddForce);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_AddTorque", RigidBody_AddTorque);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_SetDrag", RigidBody_SetDrag);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_GetVelocity", RigidBody_GetVelocity);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_SetVelocity", RigidBody_SetVelocity);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_LockAngular", RigidBody_LockAngular);
		mono_add_internal_call("Plaza.InternalCalls::RigidBody_IsAngularLocked", RigidBody_IsAngularLocked);

		mono_add_internal_call("Plaza.InternalCalls::Collider_AddShape", Collider_AddShape);
		mono_add_internal_call("Plaza.InternalCalls::Collider_AddShapeMeshCall", Collider_AddShapeMeshCall);
		mono_add_internal_call("Plaza.InternalCalls::Collider_AddShapeHeightFieldCall", Collider_AddShapeHeightFieldCall);

		mono_add_internal_call("Plaza.InternalCalls::CharacterController_MoveCall", CharacterController_MoveCall);

		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_GetText", TextRenderer_GetText);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetText", TextRenderer_SetText);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_GetPosition", TextRenderer_GetPosition);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetPosition", TextRenderer_SetPosition);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_GetScale", TextRenderer_GetScale);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetScale", TextRenderer_SetScale);
		mono_add_internal_call("Plaza.InternalCalls::TextRenderer_SetFullText", TextRenderer_SetFullText);

		mono_add_internal_call("Plaza.InternalCalls::AudioSource_Play", AudioSource_Play);
		mono_add_internal_call("Plaza.InternalCalls::AudioSource_Stop", AudioSource_Stop);
		PL_ADD_INTERNAL_CALL("AudioSource_GetSpatial");
		PL_ADD_INTERNAL_CALL("AudioSource_SetSpatial");
		PL_ADD_INTERNAL_CALL("AudioSource_GetVolume");
		PL_ADD_INTERNAL_CALL("AudioSource_SetVolume");
		PL_ADD_INTERNAL_CALL("AudioSource_GetPitch");
		PL_ADD_INTERNAL_CALL("AudioSource_SetPitch");

		mono_add_internal_call("Plaza.InternalCalls::Time_GetDeltaTime", Time_GetDeltaTime);


		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetUvs", MeshRenderer_GetUvs);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetUvs", MeshRenderer_SetUvs);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetIndices", MeshRenderer_GetIndices);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetIndices", MeshRenderer_SetIndices);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_GetNormals", MeshRenderer_GetNormals);
		//mono_add_internal_call("Plaza.InternalCalls::MeshRenderer_SetNormals", MeshRenderer_SetNormals);
	}

}