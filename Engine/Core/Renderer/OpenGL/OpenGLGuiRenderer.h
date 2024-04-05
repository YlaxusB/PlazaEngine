#pragma once
#include <Engine/Core/Renderer/GuiRenderer.h>

namespace Plaza {
	class OpenGLGuiRenderer : public GuiRenderer {
	public:
		void Init() {};
		void RenderText(Drawing::UI::TextRenderer* textRendererComponent) {};
		void Terminate() {};
	private:

	};
}