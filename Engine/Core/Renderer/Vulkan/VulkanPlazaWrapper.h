#pragma once

namespace Plaza {
	class PlBuffer {
	public:
		VmaAllocator GetVmaAllocator();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage, VmaAllocationCreateFlags flags = 0) {
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocInfo.flags = flags;

			vmaCreateBuffer(GetVmaAllocator(), &bufferInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);
		}

		void Destroy() {
			vmaFreeMemory(GetVmaAllocator(), mAllocation);
			vmaDestroyBuffer(GetVmaAllocator(), mBuffer, mAllocation);
		}

		VkBuffer& GetBuffer() {
			return mBuffer;
		}
		VmaAllocation& GetAllocation() {
			return mAllocation;
		}
	private:
		VkBuffer mBuffer;
		VmaAllocation mAllocation;
	};
}