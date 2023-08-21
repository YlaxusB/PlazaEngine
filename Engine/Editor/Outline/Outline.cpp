#include "Engine/Core/PreCompiledHeaders.h"
#include "Outline.h"
#include "Engine/Editor/Editor.h"
//using namespace Engine::Editor;
//void Outline::CombineOutlineToScene() {
//
//}

namespace Engine::Editor {
	unsigned int Outline::quadVAO = 0;
	unsigned int Outline::quadVBO = 0;
	Shader* Outline::combiningShader = nullptr;
	Shader* Outline::blurringShader = nullptr;

	FrameBuffer* Outline::blurringBuffer = nullptr;
	FrameBuffer* Outline::combiningBuffer = nullptr;
	FrameBuffer* Outline::edgeDetectionBuffer = nullptr;

	//using namespace Editor;

	void Outline::Init() {
		blurringBuffer = new FrameBuffer(GL_FRAMEBUFFER);
		int width = Application->appSizes->sceneSize.x;
		int height = Application->appSizes->sceneSize.y;
		blurringBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGB32F, width, height, GL_RGB, GL_FLOAT, GL_LINEAR);
		blurringBuffer->InitRenderBufferObject(GL_DEPTH24_STENCIL8, width, height);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER };
		blurringBuffer->DrawAttachments(attachments, width, height);
		Application->outlineBlurShader->setInt("sceneBuffer", 0);
	}

	void Outline::BlurBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, blurringBuffer->buffer);
		glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Application->singleColorShader->use();
		glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application->activeCamera->GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->GetComponent<Transform>()->GetTransform();
		Application->singleColorShader->setMat4("projection", projection);
		Application->singleColorShader->setMat4("view", view);
		Application->singleColorShader->setMat4("model", modelMatrix);

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		Outline::RenderSelectedObjects(Editor::selectedGameObject, *Application->singleColorShader);

		glBindFramebuffer(GL_FRAMEBUFFER, Application->edgeDetectionFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		// Render the blur buffer
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurringBuffer->colorBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Outline::blurringBuffer->depthBuffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Application->selectedDepthStencilRBO);

		Application->outlineBlurShader->use();
		glBindVertexArray(Application->blurVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Draw the Outline
	void Outline::RenderSelectedObjects(GameObject* gameObject, Shader shader) {
		MeshRenderer* mr = gameObject->GetComponent<MeshRenderer>();
		if (mr) {
			glm::mat4 modelMatrix = gameObject->GetComponent<Transform>()->modelMatrix;
			shader.setMat4("model", modelMatrix);
			mr->mesh->Draw(shader);
		}
		for (uint64_t child : gameObject->childrenUuid) {
			Outline::RenderSelectedObjects(&Application->activeScene->entities[child], shader);
		}
	}
}