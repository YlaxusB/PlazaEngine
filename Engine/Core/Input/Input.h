#pragma once
#include "Editor/GUI/GuiWindow.h"

namespace Plaza {
	class Input {
	public:
		static inline bool isAnyKeyPressed = false;
		class Cursor;
		static void Update();
		static void GetKeyDown();
		static void GetKeyDownOnce();
		static void GetKeyReleased();
		static void GetKeyReleasedOnce();
		static void AddFunctionToOnKeyPress(std::function<void()> function, Editor::GuiLayer layer = Editor::GuiLayer::SCENE, Editor::GuiState layerState = Editor::GuiState::FOCUSED) {

		}



	private:

	};
}