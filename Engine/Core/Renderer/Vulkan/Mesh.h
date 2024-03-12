#pragma once
#include "Engine/Core/Renderer/Mesh.h"
#include "Renderer.h"

namespace Plaza {
	class VulkanMesh : public Mesh {
	public:
		struct InstanceBuffer {
			VkBuffer buffer{ VK_NULL_HANDLE };
			VkDeviceMemory memory{ VK_NULL_HANDLE };
			size_t size = 0;
			VkDescriptorBufferInfo descriptor{ VK_NULL_HANDLE };
		} instanceBuffer;

		RendererAPI api = RendererAPI::Vulkan;

		~VulkanMesh();

		VulkanMesh(const VulkanMesh&) = default;
		VulkanMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<glm::vec3> bitangent, vector<unsigned int> indices) {
			this->vertices = vertices;
			this->indices = indices;
			this->normals = normals;
			this->uvs = uvs;
			this->tangent = tangent;
			this->bitangent = bitangent;
			this->uuid = Plaza::UUID::NewUUID();
			if (this->meshId == 0)
				this->meshId = Plaza::UUID::NewUUID();
			//setupMesh();
		}

		//VulkanMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<glm::vec3> bitangent, vector<unsigned int> indices) {
		//	this->vertices = vertices;
		//	this->indices = indices;
		//	this->normals = normals;
		//	this->uvs = uvs;
		//	this->tangent = tangent;
		//	this->bitangent = bitangent;
		//	this->indices = indices;
		//	this->uuid = Plaza::UUID::NewUUID();
		//	if (this->meshId == 0)
		//		this->meshId = Plaza::UUID::NewUUID();
		//	//setupMesh();
		//}

		VulkanMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<glm::vec3> bitangent, vector<unsigned int> indices, bool usingNormal) {
			this->vertices = vertices;
			this->indices = indices;
			this->normals = normals;
			this->uvs = uvs;
			this->tangent = tangent;
			this->bitangent = bitangent;
			this->indices = indices;
			this->usingNormal = usingNormal;
			this->uuid = Plaza::UUID::NewUUID();
			if (this->meshId == 0)
				this->meshId = Plaza::UUID::NewUUID();
			//setupMesh();
		}

		VulkanMesh() {
			uuid = Plaza::UUID::NewUUID();
		}

		void Draw(Shader& shader) override;
		void DrawInstances() override;
		void Drawe();
		void setupMesh() override;
		void Restart() override;

		bool firstRune = true;
		VkDeviceMemory mInstanceBufferMemory;
		VkBuffer mVertexBuffer;
		VkBuffer mIndexBuffer;
		VkBuffer mInstanceBuffer;
	private:
		VkDeviceMemory mVertexBufferMemory;
		VkDeviceMemory mIndexBufferMemory;
		VkDescriptorSet mDescriptorSet;

		VkBufferCreateInfo mBufferCreateInfo = {};

		friend class VulkanRenderer;

		void CreateInstanceBuffer();
		VulkanRenderer& GetVulkanRenderer();


		//void CreateVertexBuffer(vector<Vertex>) {
		//	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
		//
		//	VkBuffer stagingBuffer;
		//	VkDeviceMemory stagingBufferMemory;
		//	GetVulkanRenderer().CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | //VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		//
		//	void* data;
		//	vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		//	memcpy(data, vertices.data(), (size_t)bufferSize);
		//	vkUnmapMemory(mDevice, stagingBufferMemory);
		//
		//	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, //mVertexBuffer, mVertexBufferMemory);
		//
		//	CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);
		//
		//	vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		//	vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
		//}
	};
}

