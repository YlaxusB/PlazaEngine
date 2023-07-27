#include "Engine/Core/PreCompiledHeaders.h"
#include "Shadows.h"

#include "Engine/Core/Renderer.h"

const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));

namespace Engine {


	void ShadowsClass::Init() {
		glGenFramebuffers(1, &shadowsFBO);
		glGenTextures(1, &shadowsDepthMap);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowsDepthMap);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		shadowCascadeLevels = vector{ Application->activeCamera->farPlane / 50.0f, Application->activeCamera->farPlane / 25.0f, Application->activeCamera->farPlane / 10.0f, Application->activeCamera->farPlane / 2.0f };
		std::cout << "aiaiaiaiaia:" << std::endl;
		std::cout << shadowCascadeLevels.size() << std::endl;
		glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(shadowCascadeLevels.size()) + 1,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, shadowsFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowsDepthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
			throw 0;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// configure UBO
		// --------------------
		glGenBuffers(1, &matricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * maximumCascades, nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void ShadowsClass::GenerateDepthMap() {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 0. UBO setup
		const auto lightMatrices = ShadowsClass::GetLightSpaceMatrices();
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
		for (size_t i = 0; i < lightMatrices.size(); ++i)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		Application->shadowsDepthShader->use();

		float mult = 1.0f;
		shadowCascadeLevels = vector{ Application->activeCamera->farPlane / (9000.0f * mult), Application->activeCamera->farPlane / (3000.0f * mult), Application->activeCamera->farPlane / (1000.0f * mult), Application->activeCamera->farPlane / (500.0f * mult), Application->activeCamera->farPlane / (250.0f * mult), Application->activeCamera->farPlane / (100.0f * mult), Application->activeCamera->farPlane / (2.0f * mult) };

		lightDistance = glm::vec3(80.0f, 120.0f, 0.0f); //* glm::vec3(glfwGetTime());
		near_plane = 1.0f, far_plane = lightDistance.x + lightDistance.y;
		float size = 100.0f;

		glm::vec3 cameraPosition = glm::vec3(0.0f);//Application->activeCamera->Position;

		lightProjection = glm::ortho(
			cameraPosition.x - size,
			cameraPosition.x + size,
			cameraPosition.y - size,
			cameraPosition.y + size,
			near_plane,
			far_plane
		);

		lightPos = Application->activeCamera->Position + lightDistance;
		cameraPosition = Application->activeCamera->Position;
		lightView = glm::lookAt(lightPos, cameraPosition, glm::vec3(0.0, 1.0, 0.0));
		//lightProjection = glm::ortho(-500.0f + -Application->activeCamera->Position.x, 500.0f + Application->activeCamera->Position.x, -500.0f + -Application->activeCamera->Position.y, 500.0f + Application->activeCamera->Position.y, near_plane, far_plane);
		glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, depthMapResolution, depthMapResolution);
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

		glm::vec3 lightPos = Application->Shadows->lightPos;
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		lightProjection = Application->Shadows->lightProjection;
		lightView = Application->Shadows->lightView;
		//lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		for (MeshRenderer* meshRenderer : meshRenderers) {
			Transform* transform = meshRenderer->gameObject->transform;

			// Check if the object is inside the view frustum
			if (Application->activeCamera->IsInsideViewFrustum(transform->position)) {

				glm::mat4 modelMatrix = transform->modelMatrix;
				shader.setMat4("model", modelMatrix);

				meshRenderer->mesh->Draw(shader);
			}
		}
	}
	using std::min;
	glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane)
	{
		const auto proj = glm::perspective(
			glm::radians(Application->activeCamera->Zoom), (float)Application->appSizes->sceneSize.x / (float)Application->appSizes->sceneSize.y, nearPlane,
			farPlane);
		const auto corners = Application->activeCamera->getFrustumCornersWorldSpace(proj, Application->activeCamera->GetViewMatrix());

		glm::vec3 center = glm::vec3(0, 0, 0);
		for (const auto& v : corners)
		{
			center += glm::vec3(v);
		}
		center /= corners.size();
		const float LARGE_CONSTANT = std::abs(std::numeric_limits<float>::lowest());
		const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));
		float minX = LARGE_CONSTANT;
		float maxX = std::numeric_limits<float>::lowest();
		float minY = LARGE_CONSTANT;
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = LARGE_CONSTANT;
		float maxZ = std::numeric_limits<float>::lowest();
		for (const auto& v : corners)
		{
			const auto trf = lightView * v;
			minX = min(minX, trf.x);
			maxX = max(maxX, trf.x);
			minY = min(minY, trf.y);
			maxY = max(maxY, trf.y);
			minZ = min(minZ, trf.z);
			maxZ = max(maxZ, trf.z);
		}

		// Tune this parameter according to the scene
		constexpr float zMult = 10.0f;
		if (minZ < 0)
		{
			minZ *= zMult;
		}
		else
		{
			minZ /= zMult;
		}
		if (maxZ < 0)
		{
			maxZ /= zMult;
		}
		else
		{
			maxZ *= zMult;
		}
		const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		return lightProjection * lightView;
	}


	std::vector<glm::mat4> ShadowsClass::GetLightSpaceMatrices()
	{
		std::vector<glm::mat4> ret;
		for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
		{
			if (i == 0)
			{
				ret.push_back(getLightSpaceMatrix(Application->editorCamera->nearPlane, shadowCascadeLevels[i]));
			}
			else if (i < shadowCascadeLevels.size())
			{
				ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
			}
			else
			{
				ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], Application->editorCamera->farPlane));
			}
		}
		return ret;
	}
}