#pragma once
namespace Plaza {
	class Light : public Component {
	public:
		glm::vec4 color = glm::vec4(0.7f, 1.0f, 1.0f, 1.0f);
		float radius = 1.0f;
		float cutoff = 1.0f;
	};
}