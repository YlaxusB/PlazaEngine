#include "Renderer.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Application/Application.h"
#include "Engine/Editor/Editor.h"

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
		glStencilMask(0x00);
		glm::mat4 projection = Application::activeCamera.GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application::activeCamera.GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->transform->GetTransform();

		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);

		modelMatrix = glm::scale(modelMatrix, Editor::selectedGameObject->transform->worldScale + glm::vec3(0.01f));
		outlineShader.use();
		outlineShader.setMat4("projection", projection);
		outlineShader.setMat4("view", view);
		outlineShader.setMat4("model", modelMatrix);
		glDisable(GL_DEPTH_TEST);




		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, Application::frameBuffer);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, Engine::Application::textureColorbuffer);


		// clear all relevant buffers
		//glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);



		Engine::Application::outlineBlurShader->use();
		Engine::Application::outlineBlurShader->setInt("screenTexture", 0);
		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);

		Application::outlineBlurShader->setMat4("projection", projection);
		Application::outlineBlurShader->setMat4("view", view);
		glUniform2f(glGetUniformLocation(Application::outlineBlurShader->ID, "textureSize"), Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);

		glBindVertexArray(Engine::Application::blurVAO);
		glBindTexture(GL_TEXTURE_2D, Engine::Application::textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		/*
		Application::updateBuffers(Application::textureColorbuffer, Application::rbo);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Application::textureColorbuffer);

		glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// Set the uniforms for the shader
		glUniform1i(glGetUniformLocation(Application::outlineBlurShader->ID, "originalTexture"), 0);
		Application::outlineBlurShader->setMat4("projection", projection);
		Application::outlineBlurShader->setMat4("view", view);
		glUniform2f(glGetUniformLocation(Application::outlineBlurShader->ID, "textureSize"), Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);
		
		// Render a fullscreen quad
		//renderFullscreenQuad();

//		Renderer::OutlineDraw(Editor::selectedGameObject, outlineShader, glm::vec3(1));
		//Renderer::OutlineDraw(Editor::selectedGameObject, outlineShader, glm::vec3(1 - outlineScale));



		renderFullscreenQuad();
		*/
		glBindTexture(GL_TEXTURE_2D, 0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	// Draw the Outline
	void Renderer::OutlineDraw(GameObject* gameObject, Shader shader, glm::vec3 scale) {
		if (gameObject->GetComponent<MeshRenderer>()) {
			glm::mat4 modelMatrix = gameObject->transform->GetTransform(Editor::selectedGameObject->transform->worldPosition, gameObject->transform->worldScale);
			modelMatrix = glm::scale(modelMatrix, scale);
			shader.setMat4("model", modelMatrix);
			gameObject->GetComponent<MeshRenderer>()->mesh.Draw(shader);
		}
		for (GameObject* child : gameObject->children) {
			Renderer::OutlineDraw(child, shader, scale);
		}
	}
}