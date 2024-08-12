#pragma once
#include "Engine/Core/Renderer/PushConstants.h"

namespace Plaza {
	class PlVkPushConstants : public PlPushConstants {
		PlVkPushConstants(PlRenderStage stage, uint32_t offset, uint32_t stride) : PlPushConstants(stage, offset, stride) { };
	};
}