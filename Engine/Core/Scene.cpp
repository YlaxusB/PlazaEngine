#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Engine/Components/Core/Entity.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Engine/Core/Input/Input.h"
#include "Engine/Core/Input/Cursor.h"
#include "Engine/Application/Serializer/Components/CsScriptComponentSerializer.h"

#include "Engine/Core/Physics.h"

namespace Plaza {
	Scene* Scene::Copy(Scene* newScene, Scene* copyScene) {
		newScene->mainSceneEntity = new Entity(*copyScene->mainSceneEntity);
		for (auto& gameObj : copyScene->entities) {
			Entity* newObj = new Entity(gameObj.second);
			const auto& it = copyScene->transformComponents.find(gameObj.second.uuid);
			if (it != copyScene->transformComponents.end()) {
				Transform* transform = new Transform(*gameObj.second.GetComponent<Transform>());
				transform->uuid = newObj->uuid;
				transform->scene = "Runtime Scene";
				transform->scene = "Runtime Scene";
				newObj->RemoveComponent<Transform>();
				newObj->AddComponent<Transform>(transform);
				newObj->GetComponent<Transform>()->UpdateChildrenTransform();
			}
			//newObj->ReplaceComponent<Transform>(newObj->GetComponent<Transform>(), newObj->transform);
			MeshRenderer* meshRenderer = gameObj.second.GetComponent<MeshRenderer>();
			if (gameObj.second.GetComponent<MeshRenderer>()) {
				MeshRenderer* newMeshRenderer = new MeshRenderer(*(gameObj.second.GetComponent<MeshRenderer>()->mesh));
				newMeshRenderer->uuid = newObj->uuid;
				//newObj->RemoveComponent<MeshRenderer>();
				newObj->AddComponent<MeshRenderer>(newMeshRenderer);
				//newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
				//newScene->meshRenderers.push_back(newMeshRenderer);
			}




			//newScene->gameObjectsMap.emplace(newObj->uuid, newObj.get());
			uint64_t uuid = newObj->uuid;
			newScene->entities.emplace(uuid, std::move(*newObj));
			//newScene->gameObjects.push_back(std::make_unique<Entity>(newObj.get()));
			//delete(newObj);
		}
		newScene->meshes = unordered_map<uint64_t, shared_ptr<Mesh>>(copyScene->meshes);
		newScene->transformComponents = std::unordered_map<uint64_t, Transform>(copyScene->transformComponents);
		newScene->cameraComponents = std::unordered_map<uint64_t, Camera>(copyScene->cameraComponents);
		newScene->meshRendererComponents = std::unordered_map<uint64_t, MeshRenderer>(copyScene->meshRendererComponents);
		newScene->csScriptComponents = std::unordered_multimap<uint64_t, CsScriptComponent>(copyScene->csScriptComponents);
		newScene->UITextRendererComponents = std::unordered_multimap<uint64_t, Plaza::Drawing::UI::TextRenderer>(copyScene->UITextRendererComponents);
		newScene->entitiesNames = std::unordered_map<std::string, std::unordered_set<uint64_t>>(copyScene->entitiesNames);


		for (auto& [key, value] : copyScene->colliderComponents) {
			// RigidBody* rig = &copyScene->rigidBodyComponents.at(key);
			if (copyScene->rigidBodyComponents.find(key) == copyScene->rigidBodyComponents.end()) {
				Collider* collider = new Collider(value);
				collider->Init(nullptr);
				newScene->colliderComponents.emplace(key, *collider);
			}
			else {
				newScene->colliderComponents.emplace(key, value);
			}
		}


		//newScene->colliderComponents = std::unordered_map<uint64_t, Collider>(copyScene->colliderComponents);
		//newScene->rigidBodyComponents = std::unordered_map<uint64_t, RigidBody>(copyScene->rigidBodyComponents);
		for (auto& gameObj : copyScene->gameObjects) {
			if (gameObj->parentUuid != 0 && copyScene->entities.find(gameObj->parentUuid) != copyScene->entities.end())
				Application->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->entities.find(gameObj->parentUuid)->second.uuid;
			else if (gameObj->parentUuid == 0)
				Application->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->mainSceneEntity->uuid;
		}

		for (auto& [key, value] : copyScene->rigidBodyComponents) {
			RigidBody* rigidBody = new RigidBody(value);
			rigidBody->uuid = key;
			newScene->rigidBodyComponents.emplace(key, *rigidBody);
		}

		return newScene;
	}

	Scene::Scene() {

		//componentMaps.emplace("class Plaza::Transform", transformComponents);
		//componentMaps.emplace("class Plaza::MeshRenderer", meshRendererComponents);
		//componentMaps.emplace("class Plaza::RigidBody", rigidBodyComponents);
		//componentMaps.emplace("class Plaza::Collider", colliderComponents);
		//componentMaps.emplace("class Plaza::Camera", cameraComponents);
		//componentMaps.emplace("class Plaza::CsScriptComponent", csScriptComponents);

	}

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(meshRenderers.begin(), meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->uuid == uuid;
			});

		if (it != meshRenderers.end()) {
			meshRenderers.erase(it);
		}
	}

	std::any GetFieldValue(MonoClassField* field, MonoObject* monoObject) {
		int type = mono_type_get_type(mono_field_get_type(field));
		if (type == MONO_TYPE_I4) {
			int val;
			mono_field_get_value(monoObject, field, &val);
			return val;
		}
		else if (type == MONO_TYPE_R4) {
			float val;
			mono_field_get_value(monoObject, field, &val);
			return val;
		}
		else if (type == MONO_TYPE_R8) {
			double val;
			mono_field_get_value(monoObject, field, &val);
			return val;
		}
		else if (type == MONO_TYPE_STRING) {
			MonoString* str = nullptr;
			mono_field_get_value(monoObject, field, &str);
			if (str != nullptr) {
				const char* val = mono_string_to_utf8(str);
				return mono_string_to_utf8(str);
			}
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val;
			mono_field_get_value(monoObject, field, &val);
			return val;
		}
		else if (type == MONO_TYPE_ENUM) {
			int val;
			mono_field_get_value(monoObject, field, &val);
			return val;
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val;
			mono_field_get_value(monoObject, field, &val);
			return val;
		}
		else if (type == MONO_TYPE_CLASS) {
			//bool val;
			//mono_field_get_value(monoObject, field, &val);
			//return val;
		}
		else {
			std::cout << "Unsupported " << type << "\n";
		}
		return std::any();
	}
	std::map<std::string, Field*> GetFieldsValues(MonoObject* monoObject) {
		std::map<std::string, Field*> fields = std::map<std::string, Field*>();
		MonoClassField* monoField = NULL;
		void* iter = NULL;
		std::unordered_map<std::string, uint32_t> classFields;
		while ((monoField = mono_class_get_fields(mono_object_get_class(monoObject), &iter)) != NULL)
		{
			Field* field = new Field();
			field->mName = mono_field_get_name(monoField);
			auto result = GetFieldValue(monoField, monoObject);
			field->mValue = result;
			field->mType = mono_type_get_type(mono_field_get_type(monoField));
			fields.emplace(field->mName, field);
		}
		return fields;
	}

	void FieldSetVal(int type, std::any& value, MonoObject* monoObject, MonoClassField* field) {
		if (type == MONO_TYPE_I4) {
			int val = std::any_cast<int>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_R4) {
			float val = std::any_cast<float>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_R8) {
			double val = std::any_cast<double>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_STRING) {
			//std::string val = std::any_cast<std::string>(value);
			//mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_U8) {
			uint64_t val = std::any_cast<uint64_t>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_ENUM) {
			int val = std::any_cast<int>(value);
			mono_field_set_value(monoObject, field, &val);
		}
		else if (type == MONO_TYPE_BOOLEAN) {
			bool val = std::any_cast<bool>(value);
			mono_field_set_value(monoObject, field, &val);
		}
	}

	void Scene::Play() {
		/* Get fields values */
		std::map<uint64_t, std::map<std::string, Field*>> allFields = std::map<uint64_t, std::map<std::string, Field*>>();
		for (auto [key, value] : Application->activeScene->csScriptComponents) {
			for (auto [scriptClassKey, scriptClassValue] : value.scriptClasses) {
				allFields.emplace(key, GetFieldsValues(scriptClassValue->monoObject));
			}
		}


		// Create a new empty Scene, change active scene to runtime, copy the contents of editor scene into runtime scene and update the selected object scene

		/* Stop mono */
			/* Load the new domain */
		mono_set_assemblies_path("lib/mono");
		//mono_set_assemblies_path((Application->editorPath + "/lib/mono").c_str());
		if (Mono::mMonoRootDomain == nullptr)
			Mono::mMonoRootDomain = mono_jit_init("MyScriptRuntime");
		if (Mono::mMonoRootDomain == nullptr)
		{
			// Maybe log some error here
			return;
		}
		// Create an App Domain
		char appDomainName[] = "PlazaAppDomain";
		MonoDomain* newDomain = mono_domain_create_appdomain(appDomainName, nullptr);
		mono_domain_set(newDomain, true);
		Mono::ReloadAppDomain();
		Mono::mAppDomain = newDomain;

		mono_domain_set(Mono::mAppDomain, true);
		/* Copy the script dll */
		std::string dllPath = (Application->projectPath + "\\Binaries\\" + std::filesystem::path{ Application->activeProject->name }.stem().string() + ".dll");
		std::string newPath = (Application->projectPath + "\\Binaries\\" + std::filesystem::path{ Application->activeProject->name }.stem().string() + "copy.dll");
		std::filesystem::copy_file(dllPath, newPath, filesystem::copy_options::overwrite_existing);

		Application->runtimeScene = new Scene();
		Application->copyingScene = true;
		Application->runtimeScene = Scene::Copy(Application->runtimeScene, Application->editorScene);
		Application->activeScene = Application->runtimeScene;
		Application->copyingScene = false;
		Application->runningScene = true;
#ifndef GAME_REL
		ImGui::SetWindowFocus("Scene");
#endif
		int width, height;
		glfwGetWindowSize(Application->Window->glfwWindow, &width, &height);
		glfwSetCursorPos(Application->Window->glfwWindow, width / 2, height / 2);
		Input::Cursor::lastX = 0;
		Input::Cursor::lastY = 0;
		Input::Cursor::deltaX = 0;
		Input::Cursor::deltaY = 0;
		// Init Rigid Bodies
		for (auto& [key, value] : Application->activeScene->rigidBodyComponents) {
			value.Init();
		}




		Mono::OnStartAll();

		/* Reapply all scripts fields */
		for (auto [key, value] : Application->activeScene->csScriptComponents) {
			for (auto [scriptClassKey, scriptClassValue] : value.scriptClasses) {
				MonoClassField* monoField = NULL;
				void* iter = NULL;
				while ((monoField = mono_class_get_fields(mono_object_get_class(scriptClassValue->monoObject), &iter)) != NULL)
				{
					int type = mono_type_get_type(mono_field_get_type(monoField));
					if (type != MONO_TYPE_ARRAY && type != MONO_TYPE_CLASS) {
						if (allFields.find(key) != allFields.end())
							FieldSetVal(type, allFields.at(key).at(mono_field_get_name(monoField))->mValue, scriptClassValue->monoObject, monoField);
					}
				}
			}
		}
	}
	void Scene::Stop() {
		// Change active scene, update the selected object scene, delete runtime and set running to false.
		Editor::selectedGameObject = nullptr;
		delete(Application->runtimeScene);
		Application->runningScene = false;
		Application->activeScene = Application->editorScene;
		Application->activeCamera = Application->editorCamera;
		Editor::ScriptManager::ReloadScriptsAssembly();
	}
	void Scene::Pause() {

	}

	Entity* Scene::GetEntity(uint64_t uuid) {
		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end())
			return &it->second;

	}
	Entity* Scene::GetEntityByName(std::string name) {
		if (Application->activeScene->entitiesNames.find(name) != Application->activeScene->entitiesNames.end()) {
			for (const auto& element : Application->activeScene->entitiesNames.at(name)) {
				return &Application->activeScene->entities.at(element);
			}
		}
		return nullptr;
	}
}