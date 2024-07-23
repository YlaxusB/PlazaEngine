#include "VulkanPlazaWrapper.h"
#include "Renderer.h"

namespace Plaza{
	VmaAllocator PlBuffer::GetVmaAllocator() {
			return VulkanRenderer::GetRenderer()->mVmaAllocator;
	}
}