#pragma once
#include "UI.h"

namespace Plaza::Drawing::UI {
	struct Character {
		unsigned int TextureID;
		glm::ivec2   Size;     
		glm::ivec2   Bearing;  
		unsigned int Advance;
	};
	class TextRenderer : public Component {
	public:
		std::map<GLchar, Character> mCharacters;
		std::string mText;
		unsigned int mVAO, mVBO;
		void Init(std::string fontPath);
		void Render(Shader& shader, float x, float y, float scale, glm::vec4 color);
		void Terminate();
	};

}