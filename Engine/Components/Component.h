#pragma once
#include <string>
#include <vector>
#include "Engine/Core/UUID.h"
#include "Engine/Core/Engine.h"

namespace Plaza {
	class PLAZA_API Entity;
	class PLAZA_API Component {
	public:
		uint64_t mUuid;
		bool mEnabled = true;

		inline bool IsEnabled() { return mEnabled; };
		inline void SetEnabled(bool value) { mEnabled = value; };

		Entity* GetGameObject();
		virtual ~Component() = default;
		Component() {
			//this->componentUuid = Plaza::UUID::NewUUID();
		}
		virtual void OnInstantiate(Component* componentToInstantiate) {};

		template <class Archive>
		void serialize(Archive& archive) {
			archive(PL_SER(mUuid), PL_SER(mEnabled));
		}
	};
}