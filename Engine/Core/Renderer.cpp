#include "Renderer.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Application/Application.h"
#include "Engine/Editor/Editor.h"

#include "Engine/Core/Skybox.h"
void renderFullscreenQuad() {
	// skybox cube
	glBindVertexArray(Engine::Application::blurVAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

}

namespace Engine {
	// Render all GameObjects
	void Renderer::Render(Shader& shader) {
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

				//glBindTexture(GL_TEXTURE0, Application::frameBuffer);
				meshRenderer->mesh.Draw(shader);
				//glBindTexture(GL_TEXTURE0, 0);

			}
		}
	}

	void Renderer::RenderOutline(Shader outlineShader) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, Application::edgeDetectionFramebuffer);
		glViewport(Application::appSizes.sceneStart.x, Application::appSizes.sceneStart.y, Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);

		glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application::textureColorbuffer);
		Application::edgeDetectionShader->use();
		Application::edgeDetectionShader->setInt("screenTexture", 0);
		//glUniform1i(glGetUniformLocation(Application::edgeDetectionShader->ID, "screenTexture"), 0);

		glBindVertexArray(Engine::Application::blurVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//Renderer::Render(*Application::shader);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		/*
		glStencilMask(0x00);
		glm::mat4 projection = Application::activeCamera.GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application::activeCamera.GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->transform->GetTransform();

		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);

		modelMatrix = glm::scale(modelMatrix, Editor::selectedGameObject->transform->worldScale + glm::vec3(0.01f));
	Application::edgeDetectionShader->use();
	Application::edgeDetectionShader->setMat4("projection", projection);
		Application::edgeDetectionShader->setMat4("view", view);
		Application::edgeDetectionShader->setMat4("model", modelMatrix);
		glDisable(GL_DEPTH_TEST);

		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, Application::edgeDetectionFramebuffer);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);
		glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Application::edgeDetectionShader->use();
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Application::blurColorBuffer);
		glBindTexture(GL_TEXTURE_2D, Application::textureColorbuffer);
		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);
		GLint textureUniform = glGetUniformLocation(Application::edgeDetectionShader->ID, "screenTexture");
		glUniform1i(textureUniform, 0); // 0 corresponds to GL_TEXTURE0

		glBindVertexArray(Engine::Application::blurVAO);
		glBindTexture(GL_TEXTURE_2D, Engine::Application::textureColorbuffer);	// use the color attachment texture as the texture of the quad plane

		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);
		GLuint mainTexUniform = glGetUniformLocation(Application::edgeDetectionShader->ID, "screenTexture");
		//glUniform1i(mainTexUniform, Engine::Application::textureColorbuffer);
		Application::edgeDetectionShader->setInt("screenTexture", 0);
		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Renderer::OutlineDraw(Editor::selectedGameObject, *Engine::Application::edgeDetectionShader);
		//Render(*Application::edgeDetectionShader);
		//Skybox::Update();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
		*/
	}

	/*
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, Application::edgeDetectionFramebuffer);
		glViewport(Application::appSizes.sceneStart.x, Application::appSizes.sceneStart.y, Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);
		glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//Renderer::Render(*Application::shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/

	// Draw the Outline
	void Renderer::OutlineDraw(GameObject* gameObject, Shader shader) {
		if (gameObject->GetComponent<MeshRenderer>()) {
			glm::mat4 modelMatrix = gameObject->transform->GetTransform(Editor::selectedGameObject->transform->worldPosition, gameObject->transform->worldScale);
			shader.setMat4("model", modelMatrix);
			gameObject->GetComponent<MeshRenderer>()->mesh.Draw(shader);
		}
		for (GameObject* child : gameObject->children) {
			Renderer::OutlineDraw(child, shader);
		}
	}
}