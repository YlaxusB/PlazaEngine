#pragma once
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Renderer.h"

namespace Plaza {
	class GuiRenderer {
	public:
		virtual void Init() = 0;
		virtual void RenderText(Drawing::UI::TextRenderer* textRendererComponent) = 0;
		virtual void Terminate() = 0;
	};
}