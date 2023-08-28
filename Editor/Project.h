#pragma once
#include "Engine/Core/Script.h"
#include "Engine/Vendor/mono/jit/jit.h"
#include "Engine/Vendor/mono/metadata/assembly.h"
namespace Plaza {
	namespace Editor {
		class Project {
		public:
			std::string name;
			std::string directory;

			std::string creationDate;
			std::string lastModifyDate;

			std::string scriptsConfigFilePath;

			std::map<std::string, std::chrono::system_clock::time_point> scripts;
			std::map<std::string, MonoObject*> monoObjects;
		};
	}
}