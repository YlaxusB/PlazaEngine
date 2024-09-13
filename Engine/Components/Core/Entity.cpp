#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Physics.h"


namespace Plaza {
	Entity::Entity() {
		// Default constructor implementation
		uuid = Plaza::UUID::NewUUID();
		this->AddComponent<Transform>(new Transform(), true);
		this->name = "Entity";
		Scene::GetActiveScene()->entities.emplace(this->uuid, *this);
		if (Scene::GetActiveScene()->mainSceneEntity)
			Scene::GetActiveScene()->mainSceneEntity->childrenUuid.push_back(this->uuid);
		Scene::GetActiveScene()->entitiesNames[this->name].insert(this->uuid);
	}
	Entity::Entity(std::string objName, Entity* parent, bool addToScene, uint64_t newUuid) {
		if (newUuid)
			uuid = newUuid;
		else
			uuid = Plaza::UUID::NewUUID();

		this->AddComponent<Transform>(new Transform(), addToScene);
		name = objName;
		// Set the new parent
		if (parent != nullptr && addToScene) {
			this->parentUuid = parent->uuid;
			Scene::GetActiveScene()->entities[parentUuid].childrenUuid.push_back(this->uuid);
			//parent->childrenUuid.push_back(this->uuid);
		}
		// Add to the gameobjects list
		if (addToScene) {
			Scene::GetActiveScene()->entities.emplace(this->uuid, *this);
			Scene::GetActiveScene()->entitiesNames[this->name].insert(this->uuid);
		}

	}

	std::unordered_map<std::string, void*> Entity::GetAllComponentsMaps() {
		return Scene::GetActiveScene()->componentsMap;
	}

	Entity& Entity::GetParent() {
		if (!parentUuid)
			return Scene::GetActiveScene()->entities.find(Scene::GetActiveScene()->mainSceneEntity->uuid)->second;
		auto parentIt = Scene::GetActiveScene()->entities.find(parentUuid);
		if (parentIt != Scene::GetActiveScene()->entities.end())
			return parentIt->second;
		else return *Scene::GetActiveScene()->mainSceneEntity;
	}

	template Transform* Entity::ReplaceComponent<Transform>(Transform* newComponent); // Replace 'Transform' with the actual type
	template MeshRenderer* Entity::ReplaceComponent<MeshRenderer>(MeshRenderer* newComponent);
	template <typename T>
	T* Entity::ReplaceComponent(T* newComponent) {
		T* existingComponent = GetComponent<T>();

		if (existingComponent) {
			// Copy over the UUID from the existing component to the new one
			newComponent->mUuid = existingComponent->mUuid;

			// Replace the component in the appropriate components list
			if constexpr (std::is_same_v<T, Transform>) {
				Scene::GetActiveScene()->transformComponents[newComponent->mUuid] = *newComponent;
			}
			else if constexpr (std::is_same_v<T, MeshRenderer>) {
				Scene::GetActiveScene()->meshRendererComponents[newComponent->mUuid] = *newComponent;
			}

			// Clean up and delete the old component
			if constexpr (std::is_base_of_v<Component, T>) {
				if (existingComponent->mUuid == this->uuid) {
					existingComponent->mUuid = UUID::NewUUID();
				}

				if constexpr (std::is_base_of_v<Transform, T> || std::is_base_of_v<MeshRenderer, T>) {
					if (existingComponent->mUuid != newComponent->mUuid) {
						if constexpr (std::is_base_of_v<Transform, T>) {
							Scene::GetActiveScene()->transformComponents.erase(existingComponent->mUuid);
						}
						else if constexpr (std::is_base_of_v<MeshRenderer, T>) {
							Scene::GetActiveScene()->meshRendererComponents.erase(existingComponent->mUuid);
						}
					}
				}
			}

			//delete existingComponent;
			return newComponent;
		}

		return nullptr; // Component to replace not found
	}

	void Entity::Rename(std::string newName) {
		// Erase the old name
		Scene::GetActiveScene()->entitiesNames.at(this->name).erase(Scene::GetActiveScene()->entitiesNames.at(this->name).find(this->uuid));
		if (Scene::GetActiveScene()->entitiesNames.at(this->name).size() <= 0) {
			Scene::GetActiveScene()->entitiesNames.erase(Scene::GetActiveScene()->entitiesNames.find(this->name));
		}

		// Emplace the new name
		Scene::GetActiveScene()->entitiesNames[newName].insert(this->uuid);
		this->name = newName;
	}

	Entity::~Entity() {
		//if (!Application::Get()->runningScene) {
		//	std::vector<uint64_t> children = this->childrenUuid;
		//	for (uint64_t child : children) {
		//		if (Scene::GetActiveScene()->entities.find(child) != Scene::GetActiveScene()->entities.end())
		//			Scene::GetActiveScene()->entities.at(child).~Entity();
		//	}
		//	if (this->HasComponent<Transform>())
		//		this->RemoveComponent<Transform>();
		//	if (this->HasComponent<MeshRenderer>())
		//		this->RemoveComponent<MeshRenderer>();
		//	if (this->HasComponent<Collider>())
		//		this->RemoveComponent<Collider>();
		//	if (this->HasComponent<RigidBody>())
		//		this->RemoveComponent<RigidBody>();
		//	if (this->HasComponent<Camera>())
		//		this->RemoveComponent<Camera>();
		//	if (this->HasComponent<CsScriptComponent>())
		//		this->RemoveComponent<CsScriptComponent>();
		//	if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
		//		this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
		//	if (this->HasComponent<AudioSource>())
		//		this->RemoveComponent<AudioSource>();
		//	if (this->HasComponent<AudioListener>())
		//		this->RemoveComponent<AudioListener>();
		//	if (this->HasComponent<Light>())
		//		this->RemoveComponent<Light>();
		//	if (this->HasComponent<AnimationComponent>())
		//		this->RemoveComponent<AnimationComponent>();
		//
		//	if (Editor::selectedGameObject && Editor::selectedGameObject->uuid == this->uuid)
		//		Editor::selectedGameObject = nullptr;
		//
		//	this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
		//	if (Scene::GetActiveScene()->entitiesNames.find(this->name) != Scene::GetActiveScene()->entitiesNames.end())
		//		Scene::GetActiveScene()->entitiesNames.erase(Scene::GetActiveScene()->entitiesNames.find(this->name));
		//
		//	if (Scene::GetActiveScene()->entities.find(this->uuid) != Scene::GetActiveScene()->entities.end())
		//		Scene::GetActiveScene()->entities.extract(this->uuid);
		//	//Scene::GetActiveScene()->entities.erase(std::remove(Scene::GetActiveScene()->entities.begin(), Scene::GetActiveScene()->entities.end(), this->uuid), Scene::GetActiveScene()->entities.end());
		//	//Scene::GetActiveScene()->entities.
		//	//Scene::GetActiveScene()->entities.erase(Scene::GetActiveScene()->entities.find(this->uuid));
		//}
	}

	void Entity::Delete() {
		std::vector<uint64_t> children = this->childrenUuid;
		for (uint64_t child : children) {
			if (Scene::GetActiveScene()->entities.find(child) != Scene::GetActiveScene()->entities.end())
				Scene::GetActiveScene()->entities.at(child).Delete();
		}
		if (this->HasComponent<Transform>())
			this->RemoveComponent<Transform>();
		if (this->HasComponent<MeshRenderer>())
			this->RemoveComponent<MeshRenderer>();
		if (this->HasComponent<Collider>()) {
			//this->GetComponent<Collider>()->RemoveActor();
			this->RemoveComponent<Collider>();
		}
		if (this->HasComponent<RigidBody>())
			this->RemoveComponent<RigidBody>();
		if (this->HasComponent<Camera>())
			this->RemoveComponent<Camera>();
		if (this->HasComponent<CsScriptComponent>()) {
			this->RemoveComponent<CsScriptComponent>();
			Scene::GetActiveScene()->csScriptComponents.erase(this->uuid);
		}
		if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
			this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
		if (this->HasComponent<AudioSource>())
			this->RemoveComponent<AudioSource>();
		if (this->HasComponent<AudioListener>())
			this->RemoveComponent<AudioListener>();
		if (this->HasComponent<Light>())
			this->RemoveComponent<Light>();
		if (this->HasComponent<AnimationComponent>())
			this->RemoveComponent<AnimationComponent>();

		/*
					MeshRenderer* meshRendererToInstantiate = entityToInstantiate->GetComponent<MeshRenderer>();
		MeshRenderer* newMeshRenderer = new MeshRenderer();
		newMeshRenderer->uuid = instantiatedEntity->uuid;
		newMeshRenderer->instanced = true;
		newMeshRenderer->mesh = shared_ptr<Mesh>(meshRendererToInstantiate->mesh);
		instantiatedEntity->AddComponent<MeshRenderer>(newMeshRenderer);
		*/

		if (Editor::selectedGameObject && Editor::selectedGameObject->uuid == this->uuid)
			Editor::selectedGameObject = nullptr;

		this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
		if (Scene::GetActiveScene()->entitiesNames.find(this->name) != Scene::GetActiveScene()->entitiesNames.end())
			Scene::GetActiveScene()->entitiesNames.erase(Scene::GetActiveScene()->entitiesNames.find(this->name));
		Scene::GetActiveScene()->entities.erase(Scene::GetActiveScene()->entities.find(this->uuid));

		//this->~Entity();
	}

	template Transform* Entity::AddComp<Transform>();
	template Camera* Entity::AddComp<Camera>();
	template MeshRenderer* Entity::AddComp<MeshRenderer>();
	template RigidBody* Entity::AddComp<RigidBody>();
	template Collider* Entity::AddComp<Collider>();
	template CharacterController* Entity::AddComp<CharacterController>();
	template CsScriptComponent* Entity::AddComp<CsScriptComponent>();
	template Plaza::Drawing::UI::TextRenderer* Entity::AddComp<Plaza::Drawing::UI::TextRenderer>();
	template AudioSource* Entity::AddComp<AudioSource>();
	template AudioListener* Entity::AddComp<AudioListener>();
	template Light* Entity::AddComp<Light>();
	template AnimationComponent* Entity::AddComp<AnimationComponent>();
	template<typename T>
	T* Entity::AddComp() {
		return new T();
	}

	void Entity::RemoveComponent(Component* component) {
		std::string className = typeid(*component).name();
		auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(Scene::GetActiveScene()->componentsMap[className]);
		if (components.find(this->uuid) != components.end()) {
			component->~Component();
			components.erase(components.find(this->uuid));
		}
	}

	uint64_t  Entity::Instantiate(uint64_t uuid) {
		PLAZA_PROFILE_SECTION("Instantiate");
		if (Scene::GetActiveScene()->entities.find(uuid) == Scene::GetActiveScene()->entities.end())
			return 0;
		Entity* entityToInstantiate = &Scene::GetActiveScene()->entities.find(uuid)->second;
		Entity* instantiatedEntity = new Entity(entityToInstantiate->name, &Scene::GetActiveScene()->entities.find(entityToInstantiate->parentUuid)->second);
		instantiatedEntity = &Scene::GetActiveScene()->entities.find(instantiatedEntity->uuid)->second;

		Transform* toInstantiateTransform = &Scene::GetActiveScene()->transformComponents.find(entityToInstantiate->uuid)->second;
		Transform* newTransform = &Scene::GetActiveScene()->transformComponents.find(instantiatedEntity->uuid)->second;
		newTransform->SetRelativePosition(toInstantiateTransform->relativePosition);
		newTransform->SetRelativeRotation(glm::eulerAngles(toInstantiateTransform->rotation));
		newTransform->scale = toInstantiateTransform->scale;

		auto meshRendererIt = Scene::GetActiveScene()->meshRendererComponents.find(entityToInstantiate->uuid);
		if (meshRendererIt != Scene::GetActiveScene()->meshRendererComponents.end()) {
			PLAZA_PROFILE_SECTION("MeshRenderer");
			//MeshRenderer* meshRendererToInstantiate = meshRendererIt->second;
			MeshRenderer* newMeshRenderer = new MeshRenderer();
			newMeshRenderer->mUuid = instantiatedEntity->uuid;
			newMeshRenderer->instanced = true;
			newMeshRenderer->mesh = meshRendererIt->second.mesh;//shared_ptr<Mesh>(meshRendererToInstantiate->mesh);
			newMeshRenderer->mMaterials = meshRendererIt->second.mMaterials;
			newMeshRenderer->renderGroup = meshRendererIt->second.renderGroup;
			instantiatedEntity->AddComponent<MeshRenderer>(newMeshRenderer);
		}

		auto colliderIt = Scene::GetActiveScene()->colliderComponents.find(entityToInstantiate->uuid);
		if (colliderIt != Scene::GetActiveScene()->colliderComponents.end()) {
			PLAZA_PROFILE_SECTION("Collider");
			Collider* newCollider = new Collider(colliderIt->second);
			newCollider->mShapes.clear();
			newCollider->mUuid = instantiatedEntity->uuid;
			newCollider->mRigidActor = nullptr;
			for (ColliderShape* shape : colliderIt->second.mShapes) {
				physx::PxGeometryHolder geometry = shape->mPxShape->getGeometry();
				physx::PxMaterial* shapeMaterial = Physics::defaultMaterial;
				// Create a new shape with the same properties
				physx::PxShape* newShape = Physics::m_physics->createShape(geometry.triangleMesh(), *shapeMaterial, false);
				newCollider->mShapes.push_back(new ColliderShape(newShape, shape->mEnum, shape->mMeshUuid));
			}
			instantiatedEntity->AddComponent<Collider>(newCollider);//->Init(nullptr);
			Scene::GetActiveScene()->colliderComponents.find(instantiatedEntity->uuid)->second.Init(nullptr);
		}

		auto rigidBodyIt = Scene::GetActiveScene()->rigidBodyComponents.find(entityToInstantiate->uuid);
		if (rigidBodyIt != Scene::GetActiveScene()->rigidBodyComponents.end()) {
			PLAZA_PROFILE_SECTION("RigidBody");
			RigidBody* newRigidBody = new RigidBody(rigidBodyIt->second);
			newRigidBody->mUuid = instantiatedEntity->uuid;
			instantiatedEntity->AddComponent<RigidBody>(newRigidBody);
			instantiatedEntity->GetComponent<RigidBody>()->Init();
		}

		auto csScriptComponentIt = Scene::GetActiveScene()->csScriptComponents.find(entityToInstantiate->uuid);
		if (csScriptComponentIt != Scene::GetActiveScene()->csScriptComponents.end()) {
			PLAZA_PROFILE_SECTION("CsScriptComponent");
			auto range = Scene::GetActiveScene()->csScriptComponents.equal_range(entityToInstantiate->uuid);
			vector<CsScriptComponent*> scriptsToAdd = vector<CsScriptComponent*>();
			for (auto it = range.first; it != range.second && it != Scene::GetActiveScene()->csScriptComponents.end(); ++it) {
				if (it->second.mUuid == entityToInstantiate->uuid) {
					CsScriptComponent* newScript = new CsScriptComponent(instantiatedEntity->uuid);
					newScript->Init(it->second.scriptPath);

					/* Get all fields from the entity to instantiate */
					std::map<std::string, Field*> fields = std::map<std::string, Field*>();
					//for (auto [key, value] : it->second.scriptClasses) { fields = FieldManager::GetFieldsValues(value->monoObject); };

					/* Apply all fields to the instantiated entity */
					uint64_t key = newScript->mUuid;
					//for (auto [scriptClassKey, scriptClassValue] : newScript->scriptClasses) {
					//	MonoClassField* monoField = NULL;
					//	void* iter = NULL;
					//	while ((monoField = mono_class_get_fields(mono_object_get_class(scriptClassValue->monoObject), &iter)) != NULL)
					//	{
					//		int type = mono_type_get_type(mono_field_get_type(monoField));
					//		if (type != MONO_TYPE_ARRAY && type != MONO_TYPE_CLASS) {
					//			if (fields.find(mono_field_get_name(monoField)) != fields.end())
					//				FieldManager::FieldSetValue(type, fields.at(mono_field_get_name(monoField))->mValue, scriptClassValue->monoObject, monoField, fields.at(mono_field_get_name(monoField)));
					//		}
					//		else if (type == MONO_TYPE_CLASS) {
					//			MonoObject* currentFieldMonoObject = nullptr;
					//			mono_field_get_value(scriptClassValue->monoObject, mono_class_get_field_from_name(mono_object_get_class(scriptClassValue->monoObject), mono_field_get_name(monoField)), &currentFieldMonoObject);
					//			if (fields.find(mono_field_get_name(monoField)) != fields.end())
					//				FieldManager::FieldSetValue(type, fields.at(mono_field_get_name(monoField))->mValue, scriptClassValue->monoObject, monoField, fields.at(mono_field_get_name(monoField)));
					//		}
					//	}
					//}

					Application::Get()->activeProject->scripts.at(it->second.scriptPath).entitiesUsingThisScript.emplace(instantiatedEntity->uuid);
					if (Application::Get()->runningScene) {
						for (auto& [key, value] : newScript->scriptClasses) {
							Mono::OnStart(value->monoObject);
						}
					}

					scriptsToAdd.push_back(newScript);
				}
			}
			for (CsScriptComponent* script : scriptsToAdd) {
				instantiatedEntity->AddComponent<CsScriptComponent>(script);
			}
		}

		auto audioSourceIt = Scene::GetActiveScene()->audioSourceComponents.find(entityToInstantiate->uuid);
		if (audioSourceIt != Scene::GetActiveScene()->audioSourceComponents.end()) {
			PLAZA_PROFILE_SECTION("AudioSource");
			AudioSource* newAudioSource = new AudioSource(audioSourceIt->second);
			newAudioSource->mUuid = instantiatedEntity->uuid;
			newAudioSource->SetGain(audioSourceIt->second.mGain);
			newAudioSource->SetPitch(audioSourceIt->second.mPitch);
			newAudioSource->SetLoop(audioSourceIt->second.mLoop);
			newAudioSource->SetSpatial(audioSourceIt->second.mSpatial);
			instantiatedEntity->AddComponent<AudioSource>(newAudioSource);
		}

		auto lightIt = Scene::GetActiveScene()->lightComponents.find(entityToInstantiate->uuid);
		if (lightIt != Scene::GetActiveScene()->lightComponents.end()) {
			PLAZA_PROFILE_SECTION("Light");
			Light* newLight = new Light(lightIt->second);
			newLight->mUuid = instantiatedEntity->uuid;
			instantiatedEntity->AddComponent<Light>(newLight);
		}

		auto characterControllerIt = Scene::GetActiveScene()->characterControllerComponents.find(entityToInstantiate->uuid);
		if (characterControllerIt != Scene::GetActiveScene()->characterControllerComponents.end()) {
			PLAZA_PROFILE_SECTION("Light");
			CharacterController* newCharacterController = new CharacterController(characterControllerIt->second);
			newCharacterController->mUuid = instantiatedEntity->uuid;
			instantiatedEntity->AddComponent<CharacterController>(newCharacterController);
		}


		/* Instantiate children */
		for (unsigned int i = 0; i < entityToInstantiate->childrenUuid.size(); i++) {
			uint64_t childUuid = entityToInstantiate->childrenUuid[i];
			uint64_t uuid = Instantiate(childUuid);
			if (uuid)
				Scene::GetActiveScene()->entities.at(uuid).ChangeParent(&Scene::GetActiveScene()->entities.at(uuid).GetParent(), instantiatedEntity);
			//instantiatedEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
		}

		//instantiatedEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
		//if (instantiatedEntity->HasComponent<Collider>()) {
		//	instantiatedEntity->GetComponent<Collider>()->UpdateShapeScale(instantiatedEntity->GetComponent<Transform>()->GetWorldScale());
		//}
		return instantiatedEntity->uuid;
	}
}