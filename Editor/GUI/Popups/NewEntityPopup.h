#pragma once
#include "Popup.h"
namespace Plaza::Editor {
	class Popup::NewEntityPopup {
	public:
		static void Init(Entity* obj, Entity* parent = nullptr);
	};
}