#pragma once
#include "Engine/Components/Core/CoreComponents.h"
namespace Engine {
	class Editor
	{
	public:
		static GameObject* selectedGameObject;
	};
}
inline GameObject* Engine::Editor::selectedGameObject = nullptr;