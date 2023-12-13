#pragma once
namespace Plaza {
	class Light : public Component {
	public:
		glm::vec3 color = glm::vec3(0.7f, 1.0f, 1.0f);
		float radius = 1.0f;
		float cutoff = 1.0f;
		float intensity = 1.0f;
	};
}