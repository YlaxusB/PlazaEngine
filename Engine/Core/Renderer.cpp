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

	void DetectEdges() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, Application::edgeDetectionFramebuffer);
		glViewport(Application::appSizes.sceneStart.x, Application::appSizes.sceneStart.y, Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);

		glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application::textureColorbuffer);
		Application::edgeDetectionShader->use();
		Application::edgeDetectionShader->setInt("sceneBuffer", 0);
		//glUniform1i(glGetUniformLocation(Application::edgeDetectionShader->ID, "screenTexture"), 0);

		glBindVertexArray(Engine::Application::blurVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//Renderer::Render(*Application::shader);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//void Renderer::OutlineDraw(Editor::selectedGameObject, *Application::shader);
	void Renderer::BlurBuffer()
	{
		// Enable stencil testing
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// Clear the stencil buffer to 0
		glClear(GL_STENCIL_BUFFER_BIT);

		// Set the stencil mask to 0xFF
		glStencilMask(0xFF);

		// Render the cubes to update the stencil buffer
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// Bind the blur framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Application::blurFramebuffer);
		glViewport(Application::appSizes.sceneStart.x, Application::appSizes.sceneStart.y, Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);

		// Clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Enable stencil test
		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilMask(0xFF);

		// Render the cubes here
		Application::singleColorShader->use();
		glm::mat4 projection = Application::activeCamera.GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application::activeCamera.GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->transform->GetTransform();

		Application::singleColorShader->setMat4("projection", projection);
		Application::singleColorShader->setMat4("view", view);
		Application::singleColorShader->setMat4("model", modelMatrix);
		Application::singleColorShader->setFloat("objectID", Editor::selectedGameObject->id);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		Renderer::OutlineDraw(Editor::selectedGameObject, *Application::shader, glm::vec3(0.0f));

		// Disable stencil test for rendering the blur buffer
		//glDisable(GL_STENCIL_TEST);

		// Render the blur buffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application::selectedColorBuffer);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
		glDisable(GL_DEPTH_TEST);

		// Unbind textures and framebuffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glBindFramebuffer(GL_FRAMEBUFFER, Application::edgeDetectionFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(Application::appSizes.sceneStart.x, Application::appSizes.sceneStart.y, Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);
		// Render the blur buffer
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
		glDisable(GL_DEPTH_TEST);
		Application::singleColorShader->use();
		Application::singleColorShader->setMat4("projection", projection);
		Application::singleColorShader->setMat4("view", view);
		Application::singleColorShader->setMat4("model", modelMatrix);
		Application::singleColorShader->setFloat("objectID", Editor::selectedGameObject->id);
		//Renderer::OutlineDraw(Editor::selectedGameObject, *Application::singleColorShader, glm::vec3(1.0f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application::blurColorBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Application::blurDepthStencilRBO);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Application::selectedDepthStencilRBO);
		Application::outlineBlurShader->use();
		Application::outlineBlurShader->setInt("sceneBuffer", 0);
		Application::outlineBlurShader->setInt("depthStencilTexture", 1);
		Application::outlineBlurShader->setInt("depthStencilTexture2", 2);

		glBindVertexArray(Engine::Application::blurVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/*
		Application::shader->use();
		Application::shader->setMat4("projection", projection);
		Application::shader->setMat4("view", view);
		Application::shader->setMat4("model", modelMatrix);
		Application::shader->setFloat("objectID", Editor::selectedGameObject->id);
		Renderer::OutlineDraw(Editor::selectedGameObject, *Application::shader, glm::vec3(0.0f));
		*/

		// Unbind textures and framebuffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



	void Renderer::RenderOutline(Shader outlineShader) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, Application::selectedFramebuffer);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//DetectEdges();

		glViewport(Application::appSizes.sceneStart.x, Application::appSizes.sceneStart.y, Application::appSizes.sceneSize.x, Application::appSizes.sceneSize.y);
		//glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Enable stencil test
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		Application::shader->use();
		glm::mat4 projection = Application::activeCamera.GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application::activeCamera.GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->transform->GetTransform();

		Application::shader->setMat4("projection", projection);
		Application::shader->setMat4("view", view);
		Application::shader->setMat4("model", modelMatrix);
		Application::shader->setFloat("objectID", Editor::selectedGameObject->id);
		glStencilFunc(GL_ALWAYS, 0, 0x00);
		glStencilMask(0x00);
		OutlineDraw(Editor::selectedGameObject, *Application::shader, glm::vec3(0.0f));
		glDisable(GL_DEPTH_TEST);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		Renderer::BlurBuffer();
		glDisable(GL_STENCIL_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);



		glDisable(GL_DEPTH_TEST);


	}



	// Draw the Outline
	void Renderer::OutlineDraw(GameObject* gameObject, Shader shader, glm::vec3 scale) {
		if (gameObject->GetComponent<MeshRenderer>()) {
			glm::mat4 modelMatrix = gameObject->transform->GetTransform(Editor::selectedGameObject->transform->worldPosition, gameObject->transform->worldScale);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f) + scale);
			shader.setMat4("model", modelMatrix);
			gameObject->GetComponent<MeshRenderer>()->mesh.Draw(shader);
		}
		for (GameObject* child : gameObject->children) {
			Renderer::OutlineDraw(child, shader, scale);
		}
	}
}