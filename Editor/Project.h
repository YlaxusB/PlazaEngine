#pragma once
#include "Engine/Core/Script.h"
namespace Plaza {
	namespace Editor {
		class Project {
		public:
			std::string name;
			std::string directory;

			std::string creationDate;
			std::string lastModifyDate;

			vector<Script> scripts;
		};
	}
}