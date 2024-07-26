#include "Engine/Core/PreCompiledHeaders.h"
#include "Renderer.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Editor/Outline/Outline.h"
#include "Engine/Core/Skybox.h"
#include "Engine/Core/FrameBuffer.h"
#include "Engine/Core/Renderer/OpenGL/Bloom.h"
#include "Engine/Core/Renderer/OpenGL/ScreenSpaceReflections.h"
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"
#include "OpenGLTexture.h"

void renderFullscreenQuad() {
	// skybox cube
	glBindVertexArray(Plaza::Application->blurVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);

}

int i = 0;
namespace Plaza {
	Shadows* OpenGLRenderer::GetShadows() {
		return this->mShadows;
	}
	OpenGLRenderer* OpenGLRenderer::GetRenderer() {
		return (OpenGLRenderer*)(Application->mRenderer);
	}

	unsigned int OpenGLRenderer::pingpongFBO[2];
	unsigned int OpenGLRenderer::pingpongColorbuffers[2];
	Shader* OpenGLRenderer::mergeShader = nullptr;
	Shader* OpenGLRenderer::blurShader = nullptr;
	FrameBuffer* OpenGLRenderer::hdrFramebuffer = nullptr;
	FrameBuffer* OpenGLRenderer::bloomBlurFrameBuffer = nullptr;
	FrameBuffer* OpenGLRenderer::bloomFrameBuffer = nullptr;
	void OpenGLRenderer::Init() {
		Application->mRendererAPI = RendererAPI::OpenGL;
		//hdrFramebuffer.
		InitQuad();

		bloomBlurFrameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
		bloomBlurFrameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		bloomBlurFrameBuffer->DrawAttachments(attachments, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);

		bloomFrameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
		bloomFrameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
		bloomFrameBuffer->DrawAttachments(attachments, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);

		/* Blur pingpong framebuffer */
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorbuffers);
		for (unsigned int i = 0; i < 2; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}

		ScreenSpaceReflections::Init();
		Bloom::Init();
	}

	void OpenGLRenderer::UpdateProjectManager() {}

	void OpenGLRenderer::InitShaders(std::string shadersFolder) {
		this->blurShader = new Shader((shadersFolder + "\\Shaders\\blur\\gaussianBlurVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\blur\\gaussianBlurFragment.glsl").c_str());

		this->mergeShader = new Shader((shadersFolder + "\\Shaders\\merge\\mergeVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\merge\\mergeFragment.glsl").c_str());
	}

	// Render all GameObjects
	void OpenGLRenderer::AddInstancesToRender() {
		PLAZA_PROFILE_SECTION("Render");
		Shader& shader = *Application->shader;
		shader.use();



		glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application->activeCamera->GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", Application->activeCamera->Position);
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_2D_ARRAY, Application->Shadows->shadowsDepthMap);
		shader.setInt("shadowsDepthMap", 30);
		shader.setInt("shadowMap", 30);
		glm::vec3 lightPos = Application->Shadows->lightPos;
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("lightDir", Application->Shadows->lightDir);
		shader.setFloat("farPlane", Application->activeCamera->farPlane);
		shader.setInt("cascadeCount", Application->Shadows->shadowCascadeLevels.size());
		shader.setVec3("lightDirection", Application->Shadows->lightDir);
		for (size_t i = 0; i < Application->Shadows->shadowCascadeLevels.size(); ++i) {
			shader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", Application->Shadows->shadowCascadeLevels[i]);
		}

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		Application->activeCamera->UpdateFrustum();
		//for (const auto& meshRendererPair : Application->activeScene->meshRendererComponents) {
		//	const MeshRenderer& meshRenderer = (meshRendererPair.second);
		//	if (!meshRenderer.instanced) {
		//		const auto transformIt = Application->activeScene->transformComponents.find(meshRendererPair.first);
		//		if (transformIt != Application->activeScene->transformComponents.end() && Application->activeCamera->IsInsideViewFrustum(transformIt->second.worldPosition)) {
		//			glm::mat4 modelMatrix = transformIt->second.modelMatrix;
		//			shader.setMat4("model", modelMatrix);
		//			//meshRenderer.mesh->BindTextures(shader);
		//			//meshRenderer.mesh->Draw(shader);
		//		}
		//	}
		//}
	}

	void OpenGLRenderer::RenderScreenSpaceReflections() {

	}

	void OpenGLRenderer::RenderInstances(Shader& shader) {
		PLAZA_PROFILE_SECTION("Render Instances");
		shader.use();



		glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
		glm::mat4 view = Application->activeCamera->GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", Application->activeCamera->Position);
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_2D_ARRAY, Application->Shadows->shadowsDepthMap);
		shader.setInt("shadowsDepthMap", 30);
		shader.setInt("shadowMap", 30);
		glm::vec3 lightPos = Application->Shadows->lightPos;
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("lightDir", Application->Shadows->lightDir);
		shader.setFloat("farPlane", Application->activeCamera->farPlane);
		shader.setInt("cascadeCount", Application->Shadows->shadowCascadeLevels.size());
		shader.setVec3("lightDirection", Application->Shadows->lightDir);
		for (size_t i = 0; i < Application->Shadows->shadowCascadeLevels.size(); ++i) {
			shader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", Application->Shadows->shadowCascadeLevels[i]);
		}

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		Application->activeCamera->UpdateFrustum();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (auto [renderGroupUuid, renderGroup] : Application->activeScene->renderGroups) {
			if (renderGroup.instanceModelMatrices.size() > 0) {
				//[] renderGroup->DrawInstanced(shader);
			}
		}
		//for (auto [key, mesh] : Application->activeScene->meshes) {
		//	if (mesh->instanceModelMatrices.size() > 0) {
		//		mesh->DrawInstanced(shader);
		//	}
		//}
	}

	void OpenGLRenderer::RenderShadowMap(Shader& shader) {
		shader.use();
		for (auto [renderGroupUuid, renderGroup] : Application->activeScene->renderGroups) {
			if (renderGroup.instanceModelMatrices.size() > 0) {
				//renderGroup->DrawInstanced(shader);
				//[] renderGroup->DrawInstancedToShadowMap(shader);
			}
		}
	}

	void OpenGLRenderer::BlurBuffer(GLint colorBuffer, int passes) {

		blurShader->use();
		blurShader->setInt("image", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		bool firstIteration = true;
		bool horizontal = true;

		for (unsigned int i = 0; i < passes; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			blurShader->setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffer : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			Application->mRenderer->RenderFullScreenQuad();
			horizontal = !horizontal;
			if (firstIteration)
				firstIteration = false;
		}
	}

	void OpenGLRenderer::MergeColors(GLint texture1, GLint texture2) {
		mergeShader->use();
		mergeShader->setInt("texture1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		mergeShader->setInt("texture2", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		Application->mRenderer->RenderFullScreenQuad();
	}

	void OpenGLRenderer::RenderOutline() {
		Shader* outlineShader = Application->outlineShader;
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
		glm::mat4 modelMatrix = Editor::selectedGameObject->GetComponent<Transform>()->modelMatrix;
		Application->singleColorShader->setMat4("projection", projection);
		Application->singleColorShader->setMat4("view", view);
		Application->singleColorShader->setFloat("objectID", Editor::selectedGameObject->uuid);
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

	void OpenGLRenderer::RenderHDR() {
		PLAZA_PROFILE_SECTION("HDR");
		glBindFramebuffer(GL_FRAMEBUFFER, Application->hdrFramebuffer);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Application->hdrShader->use();
		Application->hdrShader->setInt("hdr", 5);
		Application->hdrShader->setFloat("exposure", 0.30f);
		Application->hdrShader->setInt("hdrBuffer", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application->distortionCorrectionFrameBuffer->colorBuffer);
		Application->mRenderer->RenderFullScreenQuad();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//renderFullscreenQuad();
	}

	void OpenGLRenderer::RenderBloom() {
		///* Blur only the bright fragments */
		//Renderer::bloomBlurFrameBuffer->Bind();
		//Renderer::BlurBuffer(Application->hdrBloomColor, 10);
		////Renderer::bloomBlurFrameBuffer->Bind();
		////glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Renderer::CopyFrameBufferColor(Renderer::pingpongFBO[1], bloomBlurFrameBuffer->buffer);
		///* Merge the bright fragments with the scene */
		//glBindFramebuffer(GL_FRAMEBUFFER, Renderer::bloomFrameBuffer->buffer);
		//Renderer::MergeColors(Renderer::bloomBlurFrameBuffer->colorBuffer, Application->hdrSceneColor);
		Bloom::DrawBloom();

	}

	unsigned int OpenGLRenderer::quadVAO = 0;
	unsigned int OpenGLRenderer::quadVBO = 0;
	void OpenGLRenderer::InitQuad() {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	void OpenGLRenderer::RenderFullScreenQuad() {
		renderFullscreenQuad();
	}

	void OpenGLRenderer::CopyFrameBufferColor(GLint readBuffer, GLint drawBuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, drawBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuffer);
		glBlitFramebuffer(
			0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y,
			0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR
		);
	}

	void OpenGLRenderer::CopyLastFramebufferToFinalDrawBuffer() {
		GLint drawBuffer;
#ifdef GAME_MODE
		drawBuffer = 0;
#else
		drawBuffer = Application->frameBuffer;
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, drawBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->bloomFrameBuffer->buffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuffer);
		glBlitFramebuffer(
			0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y,
			0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR
		);
	}

	void OpenGLRenderer::Destroy() {

	}

	void OpenGLRenderer::InitGUI() {

	}

	void OpenGLRenderer::NewFrameGUI() {

	}

	void OpenGLRenderer::UpdateGUI() {

	}

	ImTextureID OpenGLRenderer::GetFrameImage() {
		return (ImTextureID)Application->textureColorbuffer;
	}

	Mesh& OpenGLRenderer::CreateNewMesh(vector<glm::vec3>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& uvs, vector<glm::vec3>& tangent, vector<unsigned int>& indices, vector<unsigned int>& materialsIndices, bool usingNormal, vector<BonesHolder> bonesHolder, vector<Bone>  uniqueBonesInfo) {
		return *new OpenGLMesh(vertices, normals, uvs, tangent, indices, usingNormal);
	}

	Texture* OpenGLRenderer::LoadTexture(std::string path, uint64_t uuid) {
		OpenGLTexture* texture = new OpenGLTexture();
		texture->mAssetUuid = uuid;
		texture->path = path;
		texture->rgba = glm::vec4(INFINITY);
		//    material->diffuse.path = FileDialog::OpenFileDialog(".jpeg");
		//    material->diffuse.rgba = glm::vec4(INFINITY);
		//    material->diffuse.Load() = ModelLoader::TextureFromFile(material->diffuse.path);

		int width, height, nrComponents;
		if (std::filesystem::path{ path }.extension().string() == ".dds") {
			//textureID = texture_loadDDS(filePath.c_str());
		}
		else {
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 4);
			if (data) {
				glGenTextures(1, &texture->id);

				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 2)
					format = GL_RG;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;
				format = GL_SRGB8_ALPHA8;
				glBindTexture(GL_TEXTURE_2D, texture->id);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				GLenum error = glGetError();
				if (error != GL_NO_ERROR) {
					std::cerr << "OpenGL error after glTexImage2D: " << error << std::endl;
				}
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else {
				std::cout << "Texture failed to load at path: " << path << std::endl;
				stbi_image_free(data);
			}
		}
		return texture;
	}

	void OpenGLRenderer::DrawRenderGroupInstanced(RenderGroup* renderGroup) {

	}

	Mesh* OpenGLRenderer::RestartMesh(Mesh* mesh) {
		return nullptr;
	}
}