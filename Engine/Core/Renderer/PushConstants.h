#pragma once 
namespace Plaza {
	class PlPushConstants {
	public:
		PlPushConstants(PlRenderStage stage, uint32_t offset, uint32_t stride) : mStage(stage), mOffset(offset), mStride(stride) { };
		PlRenderStage mStage = PL_STAGE_ALL;
		uint32_t mOffset = 0;
		uint32_t mStride = 0;
		void* mData = nullptr;
	};
}