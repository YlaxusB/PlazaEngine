#pragma once
#include "Engine/Core/Scene.h"

#include "Editor/GUI/Utils/DataVisualizer.h"

#include "Engine/Application/EditorCamera.h"
bool visualizingNormals = false;
namespace Plaza::Editor {
	void callback(float) {
		Application->editorCamera->Update();
		//Application->editorCamera = new Plaza::Camera(*Application->editorCamera);
		//Application->activeCamera = Application->editorCamera;
	}
	void callbacke(glm::vec3) {

	}
	void callbacke2(glm::vec3) {
		Application->editorCamera->Update();
	}
	static class SceneInspector {
	public:
		SceneInspector(Scene* scene) {
			/* Shadows */
			if (ImGui::TreeNodeEx("Shadows", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3& lightDir = Application->Shadows->lightDir;
				Utils::DragFloat3("Light Direction: ", lightDir, 0.1f, callbacke, -360.0f, 360.0f);
				ImGui::DragInt("Depth Map Resolution: ", reinterpret_cast<int*>(&Application->Shadows->depthMapResolution), 1024, 0, *"%d");

				ImGui::TreePop();
			}

			/* Rendering */
			if (ImGui::TreeNodeEx("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ImGui::Checkbox("Visualize Normals", &visualizingNormals)) {
					if (visualizingNormals) {
						Application->shader = new Shader((Application->enginePath + "\\Shaders\\normals\\normalVisualizerVertex.glsl").c_str(),
							(Application->enginePath + "\\Shaders\\normals\\normalVisualizerFragment.glsl").c_str(), (Application->enginePath + "\\Shaders\\normals\\normalVisualizerGeometry.glsl").c_str());
					}
					else {
						Application->shader = new Shader((Application->enginePath + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\1.model_loadingFragment.glsl").c_str());
					}
				}
				ImGui::TreePop();
			}

			/* Editor Camera */
			if (ImGui::TreeNodeEx("Editor Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
				Utils::DragFloat("Zoom: ", Application->editorCamera->Zoom, 0.1f, callback, 0.0f);
				Utils::DragFloat("Movement Speed: ", Application->editorCamera->MovementSpeed, 0.1f, callback, 0.0f);
				Utils::DragFloat("Camera Sensitivity: ", Application->editorCamera->MouseSensitivity, 0.1f, callback, 0.0f);
				Utils::DragFloat("Nearplane: ", Application->editorCamera->nearPlane, 0.1f, callback, -10000.0f);
				Utils::DragFloat("Farplane: ", Application->editorCamera->farPlane, 0.1f, callback, 0.0f);

				Utils::DragFloat("Pitch: ", Application->editorCamera->Pitch, 0.1f, callback);
				Utils::DragFloat3("Front: ", Application->editorCamera->Front, 0.1f, callbacke);
				Utils::DragFloat3("Right: ", Application->editorCamera->Right, 0.1f, callbacke2);
				Utils::DragFloat3("Up: ", Application->editorCamera->Up, 0.1f, callbacke2);
				Utils::DragFloat3("WorldUp: ", Application->editorCamera->WorldUp, 0.1f, callbacke2);
				Utils::DragFloat("Yaw: ", Application->editorCamera->Yaw, 0.1f, callback);

				Utils::DragFloat("Aspect Ratio: ", Application->editorCamera->aspectRatio, 0.1f, callback);
				Utils::DragFloat("X: ", Application->appSizes->sceneSize.x, 0.1f, callback);
				Utils::DragFloat("Y: ", Application->appSizes->sceneSize.y, 0.1f, callback);


				//Utils::DragFloat("Farplane: ", Application->editorCamera->farPlane, 0.1f, callback, 0.0f);
				//Utils::DragFloat("Farplane: ", Application->editorCamera->farPlane, 0.1f, callback, 0.0f);
				//Utils::DragFloat("Farplane: ", Application->editorCamera->farPlane, 0.1f, callback, 0.0f);

				ImGui::TreePop();
			}
		}
	};
}