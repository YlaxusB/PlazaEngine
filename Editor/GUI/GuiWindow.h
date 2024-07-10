#pragma once

namespace Plaza {
	namespace Editor {
		enum GuiLayer {
			UNKNOWN = 0,
			HIERARCHY,
			SCENE,
			EDITOR,
			INSPECTOR,
			FILE_EXPLORER,
			ASSETS_IMPORTER
		};

		enum class GuiState {
			UNKNOWN = 0,
			FOCUSED,
			UNFOCUSED,
			HOVERED
		};

		class GuiWindow {
		public:
			GuiWindow(GuiLayer layer, bool startOpen = true) : mLayer(layer), mIsOpen(startOpen) {
				
			}
			virtual void Init() = 0;
			virtual void Update() = 0;
			virtual void OnKeyPress(int key, int scancode, int action, int mods) = 0;

			//virtual void HandleKeyEvent();
			//virtual void HandleFocusedKeyEvent();
			//virtual void HandleHoveredKeyEvent();

			void IsHovered();
			void IsFocused();

			GuiLayer GetLayer() {
				return mLayer;
			}
			bool IsOpen() {
				return mIsOpen;
			}
			void SetOpen(bool value) {
				mIsOpen = value;
			}

			bool mExpanded = true;
		private:
			GuiLayer mLayer = GuiLayer::UNKNOWN;
			GuiState mState = GuiState::UNKNOWN;
			bool mIsOpen = true;
		};
	}
}