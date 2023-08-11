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
	for (const auto& gameObject : Application->activeScene->gameObjects) {
		MeshRenderer* meshRenderer = gameObject.get()->GetComponent<MeshRenderer>();
		if (meshRenderer) {
			Transform* transform = gameObject.get()->GetComponent<Transform>();

			if (Application->activeCamera->IsInsideViewFrustum(transform->worldPosition)) {
				glm::mat4 modelMatrix = transform->modelMatrix;
				shader.setMat4("model", modelMatrix);
				//shader.setFloat("objectID", gameObject.get()->id);

				meshRenderer->mesh->Draw(shader);
			}
		}
	}
}