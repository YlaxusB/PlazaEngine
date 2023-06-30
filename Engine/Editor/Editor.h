#pragma once
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/GUI/guiMain.h"
namespace Engine {
	namespace Editor
	{
		static GameObject* selectedGameObject;
	}
}
inline GameObject* selectedGameObject = nullptr;