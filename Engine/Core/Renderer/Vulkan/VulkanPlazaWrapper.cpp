#include "VulkanPlazaWrapper.h"
#include "Renderer.h"

namespace Plaza{
	VmaAllocator PlVkBuffer::GetVmaAllocator() {
			return VulkanRenderer::GetRenderer()->mVmaAllocator;
	}
}