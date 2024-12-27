#ifndef PLAZA_SCENE_H
#define PLAZA_SCENE_H
#include <vector>
#include <variant>
#include <unordered_map>


//#include "Engine/Components/Core/Entity.h"


#include "Engine/Components/Rendering/Material.h"
#include "Engine/Core/RenderGroup.h"


#include <unordered_set>
#include "Engine/Core/Standards.h"
#include "Engine/Core/ComponentMapStructure.h"

#include "Engine/Components/Core/Camera.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Rendering/Light.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Physics/CharacterController.h"
#include "Engine/Components/Physics/Collider.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Scripting/CsScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Engine/Components/Drawing/UI/Gui.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Engine/Components/Audio/AudioListener.h"
#include "Engine/Components/Rendering/AnimationComponent.h"

#include "Engine/Core/Renderer/Vulkan/Renderer.h"
#include "Engine/Core/Engine.h"
#include "Engine/ECS/ECSManager.h"
//#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"

using namespace std;
namespace Plaza {
	struct VectorHash {
		template <class T>
		std::size_t operator()(const std::vector<T>& vector) const {
			std::size_t seed = vector.size();
			for (const auto& i : vector) {
				seed ^= std::hash<T>()(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};
	template <class T>
	struct VectorEqual {
		bool operator()(const std::vector<T>& lhs, const std::vector<T>& rhs) const {
			return lhs == rhs;
		}
	};

	struct Uint64Hash {
		size_t operator()(uint64_t key) const {
			uint64_t v = key * 3935559000370003845 + 2691343689449507681;

			v ^= v >> 21;
			v ^= v << 37;
			v ^= v >> 4;

			v *= 4768777513237032717;

			v ^= v << 20;
			v ^= v >> 41;
			v ^= v << 5;

			return v;
		}
	};

	struct RenderGroupKey
	{
		uint64_t first;
		uint64_t second;

		bool operator==(const RenderGroupKey& other) const
		{
			return (first == other.first
				&& second == other.second);
		}
	};

	struct PairHash {
		std::size_t operator () (const std::pair<uint64_t, std::vector<uint64_t>>& p) const {
			// Combine the hash values of the two uint64_t values
			std::size_t h1 = std::hash<uint64_t>{}(p.first);
			std::size_t h2 = p.second.size();
			for (const auto& i : p.second) {
				h2 ^= std::hash<uint64_t>()(i) + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
			}//VectorHash{}//std::hash<std::vector<uint64_t>>{}(p.second);
			return h1 ^ h2;
		}
	};

	class GameObjectList : public std::vector<std::unique_ptr<Entity>> {
	public:
		void push_back(std::unique_ptr<Entity> obj);
		Entity* find(std::string findName);
		Entity* find(uint64_t findUuid);
		~GameObjectList() = default;
	};

	class PLAZA_API Scene : public Asset {
	public:
		bool mIsDeleting = false;
		bool mRunning = false;

		static inline int sComponentCounter;
		template <class T>
		static int GetComponentId()
		{
			static int sComponentId = sComponentCounter++;
			return sComponentId;
		}

		//std::unordered_map<std::variant<uint64_t, std::string>, Entity*> gameObjectsMap;

		Entity* mainSceneEntity = nullptr;
		uint64_t mainSceneEntityUuid = 0;

		//std::unique_ptr<ECSManager> mECS;


		std::map<uint64_t, Animation*> mPlayingAnimations = std::map<uint64_t, Animation*>();

		std::unordered_map<uint64_t, std::optional<Entity>> entities;
		std::vector<ComponentPool*> mComponentPools = std::vector<ComponentPool*>();

		std::unordered_map<uint64_t, RenderGroup> renderGroups;
		std::unordered_map<std::pair<uint64_t, std::vector<uint64_t>>, uint64_t, PairHash> renderGroupsFindMap;
		std::unordered_map<uint64_t, uint64_t> renderGroupsFindMapWithMeshUuid;
		std::unordered_map<std::vector<uint64_t>, uint64_t, VectorHash, VectorEqual<uint64_t>> renderGroupsFindMapWithMaterialUuid;

		std::unordered_map<std::string, std::unordered_set<uint64_t>> entitiesNames;

		void SetParent(Entity* entity, Entity* parent) {
			if (entity->parentUuid) {
				auto& oldParentChildrenUuid = this->GetEntity(entity->parentUuid)->childrenUuid;
				oldParentChildrenUuid.erase(std::find(oldParentChildrenUuid.begin(), oldParentChildrenUuid.end(), entity->uuid));
			}

			if (parent) {
				entity->parentUuid = parent->uuid;
				parent->childrenUuid.push_back(entity->uuid);
			}
			else if (mainSceneEntityUuid && entity->uuid != mainSceneEntityUuid) {
				entity->parentUuid = mainSceneEntityUuid;
				this->GetEntity(mainSceneEntityUuid)->childrenUuid.push_back(entity->uuid);
			}

		}

		Entity* NewEntity(const std::string& name = "", Entity* parent = nullptr)
		{
			uint64_t uuid = entities.size() + 1;//Plaza::UUID::NewUUID();
			//if (entities.size() < uuid)
			//	entities.resize(uuid + 1);
			entities.emplace(uuid, Entity(name, nullptr, uuid));
			this->SetParent(&entities.at(uuid).value(), parent);
			this->AddComponent<TransformComponent>(uuid);
			return &entities.at(uuid).value();
		}

		inline Entity* GetEntity(uint64_t id) {
			if (!entities[id].has_value())
				return nullptr;
			return &entities.at(id).value();
		}

		template<typename T>
		bool HasComponent(uint64_t id) {
			if (!entities[id].has_value())
				return false;
			return this->GetEntity(id)->mComponentMask.test(GetComponentId<T>());
		}

		template<typename T>
		T* AddComponent(uint64_t id) {
			int componentId = GetComponentId<T>();
			if (mComponentPools.size() <= componentId)
				mComponentPools.resize(componentId + 1, nullptr);
			if (mComponentPools[componentId] == nullptr)
				mComponentPools[componentId] = new ComponentPool(sizeof(T), componentId);

			T* component = mComponentPools[componentId]->Add<T>(id);
			static_cast<Component*>(component)->mUuid = id;
			entities.at(id).value().mComponentMask.set(componentId);
			return component;
		}

		template<typename T>
		T* GetComponent(uint64_t id) {
			int componentId = GetComponentId<T>();
			bool hasValue = entities[id].has_value();
			if (!hasValue || !entities.at(id).value().mComponentMask.test(componentId))
				return nullptr;
			return static_cast<T*>(mComponentPools[componentId]->Get(id));
		}

		template<typename T>
		void RemoveComponent(uint64_t id) {

		}

		void RemoveEntity(uint64_t uuid) {

		}

		Entity* GetEntityByName(const std::string& name) {
			return nullptr;
		}

		RenderGroup* AddRenderGroup(Mesh* newMesh, std::vector<Material*> newMaterials, bool resizeBuffer = true) {
			if (!newMesh)
				return nullptr;
			bool foundNewRenderGroup = this->renderGroupsFindMap.find(std::pair<uint64_t, std::vector<uint64_t>>(newMesh->uuid, Material::GetMaterialsUuids(newMaterials))) != this->renderGroupsFindMap.end();
			if (foundNewRenderGroup) {
				uint64_t uuid = this->renderGroupsFindMap.at(std::pair<uint64_t, std::vector<uint64_t>>(newMesh->uuid, Material::GetMaterialsUuids(newMaterials)));
				RenderGroup* renderGroup = &this->renderGroups.at(uuid);
				return renderGroup;
			}
			else if (!foundNewRenderGroup)
			{
				RenderGroup newRenderGroup = RenderGroup(newMesh, newMaterials);
				newRenderGroup.mCount++;
				this->renderGroups.emplace(newRenderGroup.uuid, newRenderGroup);
				this->renderGroupsFindMap.emplace(std::pair<uint64_t, std::vector<uint64_t>>(newMesh->uuid, Material::GetMaterialsUuids(newMaterials)), newRenderGroup.uuid);
				this->renderGroupsFindMapWithMeshUuid.emplace(newRenderGroup.mesh->uuid, newRenderGroup.uuid);
				return &this->renderGroups.at(newRenderGroup.uuid);
			}
		}

		RenderGroup* AddRenderGroup(RenderGroup* renderGroup) {
			return AddRenderGroup(renderGroup->mesh, renderGroup->materials);
		}

		RenderGroup* GetRenderGroupWithUuids(uint64_t meshUuid, std::vector<uint64_t> materialsUuid) {
			const auto& renderGroupIt = renderGroupsFindMap.find(std::make_pair(meshUuid, materialsUuid));
			if (renderGroupIt != this->renderGroupsFindMap.end())
				return &renderGroups.at(renderGroupIt->second);
			return nullptr;
		}

		template <typename MapType>
		void EraseFoundMap(MapType& map, typename MapType::key_type value) {
			if (map.find(value) != map.end()) {
				map.erase(map.find(value));
			}
		}

		void RemoveRenderGroup(uint64_t uuid) {
			if (this->renderGroups.find(uuid) != this->renderGroups.end())
			{
				RenderGroup* renderGroup = &this->renderGroups.at(uuid);
				EraseFoundMap(this->renderGroups, renderGroup->uuid);
				if (renderGroup->mesh)
					EraseFoundMap(this->renderGroupsFindMapWithMeshUuid, renderGroup->mesh->uuid);
			}
		}

		Scene();
		Scene(Scene& other) = default;

		void InitMainEntity() {
			mainSceneEntity = this->NewEntity("Scene", nullptr);
			mainSceneEntityUuid = mainSceneEntity->uuid;
			mainSceneEntity->parentUuid = this->mainSceneEntity->uuid;
		};

		void RecalculateAddedComponents();

		void Copy(Scene* baseScene);
		static void Play(); // Starts the game
		static void Stop(); // Finishes the game
		static void Pause(); // Just pauses the game
		~Scene() {
			mIsDeleting = true;
		};

		template <class Archive>
		void serialize(Archive& archive) {
			//if (mainSceneEntity)
			//	mainSceneEntityUuid = mainSceneEntity->uuid;
			//
			//archive(PL_SER(mAssetUuid), PL_SER(mAssetName), PL_SER(mainSceneEntityUuid), PL_SER(entities), PL_SER(transformComponents), PL_SER(cameraComponents), PL_SER(meshRendererComponents),
			//	PL_SER(rigidBodyComponents), PL_SER(colliderComponents), PL_SER(csScriptComponents), PL_SER(UITextRendererComponents), PL_SER(audioSourceComponents),
			//	PL_SER(audioListenerComponents), PL_SER(lightComponents), PL_SER(characterControllerComponents), PL_SER(animationComponentComponents), PL_SER(guiComponents), PL_SER(cppScriptComponents));
			//
			//if (!mainSceneEntity)
			//	mainSceneEntity = &entities.at(mainSceneEntityUuid);
		}

		static void InitializeScenes();
		static Scene* GetEditorScene();
		static void SetEditorScene(std::shared_ptr<Scene> scene);
		static void ClearEditorScene();
		static Scene* GetRuntimeScene();
		static Scene* GetActiveScene();
		static void SetActiveScene(Scene* scene);
		static void NewRuntimeScene(Scene* baseScene);

		static void Terminate();
	private:
		static inline std::shared_ptr<Scene> sEditorScene = nullptr;
		static inline std::shared_ptr<Scene> sRuntimeScene = nullptr;
		static inline Scene* sActiveScene = nullptr;
	};

	template <typename... ComponentTypes>
	struct PLAZA_API SceneView {
		SceneView(Scene* scene) : mScene(scene) {
			int componentIds[] = { Scene::GetComponentId<ComponentTypes>()... };
			mComponentIds.assign(componentIds, componentIds + sizeof...(ComponentTypes));

			if (mComponentIds.empty()) {
				mAll = true;
			}
			else {
				// Determine the largest pool
				size_t smallestPoolSize = 0;
				for (int componentId : mComponentIds) {
					if (componentId < scene->mComponentPools.size() && scene->mComponentPools[componentId]) {
						ComponentPool* pool = scene->mComponentPools[componentId];
						if (pool->mSize < smallestPoolSize || smallestPoolSize == 0) {
							smallestPoolSize = pool->mSize;
							mSmallestPool = pool;
						}
					}
				}
			}
		}

		static bool IsEntityValid(uint64_t uuid) {
			return uuid != 0;
		}

		struct Iterator {
			Scene* mScene;
			ComponentPool* mSmallestPool;
			uint64_t mIndex;
			std::vector<int> mComponentIds;
			bool mAll;

			Iterator(Scene* scene, ComponentPool* smallestPool, uint64_t index, std::vector<int> componentIds, bool all)
				: mScene(scene), mSmallestPool(smallestPool), mIndex(index), mComponentIds(componentIds), mAll(all) { }

			uint64_t operator*() const {
				// Use the entity ID stored in the component pool
				uint64_t entityId = static_cast<Component*>(mSmallestPool->Get(mIndex))->mUuid;
				return entityId;
			}

			bool operator==(const Iterator& other) const {
				return mIndex == other.mIndex;
			}

			bool operator!=(const Iterator& other) const {
				return !(*this == other);
			}

			Iterator& operator++() {
				int entitiesSize = mScene->entities.size();
				do {
					mIndex++;
				} while (mIndex < entitiesSize && !ValidIndex());
				return *this;
			}

			bool ValidIndex() const {
				if (mAll) return true;

				// Retrieve the entity ID from the largest pool
				Component* entityId = static_cast<Component*>(mSmallestPool->Get(mIndex));
				if (!SceneView::IsEntityValid(entityId->mUuid)) return false;

				if (entityId->mUuid == 0 || entityId->mUuid == 14829735431805717965)
					return false;
				// Check if the entity has all the required components
				//const auto& entity = mScene->entities.at(entityId->mUuid);
				//for (int componentId : mComponentIds) {
				//	if (!entity.mComponentMask.test(componentId)) {
				//		return false;
				//	}
				//}
				return true;
			}
		};

		const Iterator begin() const {
			if (!mSmallestPool) return end();
			Iterator it(mScene, mSmallestPool, 0, mComponentIds, mAll);
			int entitiesSize = mScene->entities.size();
			while (it.mIndex < entitiesSize && !it.ValidIndex()) {
				it.mIndex++;
			}
			return it;
		}

		const Iterator end() const {
			return Iterator(mScene, mSmallestPool, mSmallestPool ? mSmallestPool->mSize : 0, mComponentIds, mAll);
		}

	private:
		Scene* mScene = nullptr;
		std::vector<int> mComponentIds;
		ComponentPool* mSmallestPool = nullptr;
		bool mAll = false;
	};
}
#endif //PLAZA_SCENE_H