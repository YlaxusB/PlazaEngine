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

		//std::unordered_map<std::variant<uint64_t, std::string>, Entity*> gameObjectsMap;

		Entity* mainSceneEntity = nullptr;
		uint64_t mainSceneEntityUuid;

		//std::unique_ptr<ECSManager> mECS;
		static inline int sComponentCounter;
		template <class T>
		int GetId()
		{
			static int sComponentId = sComponentCounter++;
			return sComponentId;
		}


		std::map<uint64_t, Animation*> mPlayingAnimations = std::map<uint64_t, Animation*>();

		std::unordered_map<uint64_t, Entity> entities;
		std::vector<ComponentPool> mComponentPools = std::vector<ComponentPool>();

		std::unordered_map<uint64_t, RenderGroup> renderGroups;
		std::unordered_map<std::pair<uint64_t, std::vector<uint64_t>>, uint64_t, PairHash> renderGroupsFindMap;
		std::unordered_map<uint64_t, uint64_t> renderGroupsFindMapWithMeshUuid;
		std::unordered_map<std::vector<uint64_t>, uint64_t, VectorHash, VectorEqual<uint64_t>> renderGroupsFindMapWithMaterialUuid;

		std::unordered_map<std::string, std::unordered_set<uint64_t>> entitiesNames;

		Entity* NewEntity()
		{
			uint64_t uuid = Plaza::UUID::NewUUID();
			entities.emplace(uuid, Entity("NewEntity", nullptr, uuid));
			return &entities.at(uuid);
		}

		Entity* GetEntity(uint64_t id) {
			if (entities.find(id) == entities.end())
				return nullptr;
			return &entities.at(id);
		}

		template<typename T>
		bool HasComponent(uint64_t id) {
			return this->GetEntity(id)->mComponentMask.test(GetId<T>());
		}

		template<typename T>
		T* AddComponent(uint64_t id) {
			int componentId = GetId<T>();
			if (mComponentPools.size() < componentId)
				mComponentPools.resize(componentId + 1, nullptr);
			if (mComponentPools[componentId] == nullptr)
				mComponentPools[componentId] = new ComponentPool(sizeof(T));

			T* component = new (mComponentPools[componentId].Get(id)) T();
			entities[id].mComponentMask.set(componentId);
			return component;
		}

		template<typename T>
		T* GetComponent(uint64_t id) {
			int componentId = GetId<T>();
			if (!entities[id].mComponentMask.test(componentId))
				return nullptr;
			return static_cast<T*>(mComponentPools[componentId].Get(id));
		}

		template<typename T>
		void RemoveComponent(uint64_t id) {
			
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
			mainSceneEntity = new Entity("Scene");
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
}
#endif //PLAZA_SCENE_H