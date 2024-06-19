#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Components/Core/Entity.h"
#include "Engine/Core/Renderer/Vulkan/ShadersCompiler.h"

#include "Mesh.h"
#include "Engine/Core/Renderer/Mesh.h"

#include "VulkanTexture.h"
#include "VulkanShadows.h"
#include "VulkanLighting.h"
#include "VulkanPicking.h"
#include "VulkanComputeShaders.h"
#include "VulkanBloom.h"

namespace Plaza {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanRenderer : public Renderer {
	public:
		std::array<int64_t, MAX_BONE_INFLUENCE> GetBoneIds(std::vector<unsigned int> bones);

		std::map<uint64_t, Bone> mBones = std::map<uint64_t, Bone>();
		VkSemaphore semaphore;

		struct PushConstants {
			glm::vec4 color = glm::vec4(1.0f);
			float intensity = 1.0f;
			int diffuseIndex = -1;
			int normalIndex = -1;
			int roughnessIndex = -1;
			int metalnessIndex = -1;
			float roughnessFloat = 0.5f;
			float metalnessFloat = 0.5f;
		};
		struct UniformBufferObject {
			glm::mat4 projection = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 model = glm::mat4(1.0f);
			int cascadeCount = 0;
			float farPlane = 0.0f;
			float nearPlane = 0.0f;
			alignas(16) glm::vec4 lightDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			alignas(16) glm::vec4 viewPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			glm::mat4 lightSpaceMatrices[16] = { glm::mat4(2.0f) };
			glm::vec4 cascadePlaneDistances[16] = { glm::vec4(1.0f) };
			alignas(16) glm::vec4 directionalLightColor;
			alignas(16) glm::vec4 ambientLightColor;
			bool showCascadeLevels;
		};

		void Init() override;
		void UpdateProjectManager() override;
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
		ImTextureID GetFrameImage() override;

		void AddTrackerToImage(
			VkImageView imageView,
			std::string name = "",
			VkSampler textureSampler = VK_NULL_HANDLE,
			VkImageLayout layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);

		VulkanShadows* GetShadows() override;

		Mesh& CreateNewMesh(
			vector<glm::vec3> vertices,
			vector<glm::vec3> normals,
			vector<glm::vec2> uvs,
			vector<glm::vec3> tangent,
			vector<glm::vec3> bitangent,
			vector<unsigned int> indices,
			vector<unsigned int> materialsIndices,
			bool usingNormal,
			vector<BonesHolder> bonesHolder = vector<BonesHolder>(),
			vector<Bone> uniqueBonesInfo = vector<Bone>()) override;
		Mesh* RestartMesh(Mesh* mesh);
		void DrawRenderGroupInstanced(RenderGroup* renderGroup);
		void DrawRenderGroupShadowDepthMapInstanced(RenderGroup* renderGroup, unsigned int cascade);

		Texture* LoadTexture(std::string path, uint64_t uuid = 0) override;
		Texture* LoadImGuiTexture(std::string path) override;

		bool mFramebufferResized = false;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat FindDepthFormat();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, unsigned int layerCount = 1, unsigned int mipCount = 1);
		void TransitionTextureLayout(VulkanTexture& texture, VkImageLayout newLayout, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, unsigned int layerCount = 1, unsigned int mipCount = 1);

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevel = 0, unsigned int arrayLayerCount = 1);
		VkCommandBuffer* mActiveCommandBuffer;

		void ChangeFinalDescriptorImageView(VkImageView newImageView);
		VkFormat mFinalDeferredFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		VkFormat mSwapChainImageFormat;
		std::vector<VkDescriptorSet> mSwapchainDescriptorSets = std::vector<VkDescriptorSet>();
		VkImage mFinalSceneImage;
		VkImageView mFinalSceneImageView;
		VkImage mDeferredFinalImage;
		VkImageView mDeferredFinalImageView;

		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkFramebuffer mFinalSceneFramebuffer;
		VkFramebuffer mDeferredFramebuffer;
		VkExtent2D mSwapChainExtent;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mComputeQueue = VK_NULL_HANDLE;
		std::vector<VkFence> mInFlightFences;
		VkImageView mDepthImageView;

		void AddMaterial(Material* material);
		void UpdateMaterials();

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize offset = 0);
		void CopyImage(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize offset = 0);
		VkSampler mTextureSampler;
		std::vector<VkFence> mComputeInFlightFences;

		static VulkanRenderer* GetRenderer();


		RendererAPI api = RendererAPI::Vulkan;
		VulkanShadows* mShadows;
		VulkanLighting* mLighting;
		VulkanBloom mBloom;
		VkDevice mDevice = VK_NULL_HANDLE;
		VkDescriptorPool mDescriptorPool;
		VkRenderPass mRenderPass;
		VkRenderPass mDeferredRenderPass;
		VkRenderPass mSwapchainRenderPass;
		VulkanPlazaPipeline mSwapchainRenderer;

		VulkanPlazaPipeline mGeometryPassRenderer;

		VulkanTexture mDeferredPositionTexture;
		VulkanTexture mDeferredNormalTexture;
		VulkanTexture mDeferredDiffuseTexture;
		VulkanTexture mDeferredOthersTexture;
	private:
		struct SwapChainPushConstant {
			float exposure = 1.2f;
			float gamma = 2.6f;
		};

		VkDescriptorSetLayout mSwapchainDescriptorSetLayout = VK_NULL_HANDLE;

		struct alignas(16) MaterialData {
			glm::vec4 color = glm::vec4(1.0f);
			float intensity = 1.0f;
			int diffuseIndex = -1;
			int normalIndex = -1;
			int roughnessIndex = -1;
			int metalnessIndex = -1;
			float roughnessFloat = 0.5f;
			float metalnessFloat = 0.5f;
		};

		const std::string MODEL_PATH = "C:\\Users\\Giovane\\Desktop\\Workspace\\viking_room.obj";
		const std::string TEXTURE_PATH = "C:\\Users\\Giovane\\Desktop\\Workspace\\viking_room.png";

		uint32_t mCurrentImage;
		const int MAX_FRAMES_IN_FLIGHT = 2;

		bool mEnableValidationLayers = true;
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR& capabilities);
		bool isDeviceSuitable(VkPhysicalDevice device);
		void InitVulkan();
		void SetupDebugMessenger();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void InitSurface();
		void InitSwapChain();
		void CreateImageViews(VkImageLayout initialLayout);
		void InitCommands();
		void InitSyncStructures();
		void CreateRenderPass();
		void CreateSwapchainRenderPass();
		void CreateGraphicsPipeline();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateImGuiTextureSampler();

		void CleanupSwapChain();
		void RecreateSwapChain();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		float time = 0.0f;
		bool increasing = true;
		void CalculateIndividualBone(Bone* bone, glm::mat4 target, int time, uint64_t boneId);
		void CalculateBonesParentship(Bone* bone, glm::mat4 parentTransform);
		void EarlyAnimationController();
		void CreateVertexBuffer(vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory, VkDeviceSize bufferSize = -1);
		void CreateIndexBuffer(vector<uint32_t> indices, VkBuffer& indicesBuffer, VkDeviceMemory& indicesMemoryBuffer, VkDeviceSize bufferSize = -1);
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void UpdateUniformBuffer(uint32_t currentImage);
		void CreateDescriptorSetLayout();
		void InitializeGeometryPassRenderer();

		void CreateTextureImage();
		VkDescriptorSet GetGeometryPassDescriptorSet(unsigned int frame);

		VkCommandBuffer CreateCommandBuffer();

		void CreateTextureImageView();
		void CreateTextureSampler();
		VkImage mTextureImage;
		VkDeviceMemory mTextureImageMemory;
		VkImageView mTextureImageView;


		VkDescriptorSet mFinalSceneDescriptorSet;
		VkInstance mVulkanInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkSwapchainKHR mSwapChain;

		VkPipeline mGraphicsPipeline;
		VkPipelineLayout mPipelineLayout;
		VkCommandPool mCommandPool;
		std::vector<VkCommandBuffer> mCommandBuffers;
		std::vector<VkCommandBuffer> mComputeCommandBuffers;

		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkSemaphore> mComputeFinishedSemaphores;


		void InitComputeCommandBuffers();
		void InitComputeSemaphores();
		void InitComputeInFlightFences();


		VkBuffer mVertexBuffer;
		VkDeviceMemory mVertexBufferMemory;
		VkBuffer mIndexBuffer;
		VkDeviceMemory mIndexBufferMemory;
		VkDescriptorSetLayout mDescriptorSetLayout;
		std::vector<VkDescriptorSet> mDescriptorSets;
		std::vector<VkDescriptorSet> mSwapPassDescriptorSets;

		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;
		std::vector<void*> mUniformBuffersMapped;

		std::vector<VkImage> mSwapChainImages;
		std::vector<VkImageView> mSwapChainImageViews;
		std::vector<VkFramebuffer> mSwapChainFramebuffers;

		VkDescriptorSet mMainSceneDescriptorSet;

		VkBuffer mMainIndirectCommandsBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mMainIndirectCommandsBufferMemory = VK_NULL_HANDLE;
		VkBuffer mMainVertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mMainVertexBufferMemory = VK_NULL_HANDLE;
		VkBuffer mMainIndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mMainIndexBufferMemory = VK_NULL_HANDLE;

		std::vector<VkBuffer> mMainInstanceMatrixBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mMainInstanceMatrixBufferMemories = std::vector<VkDeviceMemory>();

		std::vector<VkBuffer> mMainInstanceMaterialBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mMainInstanceMaterialBufferMemories = std::vector<VkDeviceMemory>();

		std::vector<VkBuffer> mMainInstanceMaterialOffsetsBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mMainInstanceMaterialOffsetsBufferMemories = std::vector<VkDeviceMemory>();

		std::vector<VkBuffer> mMainInstanceRenderGroupOffsetsBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mMainInstanceRenderGroupOffsetsBufferMemories = std::vector<VkDeviceMemory>();

		std::vector<VkBuffer> mBoneMatricesBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mBoneMatricesBufferMemories = std::vector<VkDeviceMemory>();
		std::vector<glm::mat4> mInstanceModelMatrices = std::vector<glm::mat4>();
		std::vector<std::vector<unsigned int>> mInstanceModelMaterialOffsets = std::vector<std::vector<unsigned int>>();
		std::vector<unsigned int> mInstanceModelMaterialsIndex = std::vector<unsigned int>();

		std::vector<MaterialData> mUploadedMaterials = std::vector<MaterialData>();
		std::unordered_map<uint64_t, unsigned int> mMaterialsHandles = std::unordered_map<uint64_t, unsigned int>();
		std::vector<VkBuffer> mMaterialBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mMaterialBufferMemories = std::vector<VkDeviceMemory>();

		uint64_t mBufferTotalVertices = 0;
		uint64_t mBufferTotalIndices = 0;
		uint64_t mIndirectDrawCount = 0;
		uint64_t mTotalInstances = 0;
		std::vector<VkDrawIndexedIndirectCommand> mIndirectCommands = std::vector<VkDrawIndexedIndirectCommand>();

		std::vector<VkBuffer> mIndirectBuffers = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> mIndirectBufferMemories = std::vector<VkDeviceMemory>();

		// ImGui variables
		VkDescriptorPool mImguiDescriptorPool;
		VkDescriptorSetLayout mImguiDescriptorSetLayout;
		VkDescriptorSet mImguiDescriptorSet;
		VkPipeline mImguiPipeline;
		VkPipelineLayout mImguiPipelineLayout;
		VkImage mImguiImage;


		VkImage mDepthImage;
		VkDeviceMemory mDepthImageMemory;
		void CreateDepthResources();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		bool HasStencilComponent(VkFormat format);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}



		/*
			mat4 projection;
mat4 view;
mat4 model;
int cascadeCount;
float cascadePlaneDistances[32];
mat4 lightSpaceMatrices[32];
float farPlane;
float nearPlane;
vec3 lightDirection;
vec3 viewPos;
	*/

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		struct VertexMaterialsOffsets {
			std::vector<unsigned int> renderGroupOffsets = std::vector<unsigned int>();
			std::vector<unsigned int> renderGroupMaterialsOffsets = std::vector<unsigned int>();
		};

		static std::array<VkVertexInputBindingDescription, 2> VertexGetBindingDescription() {
			std::array<VkVertexInputBindingDescription, 2> bindingDescriptions = {};
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputBindingDescription instanceBindingDescription = {};
			instanceBindingDescription.binding = 1;
			instanceBindingDescription.stride = sizeof(glm::vec4) * 4;
			instanceBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

			bindingDescriptions[1] = instanceBindingDescription;

			//VkVertexInputBindingDescription materialBindingDescription = {};
			//materialBindingDescription.binding = 2;
			//materialBindingDescription.stride = 64 * sizeof(unsigned int);
			//materialBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
			//bindingDescriptions[2] = materialBindingDescription;
			return bindingDescriptions;
		}

		static std::array<VkVertexInputAttributeDescription, 12> VertexGetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 12> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, normal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex, tangent);

			attributeDescriptions[4].binding = 0;
			attributeDescriptions[4].location = 4;
			attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[4].offset = offsetof(Vertex, bitangent);

			attributeDescriptions[5].binding = 1;
			attributeDescriptions[5].location = 5;
			attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[5].offset = 0;

			attributeDescriptions[6].binding = 1;
			attributeDescriptions[6].location = 6;
			attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[6].offset = sizeof(float) * 4;

			attributeDescriptions[7].binding = 1;
			attributeDescriptions[7].location = 7;
			attributeDescriptions[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[7].offset = sizeof(float) * 8;

			attributeDescriptions[8].binding = 1;
			attributeDescriptions[8].location = 8;
			attributeDescriptions[8].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[8].offset = sizeof(float) * 12;

			attributeDescriptions[8].binding = 1;
			attributeDescriptions[8].location = 8;
			attributeDescriptions[8].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[8].offset = sizeof(float) * 12;

			attributeDescriptions[9].binding = 0;
			attributeDescriptions[9].location = 9;
			attributeDescriptions[9].format = VK_FORMAT_R32G32B32A32_SINT;
			attributeDescriptions[9].offset = offsetof(Vertex, boneIds);

			attributeDescriptions[10].binding = 0;
			attributeDescriptions[10].location = 10;
			attributeDescriptions[10].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[10].offset = offsetof(Vertex, weights);

			//attributeDescriptions[11].binding = 2;
			//attributeDescriptions[11].location = 18;
			//attributeDescriptions[11].format = VK_FORMAT_R32_UINT;
			//attributeDescriptions[11].offset = sizeof(unsigned int);

			// Material
			attributeDescriptions[11].binding = 0;
			attributeDescriptions[11].location = 11;
			attributeDescriptions[11].format = VK_FORMAT_R32_UINT;
			attributeDescriptions[11].offset = offsetof(Vertex, materialIndex);
			return attributeDescriptions;
		}

		friend class VulkanMesh;
		friend class VulkanTexture;
		friend class VulkanGuiRenderer;
		friend class VulkanPicking;
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

