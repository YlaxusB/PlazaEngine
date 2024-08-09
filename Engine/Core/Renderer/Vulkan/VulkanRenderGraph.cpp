#include "Engine/Core/PreCompiledHeaders.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_vulkan.h"

namespace Plaza {
	void VulkanRenderGraph::BuildDefaultRenderGraph() {
		PlazaImageUsage inImageUsageFlags = static_cast<PlazaImageUsage>(PL_IMAGE_USAGE_COLOR_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC);
		PlazaImageUsage outImageUsageFlags = static_cast<PlazaImageUsage>(PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_COLOR_ATTACHMENT);

		this->AddTexture(make_shared<VulkanTexture>(VulkanRenderer::GetRenderer()->mMaxBindlessTextures, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(1, 1, 1), 1, 1, "TexturesBuffer"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "samplerBRDFLUT"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(512, 512, 1), 1, 6, "prefilterMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(64, 64, 1), 1, 6, "irradianceMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "shadowsDepthMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GPosition"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GDiffuse"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GNormal"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GOthers"));
		this->AddTexture(make_shared<VulkanTexture>(1, static_cast<PlazaImageUsage>(PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED), PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_D32_SFLOAT_S8_UINT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "SceneDepth"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "SceneTexture"));

		//uint64_t descriptorCount, PlazaImageUsage imageUsage, PlazaTextureType imageType, PlazaViewType viewType, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name

		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Geometry Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 0, PlazaBufferType::PL_BUFFER_UNIFORM_BUFFER, PL_STAGE_ALL, 1, sizeof(VulkanRenderer::UniformBufferObject), 2, "UniformBufferObject"))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(VulkanRenderer::GetRenderer()->mMaxBindlessTextures, 0, 20, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("TexturesBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 19, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, 1024 * 16, sizeof(VulkanRenderer::MaterialData), 2, "materialsBuffer"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 1, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, 1024 * 16, sizeof(glm::mat4), 2, "boneMatrices"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 2, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, 1024 * 256 * 64, sizeof(unsigned int), 2, "renderGroupOffsetsBuffer"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 3, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, 1024 * 256 * 64, sizeof(unsigned int), 2, "renderGroupMaterialsOffsetsBuffer"))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 6, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("samplerBRDFLUT")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 7, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("prefilterMap")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 8, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("irradianceMap")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 9, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("shadowsDepthMap")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GPosition")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 1, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GNormal")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 2, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GDiffuse")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 3, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GOthers")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 4, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SceneDepth")));


		//uint64_t descriptorCount, uint8_t location, uint8_t binding, PlazaBufferType bufferType, PlazaRenderStage renderStage, PlazaImageLayout initialLayout,

		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("UniformBufferObject")
			->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_UNIFORM_BUFFER)
			->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("materialsBuffer")
			->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
			->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("renderGroupOffsetsBuffer")
			->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
			->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("renderGroupMaterialsOffsetsBuffer")
			->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
			->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("boneMatrices")
			->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
			->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);

		// --------------- TEMPORARY
		//std::shared_ptr<VulkanTextureBinding> preFilterMap = std::make_shared<VulkanTextureBinding>(1, 0, 7, PL_BUFFER_COMBINED_IMAGE_SAMPLER, static_cast<PlazaImageUsage>(inImageUsageFlags), PL_STAGE_FRAGMENT, PL_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(512, 512, 1), 1, 1, "prefilterMap");
		//glm::vec2 size = glm::vec2(512, 512);
		//
		//preFilterMap->mTexture = std::make_shared<VulkanTexture>();
		//preFilterMap->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, VK_FORMAT_R32G32B32A32_SFLOAT, size.x, size.y, false, PlImageUsageToVkImageUsage(static_cast<PlazaImageUsage>(inImageUsageFlags)),
		//	VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, false, VK_SHARING_MODE_EXCLUSIVE, true);
		//preFilterMap->GetTexture()->CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, 6);
		//preFilterMap->GetTexture()->CreateTextureSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);

		//std::shared_ptr<VulkanTextureBinding> irradianceMap = std::make_shared<VulkanTextureBinding>(1, 0, 8, PL_BUFFER_COMBINED_IMAGE_SAMPLER, static_cast<PlazaImageUsage>(inImageUsageFlags), PL_STAGE_FRAGMENT, PL_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(64, 64, 1), 1, 1, "irradianceMap");
		//irradianceMap->mTexture = std::make_shared<VulkanTexture>();
		//irradianceMap->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, VK_FORMAT_R32G32B32A32_SFLOAT, 64, 64, false, PlImageUsageToVkImageUsage(static_cast<PlazaImageUsage>(inImageUsageFlags)),
		//	VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, false, VK_SHARING_MODE_EXCLUSIVE, false);
		//irradianceMap->GetTexture()->CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, 6);
		//irradianceMap->GetTexture()->CreateTextureSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);


		//this->GetRenderPass("Deferred Geometry Pass")->AddInputResource(preFilterMap);
		//this->GetRenderPass("Deferred Geometry Pass")->AddInputResource(irradianceMap);
		// --------------- TEMPORARY

		this->GetRenderPass("Deferred Geometry Pass")->mInputBindings.at("TexturesBuffer")->mMaxBindlessResources = VulkanRenderer::GetRenderer()->mMaxBindlessTextures;

		//uint64_t descriptorCount, uint8_t location, uint8_t binding, PlazaBufferType bufferType, PlazaRenderStage renderStage, PlazaImageLayout initialLayout, std::shared_ptr<Texture> texture
		//uint64_t descriptorCount, PlazaImageUsage imageUsage, PlazaTextureType type, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name

		//this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture ("GPosition")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 1, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture ("GDiffuse")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 2, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture ("GNormal")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 3, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture ("GOthers")))
		//	->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SceneTexture")));

			//->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, static_cast<PlazaImageUsage>(inImageUsageFlags), PL_STAGE_FRAGMENT, PL_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "SceneTexture"));
	}

	void VulkanBufferBinding::Compile() {
		mBuffer = std::make_shared<PlVkBuffer>();
		mBuffer->CreateBuffer(mMaxItems * mStride, PlBufferUsageToVkBufferUsage(this->GetBufferUsage()), PlMemoryUsageToVmaMemoryUsage(this->GetMemoryUsage()), 0, Application->mRenderer->mMaxFramesInFlight);
		mBuffer->CreateMemory(0, Application->mRenderer->mMaxFramesInFlight);
	}
	void VulkanBufferBinding::Destroy() {

	}

	void VulkanTextureBinding::Compile() {
		if (mMaxBindlessResources > 0)
			return;
		//mTexture = std::make_shared<VulkanTexture>();

		VkImageUsageFlags flags = 0;
		if (mTexture->mViewType == PL_VIEW_TYPE_CUBE)
			flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		this->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, PlImageFormatToVkFormat(mTexture->mFormat), mTexture->mResolution.x, mTexture->mResolution.y,
			false, PlImageUsageToVkImageUsage(mTexture->mImageUsage), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, mTexture->mLayersCount, flags, false);

		this->GetTexture()->CreateTextureSampler();

		VkImageAspectFlagBits aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		if (this->GetTexture()->mImageUsage & PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		this->GetTexture()->CreateImageView(PlImageFormatToVkFormat(mTexture->mFormat), aspect, PlViewTypeToVkImageViewType(mTexture->mViewType), mTexture->mLayersCount, 0);

		VulkanRenderer::GetRenderer()->AddTrackerToImage(this->GetTexture()->mImageView, mTexture->mName, this->GetTexture()->mSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	void VulkanTextureBinding::Destroy() {

	}

	void VulkanRenderPass::Compile() {
		glm::vec2 biggestSize = glm::vec2(0.0f);
		std::vector<VkImageView> frameBufferAttachments{};
		// Render Pass
		std::vector<VkSubpassDescription> subPasses{};
		std::vector<VkSubpassDependency> dependencies{};
		std::vector<VkAttachmentDescription> attachmentDescs{};
		std::vector<VkAttachmentReference> colorReferences;
		VkAttachmentReference depthReference = {};

		for (const auto& [key, value] : mOutputBindings) {
			VulkanTextureBinding* binding = static_cast<VulkanTextureBinding*>(value.get());

			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			if (value->mName == "SceneDepth")
				finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachmentDescs.push_back(
				plvk::attachmentDescription(PlImageFormatToVkFormat(binding->mTexture->mFormat),
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					finalLayout));//binding->mTexture->GetLayout()));

			frameBufferAttachments.push_back(binding->GetTexture()->mImageView);

			if (binding->GetTexture()->mImageUsage & PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) {
				depthReference.attachment = binding->mLocation;
				depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				continue;
			}
			colorReferences.push_back({ binding->mLocation, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

			biggestSize = glm::max(biggestSize, glm::vec2(binding->mTexture->mResolution));
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pColorAttachments = colorReferences.data();
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
		if (depthReference.layout != VK_IMAGE_LAYOUT_UNDEFINED)
			subpass.pDepthStencilAttachment = &depthReference;
		dependencies.push_back(
			plvk::subpassDependency(VK_SUBPASS_EXTERNAL,
				0,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
				0));
		dependencies.push_back(plvk::subpassDependency(
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			0));
		subPasses.push_back(subpass);
		mRenderPass = VulkanRenderer::GetRenderer()->CreateRenderPass(attachmentDescs.data(), attachmentDescs.size(), subPasses.data(), subPasses.size(), dependencies.data(), dependencies.size());

		// Frame buffer
		mFrameBuffer = VulkanRenderer::GetRenderer()->CreateFramebuffer(mRenderPass, biggestSize, frameBufferAttachments.data(), frameBufferAttachments.size(), 1);

		// Descriptor sets
		std::vector<VkDescriptorSetLayoutBinding> descriptorSets{};

		VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		std::vector<VkDescriptorBindingFlagsEXT> bindingFlags = std::vector<VkDescriptorBindingFlagsEXT>();
		for (const auto& [key, value] : mInputBindings) {
			switch (value->mBindingType) {
			case PL_BINDING_BUFFER:
				descriptorSets.push_back(static_cast<VulkanBufferBinding*>(value.get())->GetDescriptorLayout());
				bindingFlags.push_back(value->mMaxBindlessResources > 0 ? bindlessFlags : 0);
				break;
			case PL_BINDING_TEXTURE:
				descriptorSets.push_back(static_cast<VulkanTextureBinding*>(value.get())->GetDescriptorLayout());
				bindingFlags.push_back(value->mMaxBindlessResources > 0 ? bindlessFlags : 0);
				break;
			}
		}

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,	nullptr };
		extendedInfo.pBindingFlags = bindingFlags.data();
		extendedInfo.bindingCount = static_cast<uint32_t>(descriptorSets.size());

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = plvk::descriptorSetLayoutCreateInfo(descriptorSets, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT, &extendedInfo);

		vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout);

		std::vector<VkDescriptorSetLayout> layouts(Application->mRenderer->mMaxFramesInFlight,
			mDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
		};
		allocInfo.descriptorPool = VulkanRenderer::GetRenderer()->mDescriptorPool;
		allocInfo.descriptorSetCount = layouts.size();
		allocInfo.pSetLayouts = layouts.data();

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT
		};
		static const uint32_t maxBindlessResources = 16536;
		std::vector<uint32_t> maxBinding(Application->mRenderer->mMaxFramesInFlight,
			maxBindlessResources - 1);
		countInfo.descriptorSetCount = Application->mRenderer->mMaxFramesInFlight;
		countInfo.pDescriptorCounts = maxBinding.data();
		//allocInfo.pNext = &countInfo;
		mDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		VkResult res = vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, mDescriptorSets.data());
		if (res != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		for (unsigned int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
			std::vector<VkWriteDescriptorSet> descriptorWrites{};

			/*
						if (mType == PL_BUFFER_PUSH_CONSTANTS)
				return {};

			VkDescriptorBufferInfo bufferInfo = plvk::descriptorBufferInfo(mBuffer, 0, mStride);
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mType), mDescriptorCount, nullptr, &bufferInfo);
			VkDescriptorImageInfo imageInfo = plvk::descriptorImageInfo(mTexture.get()->GetLayout(), mTexture.get()->mImageView, mTexture.get()->mSampler);
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mBufferType), mDescriptorCount, &imageInfo, nullptr);
			*/
			std::vector<VkDescriptorBufferInfo*> bufferInfos{};
			std::vector<VkDescriptorImageInfo*> imageInfos{};
			for (const auto& [key, value] : mInputBindings) {
				switch (value->mBindingType) {
				case PL_BINDING_BUFFER: {
					VulkanBufferBinding* bufferBinding = static_cast<VulkanBufferBinding*>(value.get());
					VkDescriptorBufferInfo bufferInfo = bufferBinding->GetBufferInfo(Application->mRenderer->mCurrentFrame);
					bufferInfos.push_back(new VkDescriptorBufferInfo(bufferInfo));
					descriptorWrites.push_back(bufferBinding->GetDescriptorWrite(mDescriptorSets[i], bufferInfos[bufferInfos.size() - 1]));
					break;
				}
				case PL_BINDING_TEXTURE: {
					VulkanTextureBinding* textureBinding = static_cast<VulkanTextureBinding*>(value.get());
					VkDescriptorImageInfo imageInfo = textureBinding->GetImageInfo();
					if (value->mMaxBindlessResources > 0) {
						imageInfo = plvk::descriptorImageInfo(VK_IMAGE_LAYOUT_UNDEFINED, VK_NULL_HANDLE, VK_NULL_HANDLE);
						break;
					}
					imageInfos.push_back(new VkDescriptorImageInfo(imageInfo));
					descriptorWrites.push_back(textureBinding->GetDescriptorWrite(mDescriptorSets[i], imageInfos[imageInfos.size() - 1]));
					break;
				}
				}
			}

			vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice,
				static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr);
		}
	}

	void VulkanRenderPass::BindRenderPass() {
		std::array<VkClearValue, 5> clearValues{};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[2].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[3].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[4].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(this->mRenderPass, this->mFrameBuffer,
			Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, 0, clearValues.size(), clearValues.data());
		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	bool VulkanRenderGraph::BindPass(std::string passName) {
		if (mPasses.find(passName) == mPasses.end())
			return false;
		VulkanRenderPass* renderPass = this->GetRenderPass(passName);

		renderPass->BindRenderPass();
	}

	void VulkanRenderGraph::Execute(uint8_t imageIndex, uint8_t currentFrame) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		VkCommandBuffer commandBuffer = *mCommandBuffer;


		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		VulkanRenderer::GetRenderer()->mActiveCommandBuffer = &commandBuffer;
		//VulkanRenderer::GetRenderer()->TransitionImageLayout(VulkanRenderer::GetRenderer()->mSwapChainImages[currentFrame], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		for (auto& [key, value] : mPasses) {
			this->GetRenderPass(key)->UpdateCommandBuffer(commandBuffer);
			this->GetRenderPass(key)->Execute(this);
		}

		/* Render ImGui if in Editor build */
#ifdef EDITOR_MODE
		std::array<VkClearValue, 1> clearValues{};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(VulkanRenderer::GetRenderer()->mSwapchainRenderPass, VulkanRenderer::GetRenderer()->mSwapChainFramebuffers[imageIndex],
			VulkanRenderer::GetRenderer()->mSwapChainExtent.width, VulkanRenderer::GetRenderer()->mSwapChainExtent.height, 0, 0, 1, clearValues.data());

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		//vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanRenderer::GetRenderer()->mGraphicsPipeline);

		VkViewport viewport = plvk::viewport(0.0f, 0.0f, VulkanRenderer::GetRenderer()->mSwapChainExtent.width, VulkanRenderer::GetRenderer()->mSwapChainExtent.height);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor = plvk::rect2D(0, 0, VulkanRenderer::GetRenderer()->mSwapChainExtent.width, VulkanRenderer::GetRenderer()->mSwapChainExtent.height);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		//vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanRenderer::GetRenderer()->mPipelineLayout, 0, 1, &VulkanRenderer::GetRenderer()->mFinalSceneDescriptorSet, 0, nullptr);

		{
			PLAZA_PROFILE_SECTION("Render ImGui");
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		}

		vkCmdEndRenderPass(commandBuffer);
#endif

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}