#pragma once
#include <Engine/Core/Renderer/Lighting.h>
#include "VulkanPostEffects.h"
#include "VulkanComputeShaders.h"

namespace Plaza {
	class VulkanLighting : public Lighting {
	public:
		VulkanComputeShaders mLightSorterComputeShaders;
		VulkanPostEffects* mDeferredPassRenderer;
		VulkanShaders* mDeferredPass = nullptr;

		void Init() override;
		void GetLights() override;
		void UpdateTiles() override;
		void DrawDeferredPass();
		void Terminate() override;

	private:
		struct LightSorterPushConstants {
			glm::mat4 view;
			glm::mat4 projection;
			int lightCount;
			bool first;
			glm::vec2 screenSize;
			glm::vec2 clusterSize;
		};
		void InitializeDescriptorSets();
		void InitializeBuffers();
		
		std::vector<VkBuffer> mLightsBuffer;
		std::vector<VkDeviceMemory> mLightsBufferMemory;
		std::vector<VkBuffer> mTilesBuffer;
		std::vector<VkDeviceMemory> mTilesBufferMemory;
		std::vector<VkBuffer> mDepthValuesBuffer;
		std::vector<VkDeviceMemory> mDepthValuesBufferMemory;
	};
}