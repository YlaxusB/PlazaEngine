#include "Engine/Core/PreCompiledHeaders.h"
#include "ScreenSpaceReflections.h"
#include "Engine/Core/Renderer.h"

namespace Plaza {
	FrameBuffer* ScreenSpaceReflections::mScreenSpaceReflectionsFbo = nullptr;
	Shader* ScreenSpaceReflections::mScreenSpaceReflectionsShader = nullptr;
	void ScreenSpaceReflections::Init() {
		mScreenSpaceReflectionsFbo = new FrameBuffer(GL_FRAMEBUFFER);
		mScreenSpaceReflectionsFbo->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		mScreenSpaceReflectionsFbo->DrawAttachments(attachments, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ScreenSpaceReflections::Update() {
		mScreenSpaceReflectionsFbo->Bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		mScreenSpaceReflectionsShader->use();
		mScreenSpaceReflectionsShader->setVec3("viewPos", Application->activeCamera->Position);
		mScreenSpaceReflectionsShader->setMat4("view", Application->activeCamera->GetViewMatrix());
		mScreenSpaceReflectionsShader->setMat4("invView", glm::inverse(Application->activeCamera->GetViewMatrix()));
		mScreenSpaceReflectionsShader->setMat4("invProjection", glm::inverse(Application->activeCamera->GetProjectionMatrix()));
		mScreenSpaceReflectionsShader->setMat4("projection", Application->activeCamera->GetProjectionMatrix());
		//mScreenSpaceReflectionsShader->setMat4("cameraPosition", Application->activeCamera->getpo());
		
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, Application->gPosition);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Application->gNormal);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Application->gDiffuse);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Application->gOthers);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, Application->gDepth);
		//glBindTexture(GL_TEXTURE_2D, Application->hdrFramebuffer);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
		Renderer::RenderQuadOnScreen();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
