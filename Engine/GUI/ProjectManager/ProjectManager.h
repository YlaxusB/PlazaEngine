#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
namespace Engine {
	namespace Editor {
		class ProjectManagerGui {
		public:
			class ProjectItem;

			ProjectManagerGui() {

			}
			void Init();
			void Update();
			void SetupDockspace();
			void SetupMenuBar();
			void SetupProjectsTreeNode();

			void NewProjectButton();

			void NewProjectClick();
		};
	}
}