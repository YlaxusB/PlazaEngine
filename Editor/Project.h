#pragma once
#include "Engine/Core/Script.h"
#include "ThirdParty/mono/include/mono/jit/jit.h"
#include "ThirdParty/mono/include/mono/metadata/assembly.h"
#include "Engine/Core/Scripting/Script.h"
#include "Engine/Core/AssetsManager/Asset.h"
namespace Plaza {
	namespace Editor {
		class Project : public Asset {
		public:
			uint64_t mLastSceneUuid = 0;
			std::string creationDate = "";
			std::string lastModifyDate = "";

			static void Load(const std::string filePath);

			std::map<std::string, Script> scripts;
			std::map<std::string, MonoObject*> monoObjects;

			template <class Archive>
			void serialize(Archive& archive) {
				archive(mAssetUuid, mAssetName, creationDate, lastModifyDate, mLastSceneUuid);
			}
		};
	}
}