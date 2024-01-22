#include "Engine/Core/PreCompiledHeaders.h"
#include "Mesh.h"
namespace Plaza {
	void VulkanMesh::Drawe() {


		if (firstRune)
		{
			vector<Vertex> convertedVertices;
			convertedVertices.reserve(vertices.size());

			for (unsigned int i = 0; i < vertices.size(); i++) {
				convertedVertices.push_back(Vertex{
					vertices[i],
					(normals.size() > i) ? normals[i] : glm::vec3(0.0f),
					(uvs.size() > i) ? uvs[i] : glm::vec2(0.0f),
					(tangent.size() > i) ? tangent[i] : glm::vec3(0.0f),
					(bitangent.size() > i) ? bitangent[i] : glm::vec3(0.0f)
					});
			}
			GetVulkanRenderer().CreateVertexBuffer(convertedVertices, mVertexBuffer, mVertexBufferMemory);

			GetVulkanRenderer().CreateIndexBuffer(indices, mIndexBuffer, mIndexBufferMemory);
			firstRune = false;
		}

		if (!mIndexBuffer)
		{

		}

		VkBuffer vertexBuffers[] = { mVertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		//vkCmdBindVertexBuffers(*((VulkanRenderer*)(Application->mRenderer))->mActiveCommandBuffer, 0, 1, { &mVertexBuffer }, offsets);
		//vkCmdBindIndexBuffer(*((VulkanRenderer*)(Application->mRenderer))->mActiveCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDrawIndexed(*((VulkanRenderer*)(Application->mRenderer))->mActiveCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void VulkanMesh::Draw(Shader& shader) {
		vkCmdBindVertexBuffers(*((VulkanRenderer*)(Application->mRenderer))->mActiveCommandBuffer, 0, 1, { &mVertexBuffer }, { 0 });
		vkCmdBindIndexBuffer(*((VulkanRenderer*)(Application->mRenderer))->mActiveCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(*((VulkanRenderer*)(Application->mRenderer))->mActiveCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);;
	}

	void VulkanMesh::DrawInstances() {
		VkDeviceSize offsets[] = { 0 };
		VkCommandBuffer activeCommandBuffer = *GetVulkanRenderer().mActiveCommandBuffer;
		vkCmdBindDescriptorSets(activeCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetVulkanRenderer().mPipelineLayout, 0, 1, &GetVulkanRenderer().mDescriptorSets[GetVulkanRenderer().mCurrentFrame], 0, nullptr);
		vkCmdBindVertexBuffers(activeCommandBuffer, 0, 1, { &mVertexBuffer }, offsets);
		vkCmdBindIndexBuffer(activeCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(activeCommandBuffer, static_cast<uint32_t>(indices.size()), instanceModelMatrices.size(), 0, 0, 0);
		instanceModelMatrices.clear();
	}

	VulkanRenderer& VulkanMesh::GetVulkanRenderer() {
		return *(VulkanRenderer*)Application->mRenderer;
	}

	VulkanMesh::~VulkanMesh() {
		// VkDevice device = GetVulkanRenderer().mDevice;
		// vkDestroyBuffer(device, mIndexBuffer, nullptr);
		// vkFreeMemory(device, mIndexBufferMemory, nullptr);
		// 
		// vkDestroyBuffer(device, mVertexBuffer, nullptr);
		// vkFreeMemory(device, mVertexBufferMemory, nullptr);
	};

	void VulkanMesh::setupMesh() {
		vector<Vertex> convertedVertices;
		convertedVertices.reserve(vertices.size());

		for (unsigned int i = 0; i < vertices.size(); i++) {
			convertedVertices.push_back(Vertex{
				vertices[i],
				(normals.size() > i) ? normals[i] : glm::vec3(0.0f),
				(uvs.size() > i) ? uvs[i] : glm::vec2(0.0f),
				(tangent.size() > i) ? tangent[i] : glm::vec3(0.0f),
				(bitangent.size() > i) ? bitangent[i] : glm::vec3(0.0f)
				});
		}

		GetVulkanRenderer().CreateVertexBuffer(convertedVertices, mVertexBuffer, mVertexBufferMemory);
		GetVulkanRenderer().CreateIndexBuffer(indices, mIndexBuffer, mIndexBufferMemory);
	}

	void VulkanMesh::Restart() {

	}
}