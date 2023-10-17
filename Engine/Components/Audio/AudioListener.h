#pragma once
#include "Engine/Components/Component.h"
namespace Plaza {
	class AudioListener : public Component {
	public:
		glm::vec3 mPosition;
		glm::vec3 mOrientation;
		int volume = 100;
		void UpdateListener();
	};
}