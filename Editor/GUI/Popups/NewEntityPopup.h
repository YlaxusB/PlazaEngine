#pragma once
#include "Popup.h"
namespace Engine::Editor {
	class Popup::NewEntityPopup {
	public:
		static void Init(GameObject* obj, GameObject* parent = nullptr);
	};
}