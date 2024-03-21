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
		Application->activeScene->entities.emplace(this->uuid, *this);
		Application->activeScene->mainSceneEntity->childrenUuid.push_back(this->uuid);
		Application->activeScene->entitiesNames[this->name].insert(this->uuid);
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
			//Application->activeScene->entities[parentUuid].childrenUuid.push_back(this->uuid);
			parent->childrenUuid.push_back(this->uuid);
		}
		// Add to the gameobjects list
		if (addToScene) {
			Application->activeScene->entities.emplace(this->uuid, *this);
			Application->activeScene->entitiesNames[this->name].insert(this->uuid);
		}

	}

	std::unordered_map<std::string, void*> Entity::GetAllComponentsMaps() {
		return Application->activeScene->componentsMap;
	}

	Component* Entity::GetComponentByName(std::string className) {
		Component* component = nullptr;
		auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(Application->activeScene->componentsMap[className]);
		auto it = components.find(this->uuid);
		if (it != components.end()) {
			component = &(it->second);
		}
		return component;
	}

	Entity& Entity::GetParent() {
		if (!parentUuid)
			return Application->activeScene->entities.find(Application->activeScene->mainSceneEntity->uuid)->second;
		auto parentIt = Application->activeScene->entities.find(parentUuid);
		if (parentIt != Application->activeScene->entities.end())
			return parentIt->second;
		else return *Application->activeScene->mainSceneEntity;
	}

	template Transform* Entity::ReplaceComponent<Transform>(Transform* newComponent); // Replace 'Transform' with the actual type
	template MeshRenderer* Entity::ReplaceComponent<MeshRenderer>(MeshRenderer* newComponent);
	template <typename T>
	T* Entity::ReplaceComponent(T* newComponent) {
		T* existingComponent = GetComponent<T>();

		if (existingComponent) {
			// Copy over the UUID from the existing component to the new one
			newComponent->uuid = existingComponent->uuid;

			// Replace the component in the appropriate components list
			if constexpr (std::is_same_v<T, Transform>) {
				Application->activeScene->transformComponents[newComponent->uuid] = *newComponent;
			}
			else if constexpr (std::is_same_v<T, MeshRenderer>) {
				Application->activeScene->meshRendererComponents[newComponent->uuid] = *newComponent;
			}

			// Clean up and delete the old component
			if constexpr (std::is_base_of_v<Component, T>) {
				if (existingComponent->uuid == this->uuid) {
					existingComponent->uuid = UUID::NewUUID();
				}

				if constexpr (std::is_base_of_v<Transform, T> || std::is_base_of_v<MeshRenderer, T>) {
					if (existingComponent->uuid != newComponent->uuid) {
						if constexpr (std::is_base_of_v<Transform, T>) {
							Application->activeScene->transformComponents.erase(existingComponent->uuid);
						}
						else if constexpr (std::is_base_of_v<MeshRenderer, T>) {
							Application->activeScene->meshRendererComponents.erase(existingComponent->uuid);
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
		Application->activeScene->entitiesNames.at(this->name).erase(Application->activeScene->entitiesNames.at(this->name).find(this->uuid));
		if (Application->activeScene->entitiesNames.at(this->name).size() <= 0) {
			Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
		}

		// Emplace the new name
		Application->activeScene->entitiesNames[newName].insert(this->uuid);
		this->name = newName;
	}

	Entity::~Entity() {
		if (!Application->runningScene) {
			std::vector<uint64_t> children = this->childrenUuid;
			for (uint64_t child : children) {
				if (Application->activeScene->entities.find(child) != Application->activeScene->entities.end())
					Application->activeScene->entities.at(child).~Entity();
			}
			if (this->HasComponent<Transform>())
				this->RemoveComponent<Transform>();
			if (this->HasComponent<MeshRenderer>())
				this->RemoveComponent<MeshRenderer>();
			if (this->HasComponent<Collider>())
				this->RemoveComponent<Collider>();
			if (this->HasComponent<RigidBody>())
				this->RemoveComponent<RigidBody>();
			if (this->HasComponent<Camera>())
				this->RemoveComponent<Camera>();
			if (this->HasComponent<CsScriptComponent>())
				this->RemoveComponent<CsScriptComponent>();
			if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
				this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
			if (this->HasComponent<AudioSource>())
				this->RemoveComponent<AudioSource>();
			if (this->HasComponent<AudioListener>())
				this->RemoveComponent<AudioListener>();
			if (this->HasComponent<Light>())
				this->RemoveComponent<Light>();

			if (Editor::selectedGameObject && Editor::selectedGameObject->uuid == this->uuid)
				Editor::selectedGameObject = nullptr;

			this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
			if (Application->activeScene->entitiesNames.find(this->name) != Application->activeScene->entitiesNames.end())
				Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));

			if (Application->activeScene->entities.find(this->uuid) != Application->activeScene->entities.end())
				Application->activeScene->entities.extract(this->uuid);
			//Application->activeScene->entities.erase(std::remove(Application->activeScene->entities.begin(), Application->activeScene->entities.end(), this->uuid), Application->activeScene->entities.end());
			//Application->activeScene->entities.
			//Application->activeScene->entities.erase(Application->activeScene->entities.find(this->uuid));
		}
	}

	void Entity::Delete() {
		if (Application->runningScene) {
			std::vector<uint64_t> children = this->childrenUuid;
			for (uint64_t child : children) {
				if (Application->activeScene->entities.find(child) != Application->activeScene->entities.end())
					Application->activeScene->entities.at(child).Delete();
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
				Application->activeScene->csScriptComponents.erase(this->uuid);
			}
			if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
				this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
			if (this->HasComponent<AudioSource>())
				this->RemoveComponent<AudioSource>();
			if (this->HasComponent<AudioListener>())
				this->RemoveComponent<AudioListener>();
			if (this->HasComponent<Light>())
				this->RemoveComponent<Light>();

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
			if (Application->activeScene->entitiesNames.find(this->name) != Application->activeScene->entitiesNames.end())
				Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
			Application->activeScene->entities.erase(Application->activeScene->entities.find(this->uuid));
		}
		//this->~Entity();
	}

	template Transform* Entity::AddComp<Transform>();
	template Camera* Entity::AddComp<Camera>();
	template MeshRenderer* Entity::AddComp<MeshRenderer>();
	template RigidBody* Entity::AddComp<RigidBody>();
	template Collider* Entity::AddComp<Collider>();
	template CsScriptComponent* Entity::AddComp<CsScriptComponent>();
	template Plaza::Drawing::UI::TextRenderer* Entity::AddComp<Plaza::Drawing::UI::TextRenderer>();
	template AudioSource* Entity::AddComp<AudioSource>();
	template AudioListener* Entity::AddComp<AudioListener>();
	template<typename T>
	T* Entity::AddComp() {
		return new T();
	}

	void Entity::RemoveComponent(Component* component) {
		std::string className = typeid(*component).name();
		auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(Application->activeScene->componentsMap[className]);
		if (components.find(this->uuid) != components.end()) {
			component->~Component();
			components.erase(components.find(this->uuid));
		}
	}

	uint64_t  Entity::Instantiate(uint64_t uuid) {
		PLAZA_PROFILE_SECTION("Instantiate");
		if (Application->activeScene->entities.find(uuid) == Application->activeScene->entities.end())
			return 0;
		Entity* entityToInstantiate = &Application->activeScene->entities.find(uuid)->second;
		Entity* instantiatedEntity = new Entity(entityToInstantiate->name, &Application->activeScene->entities.find(entityToInstantiate->parentUuid)->second);
		instantiatedEntity = &Application->activeScene->entities.find(instantiatedEntity->uuid)->second;

		Transform* toInstantiateTransform = &Application->activeScene->transformComponents.find(entityToInstantiate->uuid)->second;
		Transform* newTransform = &Application->activeScene->transformComponents.find(instantiatedEntity->uuid)->second;
		newTransform->SetRelativePosition(toInstantiateTransform->relativePosition);
		newTransform->SetRelativeRotation(glm::eulerAngles(toInstantiateTransform->rotation));
		newTransform->scale = toInstantiateTransform->scale;

		auto meshRendererIt = Application->activeScene->meshRendererComponents.find(entityToInstantiate->uuid);
		if (meshRendererIt != Application->activeScene->meshRendererComponents.end()) {
			PLAZA_PROFILE_SECTION("MeshRenderer");
			//MeshRenderer* meshRendererToInstantiate = meshRendererIt->second;
			MeshRenderer* newMeshRenderer = new MeshRenderer();
			newMeshRenderer->uuid = instantiatedEntity->uuid;
			newMeshRenderer->instanced = true;
			newMeshRenderer->mesh = meshRendererIt->second.mesh;//shared_ptr<Mesh>(meshRendererToInstantiate->mesh);
			newMeshRenderer->material = meshRendererIt->second.material;
			newMeshRenderer->renderGroup = meshRendererIt->second.renderGroup;
			instantiatedEntity->AddComponent<MeshRenderer>(newMeshRenderer);
		}

		auto colliderIt = Application->activeScene->colliderComponents.find(entityToInstantiate->uuid);
		if (colliderIt != Application->activeScene->colliderComponents.end()) {
			PLAZA_PROFILE_SECTION("Collider");
			Collider* newCollider = new Collider(colliderIt->second);
			newCollider->mShapes.clear();
			newCollider->uuid = instantiatedEntity->uuid;
			newCollider->mRigidActor = nullptr;
			for (ColliderShape* shape : colliderIt->second.mShapes) {
				physx::PxGeometryHolder geometry = shape->mPxShape->getGeometry();
				physx::PxMaterial* shapeMaterial = Physics::defaultMaterial;
				// Create a new shape with the same properties
				physx::PxShape* newShape = Physics::m_physics->createShape(geometry.triangleMesh(), *shapeMaterial, false);
				newCollider->mShapes.push_back(new ColliderShape(newShape, shape->mEnum, shape->mMeshUuid));
			}
			instantiatedEntity->AddComponent<Collider>(newCollider);//->Init(nullptr);
			Application->activeScene->colliderComponents.find(instantiatedEntity->uuid)->second.Init(nullptr);
		}

		auto rigidBodyIt = Application->activeScene->rigidBodyComponents.find(entityToInstantiate->uuid);
		if (rigidBodyIt != Application->activeScene->rigidBodyComponents.end()) {
			PLAZA_PROFILE_SECTION("RigidBody");
			RigidBody* newRigidBody = new RigidBody(rigidBodyIt->second);
			newRigidBody->uuid = instantiatedEntity->uuid;
			instantiatedEntity->AddComponent<RigidBody>(newRigidBody);
			instantiatedEntity->GetComponent<RigidBody>()->Init();
		}

		auto csScriptComponentIt = Application->activeScene->csScriptComponents.find(entityToInstantiate->uuid);
		if (csScriptComponentIt != Application->activeScene->csScriptComponents.end()) {
			PLAZA_PROFILE_SECTION("CsScriptComponent");
			auto range = Application->activeScene->csScriptComponents.equal_range(entityToInstantiate->uuid);
			vector<CsScriptComponent*> scriptsToAdd = vector<CsScriptComponent*>();
			for (auto it = range.first; it != range.second && it != Application->activeScene->csScriptComponents.end(); ++it) {
				if (it->second.uuid == entityToInstantiate->uuid) {
					CsScriptComponent* newScript = new CsScriptComponent(instantiatedEntity->uuid);
					newScript->Init(it->second.scriptPath);

					/* Get all fields from the entity to instantiate */
					std::map<std::string, Field*> fields = std::map<std::string, Field*>();
					//for (auto [key, value] : it->second.scriptClasses) { fields = FieldManager::GetFieldsValues(value->monoObject); };

					/* Apply all fields to the instantiated entity */
					uint64_t key = newScript->uuid;
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

					Application->activeProject->scripts.at(it->second.scriptPath).entitiesUsingThisScript.emplace(instantiatedEntity->uuid);
					if (Application->runningScene) {
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

		auto audioSourceIt = Application->activeScene->audioSourceComponents.find(entityToInstantiate->uuid);
		if (audioSourceIt != Application->activeScene->audioSourceComponents.end()) {
			PLAZA_PROFILE_SECTION("AudioSource");
			AudioSource* newAudioSource = new AudioSource(audioSourceIt->second);
			newAudioSource->uuid = instantiatedEntity->uuid;
			newAudioSource->SetGain(audioSourceIt->second.mGain);
			newAudioSource->SetPitch(audioSourceIt->second.mPitch);
			newAudioSource->SetLoop(audioSourceIt->second.mLoop);
			newAudioSource->SetSpatial(audioSourceIt->second.mSpatial);
			instantiatedEntity->AddComponent<AudioSource>(newAudioSource);
		}

		auto lightIt = Application->activeScene->lightComponents.find(entityToInstantiate->uuid);
		if (lightIt != Application->activeScene->lightComponents.end()) {
			PLAZA_PROFILE_SECTION("Light");
			Light* newLight = new Light(lightIt->second);
			newLight->uuid = instantiatedEntity->uuid;
			instantiatedEntity->AddComponent<Light>(newLight);
		}


		/* Instantiate children */
		for (unsigned int i = 0; i < entityToInstantiate->childrenUuid.size(); i++) {
			uint64_t childUuid = entityToInstantiate->childrenUuid[i];
			uint64_t uuid = Instantiate(childUuid);
			if (uuid)
				Application->activeScene->entities.at(uuid).ChangeParent(&Application->activeScene->entities.at(uuid).GetParent(), instantiatedEntity);
			//instantiatedEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
		}

		//instantiatedEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
		//if (instantiatedEntity->HasComponent<Collider>()) {
		//	instantiatedEntity->GetComponent<Collider>()->UpdateShapeScale(instantiatedEntity->GetComponent<Transform>()->GetWorldScale());
		//}
		return instantiatedEntity->uuid;
	}
}