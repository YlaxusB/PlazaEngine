#include "Engine/Core/PreCompiledHeaders.h"
#include "ScreenSpaceReflections.h"
#include "Engine/Core/Renderer/OpenGL/Renderer.h"

namespace Plaza {
	FrameBuffer* ScreenSpaceReflections::mScreenSpaceReflectionsFbo = nullptr;
	Shader* ScreenSpaceReflections::mScreenSpaceReflectionsShader = nullptr;
	void ScreenSpaceReflections::Init() {
		mScreenSpaceReflectionsFbo = new FrameBuffer(GL_FRAMEBUFFER);
		mScreenSpaceReflectionsFbo->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		mScreenSpaceReflectionsFbo->DrawAttachments(attachments, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ScreenSpaceReflections::Update() {
		PLAZA_PROFILE_SECTION("Screen Space Reflections");
		mScreenSpaceReflectionsFbo->Bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		mScreenSpaceReflectionsShader->use();
		mScreenSpaceReflectionsShader->setVec3("cameraPos", Application::Get()->activeCamera->Position);
		mScreenSpaceReflectionsShader->setMat4("view", Application::Get()->activeCamera->GetViewMatrix());
		mScreenSpaceReflectionsShader->setMat4("invView", glm::inverse(Application::Get()->activeCamera->GetViewMatrix()));
		mScreenSpaceReflectionsShader->setMat4("invProjection", glm::inverse(Application::Get()->activeCamera->GetProjectionMatrix()));
		mScreenSpaceReflectionsShader->setMat4("projection", Application::Get()->activeCamera->GetProjectionMatrix());

		glBindTextureUnit(0, Application::Get()->gPosition);
		glBindTextureUnit(1, Application::Get()->gNormal);
		glBindTextureUnit(2, Application::Get()->gDiffuse);
		glBindTextureUnit(3, Application::Get()->gOthers);
		glBindTextureUnit(4, Application::Get()->gDepth);
		glBindTextureUnit(5, Application::Get()->hdrSceneColor);

		Application::Get()->mRenderer->RenderFullScreenQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
