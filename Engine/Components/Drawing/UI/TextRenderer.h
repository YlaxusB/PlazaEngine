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
		float mPosX = 0;
		float mPosY = 0;
		float mScale = 1.0f;
		glm::vec4 mColor = glm::vec4(1.0f);
		unsigned int mVAO, mVBO;
		TextRenderer();
		void Init(std::string fontPath);
		void Render(Shader& shader);
		void Terminate();

		void SetFullText(string text = "", float x = 0.0f, float y = 0.0f, float scale = 1.0f, glm::vec4 color = glm::vec4(1.0f)) {
			this->mPosX = x;
			this->mPosY = y;
			this->mScale = scale;
			this->mColor = color;
			this->mText = text;
		}
	};

}