#include "Renderer.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Application/Application.h"
#include "Engine/Editor/Editor.h"
namespace Engine {
	// Render all GameObjects
	void Renderer::Render(Shader shader) {
		shader.use();
		for (GameObject* gameObject : gameObjects) {
			MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer && gameObject->parent != nullptr) {

				glm::mat4 projection = Application::activeCamera.GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
				glm::mat4 view = Application::activeCamera.GetViewMatrix();
				glm::mat4 modelMatrix = gameObject->transform->GetTransform();

				shader.setMat4("projection", projection);
				shader.setMat4("view", view);
				shader.setMat4("model", modelMatrix);
				shader.setFloat("objectID", gameObject->id);

				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				meshRenderer->mesh.Draw(shader);
			}
		}
	}

	void Renderer::RenderOutline(Shader outlineShader) {
		glStencilMask(0x00);
		glm::mat4 projection = Application::activeCamera.GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application::activeCamera.GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->transform->GetTransform();

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		modelMatrix = glm::scale(modelMatrix, Editor::selectedGameObject->transform->worldScale + glm::vec3(0.01f));
		outlineShader.use();
		outlineShader.setMat4("projection", projection);
		outlineShader.setMat4("view", view);
		outlineShader.setMat4("model", modelMatrix);
		Renderer::OutlineDraw(Editor::selectedGameObject, outlineShader, glm::vec3(0.03f));

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	// Draw the Outline
	void Renderer::OutlineDraw(GameObject* gameObject, Shader shader, glm::vec3 scale) {
		if (gameObject->GetComponent<MeshRenderer>()) {
			glm::mat4 modelMatrix = gameObject->transform->GetTransform(Editor::selectedGameObject->transform->worldPosition, gameObject->transform->worldScale + scale);
			shader.setMat4("model", modelMatrix);
			gameObject->GetComponent<MeshRenderer>()->mesh.Draw(shader);
		}
		for (GameObject* child : gameObject->children) {
			Renderer::OutlineDraw(child, shader, scale);
		}
	}
}