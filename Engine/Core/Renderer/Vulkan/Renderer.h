#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Shaders/Shader.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Components/Core/Entity.h"
#include "Engine/Core/Renderer/Vulkan/ShadersCompiler.h"

#include "Mesh.h"

struct VertexV {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexV);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexV, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexV, color);
		return attributeDescriptions;
	}
};

namespace Plaza {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanRenderer : public Renderer {
	public:
		RendererAPI api = RendererAPI::Vulkan;
		void Init() override;
		void InitShaders(std::string shadersFolder) override;
		void AddInstancesToRender() override;
		void RenderShadowMap(Shader& shader) override;
		void RenderInstances(Shader& shader) override;
		void RenderBloom() override;
		void RenderScreenSpaceReflections() override;
		void RenderFullScreenQuad() override;
		void RenderOutline() override;
		void RenderHDR() override;
		void Destroy() override;
		void CopyLastFramebufferToFinalDrawBuffer() override;
		void InitGUI() override;
		void NewFrameGUI() override;
		void UpdateGUI() override;

		bool mFramebufferResized = false;
	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t mCurrentFrame = 0;

		bool mEnableValidationLayers = true;
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		bool isDeviceSuitable(VkPhysicalDevice device);
		void InitVulkan();
		void SetupDebugMessenger();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void InitSurface();
		void InitSwapChain();
		void CreateImageViews();
		void InitCommands();
		void InitSyncStructures();
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void CleanupSwapChain();
		void RecreateSwapChain();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void UpdateUniformBuffer(uint32_t currentImage);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CreateDescriptorSetLayout();

		VkInstance mVulkanInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkSwapchainKHR mSwapChain;

		VkPipeline mGraphicsPipeline;
		VkRenderPass mRenderPass;
		VkPipelineLayout mPipelineLayout;
		VkCommandPool mCommandPool;
		std::vector<VkCommandBuffer> mCommandBuffers;
		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;


		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainExtent;

		VkBuffer mVertexBuffer;
		VkDeviceMemory mVertexBufferMemory;
		VkBuffer mIndexBuffer;
		VkDeviceMemory mIndexBufferMemory;
		VkDescriptorSetLayout mDescriptorSetLayout;
		std::vector<VkDescriptorSet> mDescriptorSets;

		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;
		std::vector<void*> mUniformBuffersMapped;
		VkDescriptorPool mDescriptorPool;

		std::vector<VkImage> mSwapChainImages;
		std::vector<VkImageView> mSwapChainImageViews;
		std::vector<VkFramebuffer> mSwapChainFramebuffers;

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}

		struct UniformBufferObject {
			glm::mat4 projection;
			glm::mat4 view;
			glm::mat4 model;
		};

		const std::vector<VertexV> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0
		};
	};
	/*
	class VulkanRenderer : public Renderer {
	public:
		static Shader* mergeShader;
		static Shader* blurShader;
		static FrameBuffer* hdrFramebuffer;
		static FrameBuffer* bloomBlurFrameBuffer;
		static FrameBuffer* bloomFrameBuffer;
		/// <summary>
		/// Init the HDR framebuffer and others
		/// </summary>
		void Init();
		/// <summary>
		/// Loops through the GameObjects list, if Entity haves a MeshRenderer then it draws it with its transformation
		/// </summary>
		/// <param name="shader"></param>
		void Render(Shader& shader);

		/// <summary>
		/// Loops  through the meshes list of the active scene and Draw all instanced meshes
		/// </summary>
		void RenderInstances(Shader& shader);
		void RenderInstancesShadowMap(Shader& shader);

		/// <summary>
		/// Renders the Outline of the Selected Entity and its children
		/// </summary>
		/// <param name="outlineShader"></param>
		static void RenderOutline(Shader outlineShader);

		/// <summary>
		/// Renders a fullscreen quad with HDR using the scene color buffer
		/// </summary>
		static void RenderHDR();

		/// <summary>
		/// Renders a fullscreen quad blurring all fragments that its color exceeds a clamp, then merges it into the scene buffer
		/// </summary>
		static void RenderBloom();

		/// <summary>
		/// <p>Renders a full quad on the screen with a Shader</p>
		/// <p>Optimal for Post-Processing</p>
		/// </summary>
		static void RenderQuadOnScreen();

		static unsigned int quadVAO;
		static unsigned int quadVBO;
		static void InitQuad();

		static void BlurBuffer(GLint colorBuffer, int passes);
		/// <summary>
		/// Merge two textures
		/// </summary>
		/// <param name="outBuffer"></param>
		/// <param name="colorBuffer"></param>
		static void MergeColors(GLint texture1, GLint texture2);

		static void CopyFrameBufferColor(GLint readBuffer, GLint drawBuffer);
	private:
		static unsigned int pingpongFBO[2];
		static unsigned int pingpongColorbuffers[2];
	};
	*/
}

