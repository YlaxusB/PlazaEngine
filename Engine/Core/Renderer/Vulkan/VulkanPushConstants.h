#pragma once
#include "Engine/Core/Renderer/PushConstants.h"

namespace Plaza {
	class PlVkPushConstants : public PlPushConstants {
		PlVkPushConstants(PlRenderStage stage, uint32_t offset, uint32_t stride) : PlPushConstants(stage, offset, stride) { };

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<PlPushConstants>(this));
		}
	};
}

PL_SER_REGISTER_TYPE(PlVkPushConstants);
PL_SER_REGISTER_POLYMORPHIC_RELATION(PlPushConstants, PlVkPushConstants);