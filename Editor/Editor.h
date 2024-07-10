#pragma once
#include <string>
#include <map>
#include "Engine/Core/Scripting/FieldManager.h"
#include "Editor/GUI/guiMain.h"
namespace Plaza {
	namespace Editor {
		class EditorClass {
		public:
			Gui mGui;
			std::string directory;
		};
	}
}