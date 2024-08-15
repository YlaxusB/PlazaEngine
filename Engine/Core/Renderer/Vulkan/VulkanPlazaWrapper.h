#pragma once
#include "ThirdParty/include/VulkanMemoryAllocator/vk_mem_alloc.h"

namespace Plaza {
	class PlVkBuffer : public PlBuffer {
	public:
		PlVkBuffer() {};
		PlVkBuffer(PlBufferType type, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, PlBufferUsage bufferUsage, PlMemoryUsage memoryUsage, const std::string& name)
			: PlBuffer(type, maxItems, stride, bufferCount, bufferUsage, memoryUsage, name) {};

		//uint64_t descriptorCount, uint8_t binding, PlBufferType type, PlRenderStage stage, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, const std::string& name

		VmaAllocator GetVmaAllocator();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage, VmaAllocationCreateFlags flags = 0, unsigned int buffersCount = 1) override {
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = vmaUsage;
			allocInfo.flags = flags;

			mBuffers.resize(buffersCount);
			mAllocations.resize(buffersCount);
			for (unsigned int i = 0; i < buffersCount; ++i) {
				vmaCreateBuffer(GetVmaAllocator(), &bufferInfo, &allocInfo, &mBuffers[i], &mAllocations[i], nullptr);
			}

			//vmaAllocateMemory(GetVmaAllocator());
		}

		void CreateMemory(VmaAllocationCreateFlags flags = 0, unsigned int buffersCount = 1) override {
			//VmaAllocationCreateInfo allocInfo = {};
			//allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			//allocInfo.flags = flags;
			//VmaAllocation allocation;
			//VmaAllocationInfo allocationInfo;
			//for (unsigned int i = 0; i < buffersCount; ++i) {
			//	vmaAllocateMemoryForBuffer(GetVmaAllocator(), mBuffers[i], &allocInfo, &allocation, &allocationInfo);
			//}
		}

		void Destroy() override {
			for (unsigned int i = 0; i < mBuffers.size(); ++i) {
				vmaFreeMemory(GetVmaAllocator(), mAllocations[i]);
				vmaDestroyBuffer(GetVmaAllocator(), mBuffers[i], mAllocations[i]);
			}
		}

		VkBuffer& GetBuffer(unsigned int index = 0) {
			return mBuffers[index];
		}
		VmaAllocation& GetAllocation(unsigned int index = 0) {
			return mAllocations[index];
		}

		VkDeviceMemory& GetMemory(unsigned int index = 0) {
			if (mMemories.size() < mBuffers.size()) {

			}
			return mMemories[index];
		}

	private:
		std::vector<VkBuffer> mBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mMemories = std::vector<VkDeviceMemory>();
		std::vector<VmaAllocation> mAllocations = std::vector<VmaAllocation>();

		void UpdateDataHelper(unsigned int index, const void* newData, size_t size) override {
			void* data;
			vmaMapMemory(GetVmaAllocator(), mAllocations[index], &data);
			memcpy(data, newData, size);
			vmaUnmapMemory(GetVmaAllocator(), mAllocations[index]);
		}
	};
}