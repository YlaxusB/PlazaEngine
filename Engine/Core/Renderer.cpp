#include "Engine/Core/PreCompiledHeaders.h"
#include "Renderer.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Editor/Outline/Outline.h"
#include "Engine/Core/Skybox.h"
void renderFullscreenQuad() {
	// skybox cube
	glBindVertexArray(Engine::Application->blurVAO);
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
				glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
				glm::mat4 view = Application->activeCamera->GetViewMatrix();
				glm::mat4 modelMatrix = gameObject->transform->GetTransform();



				shader.setMat4("projection", projection);
				shader.setMat4("view", view);
				shader.setMat4("model", modelMatrix);
				shader.setFloat("objectID", gameObject->id);
				// set light uniforms
				shader.setVec3("viewPos", Application->activeCamera->Position);
				glm::vec3 lightPos = Application->Shadows->lightPos;
				shader.setVec3("lightPos", lightPos);
				glActiveTexture(GL_TEXTURE30);
				glBindTexture(GL_TEXTURE_2D, Application->Shadows->shadowsDepthMap);
				shader.setInt("shadowsDepthMap", 30);
				shader.setInt("shadowMap", 30);



				glm::mat4 lightProjection, lightView;
				glm::mat4 lightSpaceMatrix;
				lightProjection = Application->Shadows->lightProjection;
				lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
				lightSpaceMatrix = lightProjection * lightView;
				shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
				shader.setFloat("farPlane", Application->activeCamera->farPlane);
				std::vector<float> shadowCascadeLevels{ Application->activeCamera->farPlane / 50.0f, Application->activeCamera->farPlane / 25.0f, Application->activeCamera->farPlane / 10.0f, Application->activeCamera->farPlane / 2.0f };
				shader.setInt("cascadeCount", shadowCascadeLevels.size());
				for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
				{
					shader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
				}
				//shader.setInt("blinn", blinn);

				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				meshRenderer->mesh.Draw(shader);
			}
		}
	}
	//void Renderer::OutlineDraw(Editor::selectedGameObject, *Application->shader);
	void Renderer::BlurBuffer()
	{

	}



	void Renderer::RenderOutline(Shader outlineShader) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, Application->selectedFramebuffer);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//DetectEdges();

		glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		//glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Enable stencil test
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		Application->singleColorShader->use();
		glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application->activeCamera->GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->transform->GetTransform();

		Application->singleColorShader->setMat4("projection", projection);
		Application->singleColorShader->setMat4("view", view);
		Application->singleColorShader->setMat4("model", modelMatrix);
		Application->singleColorShader->setFloat("objectID", Editor::selectedGameObject->id);
		glStencilFunc(GL_ALWAYS, 0, 0x00);
		glStencilMask(0x00);
		Editor::Outline::RenderSelectedObjects2(Editor::selectedGameObject, *Application->singleColorShader);
		glDisable(GL_DEPTH_TEST);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		Editor::Outline::BlurBuffer();
		glDisable(GL_STENCIL_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);



		glDisable(GL_DEPTH_TEST);


	}




}