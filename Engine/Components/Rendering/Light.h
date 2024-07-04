#pragma once
namespace Plaza {
	class Light : public Component {
	public:
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float radius = 1.0f;
		float cutoff = 1.0f;
		float intensity = 1.0f;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Component>(this), color, radius, cutoff, intensity);
		}
	};
}