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

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(glm::mat4) * instanceModelMatrices.size();
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		void* data;
		vkMapMemory(GetVulkanRenderer().mDevice, mInstanceBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, instanceModelMatrices.data(), static_cast<size_t>(bufferInfo.size));
		vkUnmapMemory(GetVulkanRenderer().mDevice, mInstanceBufferMemory);

		VkDeviceSize offsets[] = { 0 };
		VkCommandBuffer activeCommandBuffer = *GetVulkanRenderer().mActiveCommandBuffer;
		VkDescriptorSet descriptorSets[] = { GetVulkanRenderer().mDescriptorSets[GetVulkanRenderer().mCurrentFrame] };
		vkCmdBindDescriptorSets(activeCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetVulkanRenderer().mPipelineLayout, 0, 1, &GetVulkanRenderer().mDescriptorSets[GetVulkanRenderer().mCurrentFrame], 0, nullptr);
		vector<VkBuffer> verticesBuffer = { mVertexBuffer, mInstanceBuffer };
		vkCmdBindVertexBuffers(activeCommandBuffer, 0, 1, &mVertexBuffer, offsets);
		vkCmdBindVertexBuffers(activeCommandBuffer, 1, 1, &mInstanceBuffer, offsets);
		vkCmdBindIndexBuffer(activeCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(activeCommandBuffer, static_cast<uint32_t>(indices.size()), instanceModelMatrices.size(), 0, 0, 0);
		instanceModelMatrices.clear();
	}

	VulkanRenderer& VulkanMesh::GetVulkanRenderer() {
		return *(VulkanRenderer*)Application->mRenderer;
	}

	VulkanMesh::~VulkanMesh() {
		VkDevice device = GetVulkanRenderer().mDevice;
		vkDestroyBuffer(device, mIndexBuffer, nullptr);
		vkFreeMemory(device, mIndexBufferMemory, nullptr);

		vkDestroyBuffer(device, mVertexBuffer, nullptr);
		vkFreeMemory(device, mVertexBufferMemory, nullptr);

		vkDestroyBuffer(device, mInstanceBuffer, nullptr);
		vkFreeMemory(device, mInstanceBufferMemory, nullptr);
	};

	void VulkanMesh::CreateInstanceBuffer() {
		for (int i = 0; i < 8; ++i) {
			this->instanceModelMatrices.push_back(glm::mat4(13.0f));
		}
		VkDeviceSize bufferSize = 8 * sizeof(glm::mat4);

		//VkBuffer stagingBuffer;
		//VkDeviceMemory stagingBufferMemory;
		GetVulkanRenderer().CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mInstanceBuffer, mInstanceBufferMemory);
		//void* data;
		//vkMapMemory(GetVulkanRenderer().mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		//memcpy(data, this->instanceModelMatrices.data(), (size_t)bufferSize);
		//vkUnmapMemory(GetVulkanRenderer().mDevice, stagingBufferMemory);
		//
		//GetVulkanRenderer().CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mInstanceBuffer, mInstanceBufferMemory);
		//
		//GetVulkanRenderer().CopyBuffer(stagingBuffer, mInstanceBuffer, bufferSize);
		//
		//vkDestroyBuffer(GetVulkanRenderer().mDevice, stagingBuffer, nullptr);
		//vkFreeMemory(GetVulkanRenderer().mDevice, stagingBufferMemory, nullptr);
	}

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
		this->CreateInstanceBuffer();
	}

	void VulkanMesh::Restart() {

	}
}