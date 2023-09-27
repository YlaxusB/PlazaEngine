#pragma once
#include "UI.h"

namespace Plaza::Drawing::UI {
	struct Character {
		unsigned int TextureID;
		glm::ivec2   Size;     
		glm::ivec2   Bearing;  
		unsigned int Advance;
	};
	class TextRenderer {
	public:
		std::map<char, Character> characters;
		float xPos, yPos, scale;
		void Init(std::string fontPath);
		void Render();
		void Terminate();
	};

}