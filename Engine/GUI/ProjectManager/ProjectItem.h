#pragma once
#include "Engine/GUI/ProjectManager/ProjectManager.h"

namespace Engine {
	namespace Editor {
		class ProjectManagerGui::ProjectItem {
		public:
			std::string name;
			std::string path;
			void Setup();
			void OnClick();
			ProjectItem(std::string name, std::string path) : name(name), path(path) {
				Setup();
			}
		};
	}
}