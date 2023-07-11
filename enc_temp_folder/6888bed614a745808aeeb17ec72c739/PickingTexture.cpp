#include "Engine/Core/PreCompiledHeaders.h"
#include "PickingTexture.h"
void PickingTexture::generateTexture() {
	glDisable(GL_BLEND);
	Application->pickingTexture->enableWriting();
	//Application->pickingTexture->updateSize(Application->appSizes->sceneSize);
	glViewport(0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
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
	shader.setVec3("viewPos", Application->activeCamera->Position);
	for (MeshRenderer* meshRenderer : meshRenderers) {
		GameObject* gameObject = meshRenderer->gameObject;

		glm::mat4 modelMatrix = gameObject->transform->modelMatrix;
		shader.setMat4("model", modelMatrix);
		shader.setFloat("objectID", gameObject->id);

		meshRenderer->mesh->Draw(shader);
	}
}