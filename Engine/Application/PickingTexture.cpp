#include "Engine/Core/PreCompiledHeaders.h"
#include "PickingTexture.h"

void PickingTexture::GenerateTexture() {
	glDisable(GL_BLEND);
	Application->pickingTexture->enableWriting();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Render with Picking Shader
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	Render(*Application->pickingShader);
	Application->pickingTexture->disableWriting();
	glEnable(GL_BLEND);
}


void PickingTexture::Render(Shader& shader) {
	shader.use();

	glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
	glm::mat4 view = Application->activeCamera->GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	for (auto& entity : Application->activeScene->entities) {
		MeshRenderer* meshRenderer = entity.second.GetComponent<MeshRenderer>();
		if (meshRenderer) {
			Transform* transform = entity.second.GetComponent<Transform>();

			if (Application->activeCamera->IsInsideViewFrustum(transform->worldPosition)) {
				glm::mat4 modelMatrix = transform->modelMatrix;
				shader.setMat4("model", modelMatrix);
				uint32_t lower32 = static_cast<uint32_t>(entity.first);
				uint32_t upper32 = static_cast<uint32_t>(entity.first >> 32);
				std::vector<uint32_t> uint32Values = { lower32, upper32 };
				glUniform1uiv(glGetUniformLocation(shader.ID, "uuidArray"), uint32Values.size(), uint32Values.data());

				//  if (meshRenderer->mesh)
				//  meshRenderer->mesh.Draw(shader);
			}
		}
	}
}