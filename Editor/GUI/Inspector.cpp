#include "Engine/Core/PreCompiledHeaders.h"
#include "Inspector.h"

#include "Editor/GUI/Inspector/MeshRendererInspector.h"
#include "Editor/GUI/Inspector/RigidBodyInspector.h"
#include "Editor/GUI/Inspector/SceneInspector.h"
#include "Editor/GUI/Inspector/ColliderInspector.h"
#include "Editor/GUI/Inspector/CppScriptComponentInspector.h"
#include "Editor/GUI/Inspector/CsScriptComponentInspector.h"
#include "Editor/GUI/Inspector/AudioListenerInspector.h"
#include "Editor/GUI/Inspector/AudioSourceInspector.h"
#include "Editor/GUI/Inspector/CameraInspector.h"

#include "Editor/GUI/Inspector/FileInspector/MaterialFileInspector.h"
#include "Editor/GUI/Inspector/FileInspector/TextEditor.h"
#include "Editor/GUI/Inspector/LightInspector.h"
#include "Editor/GUI/Inspector/CharacterControllerInspector.h"
#include "Editor/GUI/Inspector/TextRendererInspector.h"
#include "Editor/GUI/Inspector/AnimationComponentInspector.h"
#include "Editor/GUI/Inspector/GuiComponentInspector.h"

namespace Plaza::Editor {
	/* File Inspector*/
	std::vector<Component*> Inspector::ComponentInspector::components;
	void Inspector::FileInspector::CreateInspector() {
		ImGui::SetCursorPosY(50);
		ImGui::Indent(10);
		for (auto [key, value] : Editor::selectedFiles) {
			CreateRespectiveInspector(value);
		}
	}

	void Inspector::FileInspector::CreateRespectiveInspector(File* file) {
		if (!file->name.empty()) {
			std::string extension = std::filesystem::path{ file->directory }.extension().string();
			if (extension == Standards::materialExtName) {
				Editor::MaterialFileInspector::MaterialFileInspector(AssetsManager::GetMaterial(AssetsManager::GetAsset(std::filesystem::path(file->directory))->mAssetUuid));
			}
			else if (extension == Standards::modelExtName) {
				Editor::TextEditor::TextEditor(file);
			}
		}
	}

	/* Component Inspector*/
	void Inspector::ComponentInspector::CreateInspector(Scene* scene) {
		if (Editor::selectedGameObject) {
			ImGui::SetCursorPosY(50);
			ImGui::Indent(10);
			bool selectedEntityIsSceneEntity = Editor::selectedGameObject->uuid == Scene::GetActiveScene()->mainSceneEntity->uuid;
			if (selectedEntityIsSceneEntity) {
				SceneInspector::SceneInspector(Scene::GetActiveScene());
			}
			else {
				ImGui::Text(Editor::selectedGameObject->name.c_str());
				for (Component* component : components) {
					ComponentInspector::CreateRespectiveInspector(scene, component);
				}
				ImGui::Text("Parent Uuid: ");
				std::string uuidString = std::to_string(Editor::selectedGameObject->parentUuid);
				ImGui::Text(uuidString.c_str());
			}
		}
	}
	void Inspector::ComponentInspector::UpdateComponents() {
		components.clear();
		uint64_t uuid = Editor::selectedGameObject->uuid;
		Scene* scene = Scene::GetActiveScene();

		ComponentMask entityComponentMask = scene->GetEntity(uuid)->mComponentMask;
		for (ComponentPool* pool : scene->mComponentPools) {
			if (pool == nullptr)
				continue;
			if (entityComponentMask.test(pool->mComponentMask)) {
				components.push_back(static_cast<Component*>(pool->Get(uuid)));
			}
		}

		//FIX: Update the components inspector
		//if (activeScene->transformComponents.contains(uuid))
		//	components.push_back(&activeScene->transformComponents.at(uuid));
		//
		//if (activeScene->meshRendererComponents.contains(uuid))
		//	components.push_back(&activeScene->meshRendererComponents.at(uuid));
		//
		//if (activeScene->cameraComponents.contains(uuid))
		//	components.push_back(&activeScene->cameraComponents.at(uuid));
		//
		//if (activeScene->rigidBodyComponents.contains(uuid))
		//	components.push_back(&activeScene->rigidBodyComponents.at(uuid));
		//
		//if (activeScene->colliderComponents.contains(uuid))
		//	components.push_back(&activeScene->colliderComponents.at(uuid));
		//
		//if (activeScene->cppScriptComponents.contains(uuid))
		//	components.push_back(&activeScene->cppScriptComponents.at(uuid));
		//if (activeScene->csScriptComponents.contains(uuid)) {
		//	auto range = activeScene->csScriptComponents.equal_range(uuid);
		//	for (auto it = range.first; it != range.second; ++it) {
		//		components.push_back(&it->second);
		//	}
		//}
		//
		//if (activeScene->audioSourceComponents.contains(uuid))
		//	components.push_back(&activeScene->audioSourceComponents.at(uuid));
		//if (activeScene->audioListenerComponents.contains(uuid))
		//	components.push_back(&activeScene->audioListenerComponents.at(uuid));
		//if (activeScene->lightComponents.contains(uuid))
		//	components.push_back(&activeScene->lightComponents.at(uuid));
		//if (activeScene->characterControllerComponents.contains(uuid))
		//	components.push_back(&activeScene->characterControllerComponents.at(uuid));
		//if (activeScene->UITextRendererComponents.contains(uuid))
		//	components.push_back(&activeScene->UITextRendererComponents.at(uuid));
		//if (activeScene->animationComponentComponents.contains(uuid))
		//	components.push_back(&activeScene->animationComponentComponents.at(uuid));
		//if (activeScene->guiComponents.contains(uuid))
		//	components.push_back(&activeScene->guiComponents.at(uuid));
	}

	void Inspector::ComponentInspector::CreateRespectiveInspector(Scene* scene, Component* component) {
		if (TransformComponent* transform = dynamic_cast<TransformComponent*>(component)) {
			Editor::Gui::TransformInspector inspector{ scene, Editor::selectedGameObject };
		}
		else if (MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(component)) {
			Plaza::Editor::MeshRendererInspector::MeshRendererInspector(scene, Editor::selectedGameObject);
			//Plaza::Editor::MaterialInspector::MaterialInspector(Editor::selectedGameObject);
		}
		else if (Camera* camera = dynamic_cast<Camera*>(component)) {
			Plaza::Editor::CameraInspector::CameraInspector(camera);
		}
		else if (RigidBody* rigidBody = dynamic_cast<RigidBody*>(component)) {
			Plaza::Editor::RigidBodyInspector::RigidBodyInspector(rigidBody);
		}
		else if (Collider* collider = dynamic_cast<Collider*>(component)) {
			Plaza::Editor::ColliderInspector::ColliderInspector(scene, collider);
		}
		else if (CppScriptComponent* cppScriptComponent = dynamic_cast<CppScriptComponent*>(component)) {
			Plaza::Editor::CppScriptComponentInspector::CppScriptComponentInspector(cppScriptComponent);
		}
		else if (CsScriptComponent* csScriptComponent = dynamic_cast<CsScriptComponent*>(component)) {
			Plaza::Editor::CsScriptComponentInspector::CsScriptComponentInspector(csScriptComponent);
		}
		else if (AudioSource* audioSource = dynamic_cast<AudioSource*>(component)) {
			Plaza::Editor::AudioSourceInspector::AudioSourceInspector(audioSource);
		}
		else if (AudioListener* audioListener = dynamic_cast<AudioListener*>(component)) {
			Plaza::Editor::AudioListenerInspector::AudioListenerInspector(audioListener);
		}
		else if (Light* light = dynamic_cast<Light*>(component)) {
			Plaza::Editor::LightInspector::LightInspector(light);
		}
		else if (CharacterController* characterController = dynamic_cast<CharacterController*>(component)) {
			Plaza::Editor::CharacterControllerInspector::CharacterControllerInspector(characterController);
		}
		else if (Drawing::UI::TextRenderer* textRenderer = dynamic_cast<Drawing::UI::TextRenderer*>(component)) {
			Plaza::Editor::TextRendererInspector::TextRendererInspector(textRenderer);
		}
		else if (AnimationComponent* animationComponent = dynamic_cast<AnimationComponent*>(component)) {
			Plaza::Editor::AnimationComponentInspector::AnimationComponentInspector(animationComponent);
		}
		else if (GuiComponent* guiComponent = dynamic_cast<GuiComponent*>(component)) {
			Plaza::Editor::GuiComponentInspector::GuiComponentInspector(guiComponent);
		}
	}
}