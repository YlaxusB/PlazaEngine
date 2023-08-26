#include "Engine/Core/PreCompiledHeaders.h"
#include "Camera.h"
namespace Plaza {
	Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	void Camera::Update() {
		if (!this->isEditorCamera)
			Position = Application->activeScene->transformComponents.at(this->uuid).GetWorldPosition();
		updateCameraVectors();
	}

	glm::mat4 Camera::GetProjectionMatrix() {
		return glm::perspective(glm::radians(this->Zoom), (Application->appSizes->sceneSize.x / Application->appSizes->sceneSize.y), nearPlane, farPlane);
	}

	glm::mat4 Camera::GetProjectionMatrix(float nearPlaneCustom, float farPlaneCustom) {
		nearPlaneCustom = nearPlaneCustom == NULL ? nearPlane : nearPlaneCustom;
		farPlaneCustom = farPlaneCustom == NULL ? nearPlane : farPlaneCustom;
		return glm::perspective(this->Zoom, (Application->appSizes->sceneSize.x / Application->appSizes->sceneSize.y), nearPlaneCustom, farPlaneCustom);
	}

	void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * MovementSpeedTemporaryBoost * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == UP)
			Position -= Up * velocity;
		if (direction == DOWN)
			Position += Up * velocity;

	}

	void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();

		UpdateFrustum();
	}

	void Camera::UpdateFrustum() {
		glm::mat4 viewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
		glm::vec4 leftPlane = viewProjectionMatrix[3] + viewProjectionMatrix[0];
		glm::vec4 rightPlane = viewProjectionMatrix[3] - viewProjectionMatrix[0];
		glm::vec4 bottomPlane = viewProjectionMatrix[3] + viewProjectionMatrix[1];
		glm::vec4 topPlane = viewProjectionMatrix[3] - viewProjectionMatrix[1];
		glm::vec4 nearPlaneFrustum = viewProjectionMatrix[3] + viewProjectionMatrix[2];
		glm::vec4 farPlaneFrustum = viewProjectionMatrix[3] - viewProjectionMatrix[2];

		// Normalize the planes
		frustum.leftPlane = glm::normalize(leftPlane);
		frustum.rightPlane = glm::normalize(rightPlane);
		frustum.bottomPlane = glm::normalize(bottomPlane);
		frustum.topPlane = glm::normalize(topPlane);
		frustum.nearPlaneFrustum = glm::normalize(nearPlaneFrustum);
		frustum.farPlaneFrustum = glm::normalize(farPlaneFrustum);
	}
	bool Camera::IsInsideViewFrustum(glm::vec3 pos) {
		glm::vec4 objectPos = glm::vec4(pos, 1.0f);
		return true; /// TEMPORARILY DISABLED FRUSTUM CULLING -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		return
			(glm::dot(objectPos, frustum.leftPlane) > 0.0f) &&
			(glm::dot(objectPos, frustum.rightPlane) > 0.0f) &&
			(glm::dot(objectPos, frustum.bottomPlane) > 0.0f) &&
			(glm::dot(objectPos, frustum.topPlane) > 0.0f) &&
			(glm::dot(objectPos, frustum.nearPlaneFrustum) > 0.0f) &&
			(glm::dot(objectPos, frustum.farPlaneFrustum) > 0.0f);
	}

	std::vector<glm::vec4> Camera::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
	{
		const auto inv = glm::inverse(proj * view);

		std::vector<glm::vec4> frustumCorners;
		for (unsigned int x = 0; x < 2; ++x)
		{
			for (unsigned int y = 0; y < 2; ++y)
			{
				for (unsigned int z = 0; z < 2; ++z)
				{
					const glm::vec4 pt =
						inv * glm::vec4(
							2.0f * x - 1.0f,
							2.0f * y - 1.0f,
							2.0f * z - 1.0f,
							1.0f);
					frustumCorners.push_back(pt / pt.w);
				}
			}
		}

		return frustumCorners;
	};

	void Camera::updateCameraVectors()
	{
		if (this->isEditorCamera) {
			// calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
		else {
			if (this->uuid && Application && Application->activeScene->transformComponents.find(this->uuid) != Application->activeScene->transformComponents.end()) {
				Transform* transform = &Application->activeScene->transformComponents.at(this->uuid);
				glm::mat4 transformationMatrix = transform->GetTransform(); // Assuming you have a transformation matrix

				glm::vec3 upVector = glm::normalize(glm::vec3(transformationMatrix[1])); // The second column is the up vector
				glm::vec3 frontVector = -glm::normalize(glm::vec3(transformationMatrix[2])); // The third column is the negative forward vector
				glm::vec3 rightVector = glm::normalize(glm::vec3(transformationMatrix[0])); // The first column is the right vector

				Front = glm::normalize(frontVector);
				// also re-calculate the Right and Up vector
				Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
				Up = glm::normalize(glm::cross(Right, Front));
			}
		}
		/*
		if (!this->isEditorCamera && this->uuid && Application) {
			if (Application->activeScene->transformComponents.find(this->uuid) != Application->activeScene->transformComponents.end()) {
				Transform* transform = &Application->activeScene->transformComponents.at(this->uuid);
				transform->rotation = GetEulerAnglesIgnoreY();
				transform->UpdateChildrenTransform();
			}
		}*/
	};
}