#include "Engine/Core/PreCompiledHeaders.h"
#include "ScreenSpaceReflections.h"
#include "Engine/Core/Renderer/OpenGL/Renderer.h"

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
		PLAZA_PROFILE_SECTION("Screen Space Reflections");
		mScreenSpaceReflectionsFbo->Bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		mScreenSpaceReflectionsShader->use();
		mScreenSpaceReflectionsShader->setVec3("cameraPos", Application->activeCamera->Position);
		mScreenSpaceReflectionsShader->setMat4("view", Application->activeCamera->GetViewMatrix());
		mScreenSpaceReflectionsShader->setMat4("invView", glm::inverse(Application->activeCamera->GetViewMatrix()));
		mScreenSpaceReflectionsShader->setMat4("invProjection", glm::inverse(Application->activeCamera->GetProjectionMatrix()));
		mScreenSpaceReflectionsShader->setMat4("projection", Application->activeCamera->GetProjectionMatrix());

		glBindTextureUnit(0, Application->gPosition);
		glBindTextureUnit(1, Application->gNormal);
		glBindTextureUnit(2, Application->gDiffuse);
		glBindTextureUnit(3, Application->gOthers);
		glBindTextureUnit(4, Application->gDepth);
		glBindTextureUnit(5, Application->hdrSceneColor);

		Application->mRenderer->RenderFullScreenQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
