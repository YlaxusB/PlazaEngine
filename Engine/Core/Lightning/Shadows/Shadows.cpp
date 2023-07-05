#include "Shadows.h"
#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Core/Renderer.h"
namespace Engine {
	void ShadowsClass::Init() {
		glGenFramebuffers(1, &shadowsFBO);
		glGenTextures(1, &shadowsDepthMap);
		glBindTexture(GL_TEXTURE_2D, shadowsDepthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, shadowsFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowsDepthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ShadowsClass::GenerateDepthMap() {
		glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowsFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_CULL_FACE);
		ShadowsClass::RenderScene(*Application->shadowsDepthShader);;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ShadowsClass::RenderScene(Shader& shader) {
		shader.use();
		for (GameObject* gameObject : gameObjects) {
			MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer && gameObject->parent != nullptr) {
				glm::mat4 modelMatrix = gameObject->transform->GetTransform();

				shader.setMat4("model", modelMatrix);

				glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
				glm::mat4 lightProjection, lightView;
				glm::mat4 lightSpaceMatrix;
				float near_plane = 1.0f, far_plane = 7.5f;
				lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
				lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
				lightSpaceMatrix = lightProjection * lightView;
				shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

				meshRenderer->mesh.Draw(shader);
			}
		}
	}
}