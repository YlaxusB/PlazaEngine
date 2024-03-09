#include "Engine/Core/PreCompiledHeaders.h"
#include "VulkanShaders.h"

namespace Plaza {
	static VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}


	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	static std::array<VkVertexInputAttributeDescription, 5> VertexGetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 1;
		attributeDescriptions[1].location = 5;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = 0;

		attributeDescriptions[2].binding = 1;
		attributeDescriptions[2].location = 6;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = sizeof(float) * 4;

		attributeDescriptions[3].binding = 1;
		attributeDescriptions[3].location = 7;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[3].offset = sizeof(float) * 8;

		attributeDescriptions[4].binding = 1;
		attributeDescriptions[4].location = 8;
		attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[4].offset = sizeof(float) * 12;
		return attributeDescriptions;
	}

	void VulkanShaders::Init(VkDevice device, VkRenderPass renderPass, int width, int height, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo, std::vector<VkPushConstantRange> pushConstantRanges, bool useVertexInputInfo) {
		auto vertShaderCode = readFile(mVertexShaderPath);
		auto fragShaderCode = readFile(mFragmentShaderPath);
		//auto geomShaderCode = readFile(mGeometryShaderPath);

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);
		//VkShaderModule geomShaderModule = createShaderModule(geomShaderCode, device);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();

		if (!mVertexShaderPath.empty())
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";
			shaderStages.push_back(vertShaderStageInfo);
		}

		if (!mFragmentShaderPath.empty())
		{
			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";
			shaderStages.push_back(fragShaderStageInfo);
		}

		//f (!mGeometryShaderPath.empty())
		//
		//	VkPipelineShaderStageCreateInfo geomShaderStageInfo{};
		//	geomShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//	geomShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		//	geomShaderStageInfo.module = geomShaderModule;
		//	geomShaderStageInfo.pName = "main";
		//	shaderStages.push_back(geomShaderStageInfo);
		//

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = (float)width;
		scissor.extent.height = (float)height;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;

		//rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		//rasterizer.depthBiasEnable = VK_FALSE;
		//rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		//rasterizer.depthBiasClamp = 0.0f; // Optional
		//rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		if (!useVertexInputInfo)
		{
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthStencil.depthTestEnable = VK_TRUE;
		//depthStencil.depthWriteEnable = VK_TRUE;
		//depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		//depthStencil.depthBoundsTestEnable = VK_FALSE;
		//depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		if (useVertexInputInfo)
		{
			// Populate VkVertexInputBindingDescription (if needed)
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0; // Assuming the binding is 0
			bindingDescription.stride = sizeof(glm::vec3); // Adjust the stride based on your vertex data structure
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // or VK_VERTEX_INPUT_RATE_INSTANCE if using instancing

			std::array<VkVertexInputBindingDescription, 2> bindingDescriptions = {};
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputBindingDescription instanceBindingDescription = {};
			instanceBindingDescription.binding = 1;
			instanceBindingDescription.stride = sizeof(glm::vec4) * 4;
			instanceBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

			std::array<VkVertexInputAttributeDescription, 4> instanceAttributeDescriptions = {};
			instanceAttributeDescriptions[0].binding = 1;
			instanceAttributeDescriptions[0].location = 5;
			instanceAttributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			instanceAttributeDescriptions[0].offset = 0;

			instanceAttributeDescriptions[1].binding = 1;
			instanceAttributeDescriptions[1].location = 6;
			instanceAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			instanceAttributeDescriptions[1].offset = sizeof(float) * 4;

			instanceAttributeDescriptions[2].binding = 1;
			instanceAttributeDescriptions[2].location = 7;
			instanceAttributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			instanceAttributeDescriptions[2].offset = sizeof(float) * 8;

			instanceAttributeDescriptions[3].binding = 1;
			instanceAttributeDescriptions[3].location = 8;
			instanceAttributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			instanceAttributeDescriptions[3].offset = sizeof(float) * 12;
			bindingDescriptions[1] = instanceBindingDescription;

			auto attributeDescriptions = VertexGetAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
			vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		}
		else
		{
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
		}

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		colorBlending.attachmentCount = useVertexInputInfo ? 1 : 1;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthStencil.depthTestEnable = VK_TRUE;
		//depthStencil.depthWriteEnable = VK_TRUE;
		//depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		//depthStencil.back.compareOp = VK_COMPARE_OP_ALWAYS;

		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;

		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = this->mPipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.pDepthStencilState = &depthStencil;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->mPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		//vkDestroyShaderModule(device, geomShaderModule, nullptr);
	}
}