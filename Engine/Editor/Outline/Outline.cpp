#include "Engine/Core/PreCompiledHeaders.h"
#include "Outline.h"
#include "Engine/Editor/Editor.h"
//using namespace Plaza::Editor;
//void Outline::CombineOutlineToScene() {
//
//}

namespace Plaza::Editor {
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
		int width = Application::Get()->appSizes->sceneSize.x;
		int height = Application::Get()->appSizes->sceneSize.y;
		blurringBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGB32F, width, height, GL_RGB, GL_FLOAT, GL_LINEAR);
		blurringBuffer->Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glBindBuffer(GL_FRAMEBUFFER, 0);
		blurringBuffer->InitRenderBufferObject(GL_DEPTH24_STENCIL8, width, height);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER };
		blurringBuffer->DrawAttachments(attachments, width, height);
		Application::Get()->outlineBlurShader->setInt("sceneBuffer", 0);
	}

	void Outline::BlurBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, blurringBuffer->buffer);
		glViewport(Application::Get()->appSizes->sceneStart.x, Application::Get()->appSizes->sceneStart.y, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Application::Get()->singleColorShader->use();
		glm::mat4 projection = Application::Get()->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application::Get()->activeCamera->GetViewMatrix();
		glm::mat4 modelMatrix = Editor::selectedGameObject->GetComponent<Transform>()->GetTransform();
		Application::Get()->singleColorShader->setMat4("projection", projection);
		Application::Get()->singleColorShader->setMat4("view", view);
		Application::Get()->singleColorShader->setMat4("model", modelMatrix);

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		Outline::RenderSelectedObjects(Editor::selectedGameObject, *Application::Get()->singleColorShader);

		glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->edgeDetectionFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(Application::Get()->appSizes->sceneStart.x, Application::Get()->appSizes->sceneStart.y, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);
		// Render the blur buffer
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurringBuffer->colorBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Outline::blurringBuffer->depthBuffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Application::Get()->selectedDepthStencilRBO);

		Application::Get()->outlineBlurShader->use();
		glBindVertexArray(Application::Get()->blurVAO);
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
	void Outline::RenderSelectedObjects(Entity* entity, Shader shader) {
		MeshRenderer* mr = entity->GetComponent<MeshRenderer>();
		if (mr) {
			glm::mat4 modelMatrix = entity->GetComponent<Transform>()->modelMatrix;
			shader.setMat4("model", modelMatrix);
			if(mr->mesh)
			mr->mesh->Draw(shader);
		}
		for (uint64_t child : entity->childrenUuid) {
			Outline::RenderSelectedObjects(&Application::Get()->activeScene->entities[child], shader);
		}
	}
}