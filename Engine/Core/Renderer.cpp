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
int i = 0;
namespace Engine {
	// Render all GameObjects
	void Renderer::Render(Shader& shader) {

		shader.use();

		glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application->activeCamera->GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", Application->activeCamera->Position);
		glm::vec3 lightPos = Application->Shadows->lightPos;
		shader.setVec3("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_2D_ARRAY, Application->Shadows->shadowsDepthMap);
		shader.setInt("shadowsDepthMap", 30);
		shader.setInt("shadowMap", 30);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		lightProjection = Application->Shadows->lightProjection;
		lightView = Application->Shadows->lightView;
		//lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		shader.setVec3("lightDir", Application->Shadows->lightDir);
		shader.setFloat("farPlane", Application->activeCamera->farPlane);
		shader.setInt("cascadeCount", Application->Shadows->shadowCascadeLevels.size());
		for (size_t i = 0; i < Application->Shadows->shadowCascadeLevels.size(); ++i)
		{
			shader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", Application->Shadows->shadowCascadeLevels[i]);
		}


		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		// End measuring time and calculate duration


		//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
		//// Print the execution time
		//std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

		Application->activeCamera->UpdateFrustum();
		for (MeshRenderer* meshRenderer : meshRenderers) {
			Transform* transform = meshRenderer->gameObject->transform;

			if (Application->activeCamera->IsInsideViewFrustum(transform->position)) {



				glm::mat4 modelMatrix = transform->modelMatrix;
				shader.setMat4("model", modelMatrix);
				meshRenderer->mesh->BindTextures(shader);
				meshRenderer->mesh->Draw(shader);
			}
			/*
			auto startTime = std::chrono::high_resolution_clock::now();
			// End measuring time and calculate duration
			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
			// Print the execution time
			std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
			*/
		}
	}
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
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
		Application->singleColorShader->setFloat("objectID", Editor::selectedGameObject->id);
		glStencilFunc(GL_ALWAYS, 0, 0x00);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		Editor::Outline::BlurBuffer();
		glDisable(GL_STENCIL_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
	}




}