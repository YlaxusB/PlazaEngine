#pragma once
#include "Renderer.h"
#include "Engine/Core/Renderer/RenderGraph.h"
#include "VulkanPlazaPipeline.h"
#include "VulkanPlazaInitializator.h"
#include "VulkanTexture.h"
#include "VulkanPlazaWrapper.h"
namespace Plaza {
	static VkDescriptorType PlBufferTypeToVkDescriptorType(PlBufferType type) {
		VkDescriptorType convertedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		switch (type) {
		case PL_BUFFER_UNIFORM_BUFFER:
			convertedType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case PL_BUFFER_STORAGE_BUFFER:
			convertedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case PL_BUFFER_STORAGE_BUFFER_DYNAMIC:
			convertedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			break;
		case PL_BUFFER_SAMPLER:
			convertedType = VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		case PL_BUFFER_COMBINED_IMAGE_SAMPLER:
			convertedType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		}
		return convertedType;
	}

	static VkImageType PlTextureTypeToVkImageType(PlTextureType type) {
		VkImageType convertedType = VK_IMAGE_TYPE_2D;
		switch (type) {
		case PL_TYPE_1D:
			convertedType = VK_IMAGE_TYPE_1D;
			break;
		case PL_TYPE_2D:
			convertedType = VK_IMAGE_TYPE_2D;
			break;
		case PL_TYPE_3D:
			convertedType = VK_IMAGE_TYPE_3D;
			break;
		}
		return convertedType;
	}

	static VkImageViewType PlViewTypeToVkImageViewType(PlViewType type) {
		VkImageViewType convertedType = VK_IMAGE_VIEW_TYPE_1D;
		switch (type) {
		case PL_VIEW_TYPE_1D:
			convertedType = VK_IMAGE_VIEW_TYPE_1D;
			break;
		case PL_VIEW_TYPE_2D:
			convertedType = VK_IMAGE_VIEW_TYPE_2D;
			break;
		case PL_VIEW_TYPE_3D:
			convertedType = VK_IMAGE_VIEW_TYPE_3D;
			break;
		case PL_VIEW_TYPE_CUBE:
			convertedType = VK_IMAGE_VIEW_TYPE_CUBE;
			break;
		case PL_VIEW_TYPE_1D_ARRAY:
			convertedType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			break;
		case PL_VIEW_TYPE_2D_ARRAY:
			convertedType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			break;
		case PL_VIEW_TYPE_CUBE_ARRAY:
			convertedType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
			break;
		}
		return convertedType;
	}

	static VkFormat PlImageFormatToVkFormat(PlTextureFormat format) {
		VkFormat convertedFormat = VK_FORMAT_R8G8B8_UNORM;
		switch (format) {
		case PL_FORMAT_R32G32B32A32_SFLOAT:
			convertedFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		case PL_FORMAT_R32G32B32A32_SINT:
			convertedFormat = VK_FORMAT_R32G32B32A32_SINT;
			break;
		case PL_FORMAT_R32G32B32_SFLOAT:
			convertedFormat = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case PL_FORMAT_R32G32_SFLOAT:
			convertedFormat = VK_FORMAT_R32G32_SFLOAT;
			break;
		case PL_FORMAT_R32_SFLOAT:
			convertedFormat = VK_FORMAT_R32_SFLOAT;
			break;
		case PL_FORMAT_R32_UINT:
			convertedFormat = VK_FORMAT_R32_UINT;
			break;
		case PL_FORMAT_R8G8B8A8_UNORM:
			convertedFormat = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case PL_FORMAT_R8G8B8_UNORM:
			convertedFormat = VK_FORMAT_R8G8B8_UNORM;
			break;
		case PL_FORMAT_D32_SFLOAT_S8_UINT:
			convertedFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
			break;
		default:
			assert(false && "Unhandled PlazaTextureFormat case");
			break;
		}
		return convertedFormat;
	}

	static VkImageUsageFlags PlImageUsageToVkImageUsage(PlImageUsage imageUsage) {
		VkImageUsageFlags convertedUsage = 0;
		if (imageUsage & PL_IMAGE_USAGE_TRANSFER_SRC) convertedUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (imageUsage & PL_IMAGE_USAGE_TRANSFER_DST) convertedUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (imageUsage & PL_IMAGE_USAGE_SAMPLED) convertedUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if (imageUsage & PL_IMAGE_USAGE_STORAGE) convertedUsage |= VK_IMAGE_USAGE_STORAGE_BIT;
		if (imageUsage & PL_IMAGE_USAGE_COLOR_ATTACHMENT) convertedUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (imageUsage & PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) convertedUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if (imageUsage & PL_IMAGE_USAGE_INPUT_ATTACHMENT) convertedUsage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		return convertedUsage;
	}

	static VkShaderStageFlags PlRenderStageToVkShaderStage(PlRenderStage stage) {
		VkShaderStageFlags newStage{};
		if (stage & PL_STAGE_ALL) newStage = VK_SHADER_STAGE_ALL;
		if (stage & PL_STAGE_VERTEX) newStage |= VK_SHADER_STAGE_VERTEX_BIT;
		if (stage & PL_STAGE_FRAGMENT) newStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (stage & PL_STAGE_GEOMETRY) newStage |= VK_SHADER_STAGE_COMPUTE_BIT;
		if (stage & PL_STAGE_COMPUTE) newStage |= VK_SHADER_STAGE_COMPUTE_BIT;
		return newStage;
	}

	static VkMemoryPropertyFlags PlMemoryPropertyToVkMemoryProperty(PlMemoryProperty memoryProperties) {
		VkMemoryPropertyFlags convertedPropertyFlags = 0;

		if (memoryProperties & PL_MEMORY_PROPERTY_DEVICE_LOCAL) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_HOST_VISIBLE) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_HOST_COHERENT) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_HOST_CACHED) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_LAZILY_ALLOCATED) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_PROTECTED) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_PROTECTED_BIT;
		}

		return convertedPropertyFlags;
	}

	static VkBufferUsageFlags PlBufferUsageToVkBufferUsage(PlBufferUsage usage) {
		VkBufferUsageFlags convertedUsage{};
		if (usage & PL_BUFFER_USAGE_TRANSFER_SRC) {
			convertedUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}
		if (usage & PL_BUFFER_USAGE_TRANSFER_DST) {
			convertedUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		if (usage & PL_BUFFER_USAGE_UNIFORM_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_STORAGE_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_INDEX_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_VERTEX_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_INDIRECT_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		return convertedUsage;
	}

	static VmaMemoryUsage PlMemoryUsageToVmaMemoryUsage(PlMemoryUsage usage) {
		VmaMemoryUsage convertedUsage = VMA_MEMORY_USAGE_UNKNOWN;
		switch (usage) {
		case PL_MEMORY_USAGE_GPU_ONLY:
			convertedUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			break;
		case PL_MEMORY_USAGE_CPU_ONLY:
			convertedUsage = VMA_MEMORY_USAGE_CPU_ONLY;
			break;
		case PL_MEMORY_USAGE_CPU_TO_GPU:
			convertedUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			break;
		case PL_MEMORY_USAGE_GPU_TO_CPU:
			convertedUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
			break;
		case PL_MEMORY_USAGE_CPU_COPY:
			convertedUsage = VMA_MEMORY_USAGE_CPU_COPY;
			break;
		case PL_MEMORY_USAGE_GPU_LAZILY_ALLOCATED:
			convertedUsage = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
			break;
		case PL_MEMORY_USAGE_AUTO:
			convertedUsage = VMA_MEMORY_USAGE_AUTO;
			break;
		case PL_MEMORY_USAGE_AUTO_PREFER_DEVICE:
			convertedUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			break;
		case PL_MEMORY_USAGE_AUTO_PREFER_HOST:
			convertedUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
			break;
		}
		return convertedUsage;
	}

	static VkPrimitiveTopology PlTopologyToVkTopology(PlPrimitiveTopology topology) {
		switch (topology) {
		case PL_TOPOLOGY_TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case PL_TOPOLOGY_LINE_LIST: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case PL_TOPOLOGY_POINT_LIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	static VkPolygonMode PlPolygonModeToVkPolygonMode(PlPolygonMode mode) {
		switch (mode) {
		case PL_POLYGON_MODE_FILL: return VK_POLYGON_MODE_FILL;
		case PL_POLYGON_MODE_LINE: return VK_POLYGON_MODE_LINE;
		case PL_POLYGON_MODE_POINT: return VK_POLYGON_MODE_POINT;
		default: return VK_POLYGON_MODE_FILL;
		}
	}

	static VkCullModeFlags PlCullModeToVkCullMode(PlCullMode mode) {
		switch (mode) {
		case PL_CULL_MODE_NONE: return VK_CULL_MODE_NONE;
		case PL_CULL_MODE_FRONT: return VK_CULL_MODE_FRONT_BIT;
		case PL_CULL_MODE_BACK: return VK_CULL_MODE_BACK_BIT;
		case PL_CULL_MODE_FRONT_AND_BACK: return VK_CULL_MODE_FRONT_AND_BACK;
		default: return VK_CULL_MODE_BACK_BIT;
		}
	}

	static VkFrontFace PlFrontFaceToVkFrontFace(PlFrontFace face) {
		switch (face) {
		case PL_FRONT_FACE_COUNTER_CLOCKWISE: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case PL_FRONT_FACE_CLOCKWISE: return VK_FRONT_FACE_CLOCKWISE;
		default: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
	}

	static VkSampleCountFlagBits PlSampleCountToVkSampleCount(PlSampleCount sampleCount) {
		switch (sampleCount) {
		case PL_SAMPLE_COUNT_1_BIT: return VK_SAMPLE_COUNT_1_BIT;
		case PL_SAMPLE_COUNT_2_BIT: return VK_SAMPLE_COUNT_2_BIT;
		case PL_SAMPLE_COUNT_4_BIT: return VK_SAMPLE_COUNT_4_BIT;
		case PL_SAMPLE_COUNT_8_BIT: return VK_SAMPLE_COUNT_8_BIT;
		case PL_SAMPLE_COUNT_16_BIT: return VK_SAMPLE_COUNT_16_BIT;
		case PL_SAMPLE_COUNT_32_BIT: return VK_SAMPLE_COUNT_32_BIT;
		case PL_SAMPLE_COUNT_64_BIT: return VK_SAMPLE_COUNT_64_BIT;
		default: return VK_SAMPLE_COUNT_1_BIT;
		}
	}

	static VkCompareOp PlCompareOpToVkCompareOp(PlCompareOp compareOp) {
		switch (compareOp) {
		case PL_COMPARE_OP_NEVER: return VK_COMPARE_OP_NEVER;
		case PL_COMPARE_OP_LESS: return VK_COMPARE_OP_LESS;
		case PL_COMPARE_OP_EQUAL: return VK_COMPARE_OP_EQUAL;
		case PL_COMPARE_OP_LESS_OR_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
		case PL_COMPARE_OP_GREATER: return VK_COMPARE_OP_GREATER;
		case PL_COMPARE_OP_NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
		case PL_COMPARE_OP_GREATER_OR_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case PL_COMPARE_OP_ALWAYS: return VK_COMPARE_OP_ALWAYS;
		default: return VK_COMPARE_OP_LESS_OR_EQUAL;
		}
	}

	static VkBlendFactor PlBlendFactorToVkBlendFactor(PlBlendFactor factor) {
		switch (factor) {
		case PL_BLEND_FACTOR_ZERO: return VK_BLEND_FACTOR_ZERO;
		case PL_BLEND_FACTOR_ONE: return VK_BLEND_FACTOR_ONE;
		case PL_BLEND_FACTOR_SRC_COLOR: return VK_BLEND_FACTOR_SRC_COLOR;
		case PL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case PL_BLEND_FACTOR_DST_COLOR: return VK_BLEND_FACTOR_DST_COLOR;
		case PL_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case PL_BLEND_FACTOR_SRC_ALPHA: return VK_BLEND_FACTOR_SRC_ALPHA;
		case PL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case PL_BLEND_FACTOR_DST_ALPHA: return VK_BLEND_FACTOR_DST_ALPHA;
		case PL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case PL_BLEND_FACTOR_CONSTANT_COLOR: return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case PL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case PL_BLEND_FACTOR_CONSTANT_ALPHA: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case PL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case PL_BLEND_FACTOR_SRC_ALPHA_SATURATE: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case PL_BLEND_FACTOR_SRC1_COLOR: return VK_BLEND_FACTOR_SRC1_COLOR;
		case PL_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case PL_BLEND_FACTOR_SRC1_ALPHA: return VK_BLEND_FACTOR_SRC1_ALPHA;
		case PL_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default: return VK_BLEND_FACTOR_ZERO;
		}
	}

	static VkBlendOp PlBlendOpToVkBlendOp(PlBlendOp op) {
		switch (op) {
		case PL_BLEND_OP_ADD: return VK_BLEND_OP_ADD;
		case PL_BLEND_OP_SUBTRACT: return VK_BLEND_OP_SUBTRACT;
		case PL_BLEND_OP_REVERSE_SUBTRACT: return VK_BLEND_OP_REVERSE_SUBTRACT;
		case PL_BLEND_OP_MIN: return VK_BLEND_OP_MIN;
		case PL_BLEND_OP_MAX: return VK_BLEND_OP_MAX;
		case PL_BLEND_OP_ZERO_EXT: return static_cast<VkBlendOp>(1000148000);
		case PL_BLEND_OP_SRC_EXT: return static_cast<VkBlendOp>(1000148001);
		case PL_BLEND_OP_DST_EXT: return static_cast<VkBlendOp>(1000148002);
		case PL_BLEND_OP_SRC_OVER_EXT: return static_cast<VkBlendOp>(1000148003);
		case PL_BLEND_OP_DST_OVER_EXT: return static_cast<VkBlendOp>(1000148004);
		case PL_BLEND_OP_SRC_IN_EXT: return static_cast<VkBlendOp>(1000148005);
		case PL_BLEND_OP_DST_IN_EXT: return static_cast<VkBlendOp>(1000148006);
		case PL_BLEND_OP_SRC_OUT_EXT: return static_cast<VkBlendOp>(1000148007);
		case PL_BLEND_OP_DST_OUT_EXT: return static_cast<VkBlendOp>(1000148008);
		case PL_BLEND_OP_SRC_ATOP_EXT: return static_cast<VkBlendOp>(1000148009);
		case PL_BLEND_OP_DST_ATOP_EXT: return static_cast<VkBlendOp>(1000148010);
		case PL_BLEND_OP_XOR_EXT: return static_cast<VkBlendOp>(1000148011);
		case PL_BLEND_OP_MULTIPLY_EXT: return static_cast<VkBlendOp>(1000148012);
		case PL_BLEND_OP_SCREEN_EXT: return static_cast<VkBlendOp>(1000148013);
		case PL_BLEND_OP_OVERLAY_EXT: return static_cast<VkBlendOp>(1000148014);
		case PL_BLEND_OP_DARKEN_EXT: return static_cast<VkBlendOp>(1000148015);
		case PL_BLEND_OP_LIGHTEN_EXT: return static_cast<VkBlendOp>(1000148016);
		case PL_BLEND_OP_COLORDODGE_EXT: return static_cast<VkBlendOp>(1000148017);
		case PL_BLEND_OP_COLORBURN_EXT: return static_cast<VkBlendOp>(1000148018);
		case PL_BLEND_OP_HARDLIGHT_EXT: return static_cast<VkBlendOp>(1000148019);
		case PL_BLEND_OP_SOFTLIGHT_EXT: return static_cast<VkBlendOp>(1000148020);
		case PL_BLEND_OP_DIFFERENCE_EXT: return static_cast<VkBlendOp>(1000148021);
		case PL_BLEND_OP_EXCLUSION_EXT: return static_cast<VkBlendOp>(1000148022);
		case PL_BLEND_OP_INVERT_EXT: return static_cast<VkBlendOp>(1000148023);
		case PL_BLEND_OP_INVERT_RGB_EXT: return static_cast<VkBlendOp>(1000148024);
		case PL_BLEND_OP_LINEARDODGE_EXT: return static_cast<VkBlendOp>(1000148025);
		case PL_BLEND_OP_LINEARBURN_EXT: return static_cast<VkBlendOp>(1000148026);
		case PL_BLEND_OP_VIVIDLIGHT_EXT: return static_cast<VkBlendOp>(1000148027);
		case PL_BLEND_OP_LINEARLIGHT_EXT: return static_cast<VkBlendOp>(1000148028);
		case PL_BLEND_OP_PINLIGHT_EXT: return static_cast<VkBlendOp>(1000148029);
		case PL_BLEND_OP_HARDMIX_EXT: return static_cast<VkBlendOp>(1000148030);
		case PL_BLEND_OP_HSL_HUE_EXT: return static_cast<VkBlendOp>(1000148031);
		case PL_BLEND_OP_HSL_SATURATION_EXT: return static_cast<VkBlendOp>(1000148032);
		case PL_BLEND_OP_HSL_COLOR_EXT: return static_cast<VkBlendOp>(1000148033);
		case PL_BLEND_OP_HSL_LUMINOSITY_EXT: return static_cast<VkBlendOp>(1000148034);
		case PL_BLEND_OP_PLUS_EXT: return static_cast<VkBlendOp>(1000148035);
		case PL_BLEND_OP_PLUS_CLAMPED_EXT: return static_cast<VkBlendOp>(1000148036);
		case PL_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT: return static_cast<VkBlendOp>(1000148037);
		case PL_BLEND_OP_PLUS_DARKER_EXT: return static_cast<VkBlendOp>(1000148038);
		case PL_BLEND_OP_MINUS_EXT: return static_cast<VkBlendOp>(1000148039);
		case PL_BLEND_OP_MINUS_CLAMPED_EXT: return static_cast<VkBlendOp>(1000148040);
		case PL_BLEND_OP_CONTRAST_EXT: return static_cast<VkBlendOp>(1000148041);
		case PL_BLEND_OP_INVERT_OVG_EXT: return static_cast<VkBlendOp>(1000148042);
		case PL_BLEND_OP_RED_EXT: return static_cast<VkBlendOp>(1000148043);
		case PL_BLEND_OP_GREEN_EXT: return static_cast<VkBlendOp>(1000148044);
		case PL_BLEND_OP_BLUE_EXT: return static_cast<VkBlendOp>(1000148045);
		default: return VK_BLEND_OP_ADD;
		}
	}

	static VkColorComponentFlags PlColorComponentFlagsToVkColorComponentFlags(PlColorComponentFlags flags) {
		VkColorComponentFlags newFlags = 0;
		if (flags & PL_COLOR_COMPONENT_R_BIT) newFlags |= VK_COLOR_COMPONENT_R_BIT;
		if (flags & PL_COLOR_COMPONENT_G_BIT) newFlags |= VK_COLOR_COMPONENT_G_BIT;
		if (flags & PL_COLOR_COMPONENT_B_BIT) newFlags |= VK_COLOR_COMPONENT_B_BIT;
		if (flags & PL_COLOR_COMPONENT_A_BIT) newFlags |= VK_COLOR_COMPONENT_A_BIT;
		return newFlags;
	}

	static VkLogicOp PlLogicOpToVkLogicOp(PlLogicOp op) {
		switch (op) {
		case PL_LOGIC_OP_CLEAR: return VK_LOGIC_OP_CLEAR;
		case PL_LOGIC_OP_AND: return VK_LOGIC_OP_AND;
		case PL_LOGIC_OP_AND_REVERSE: return VK_LOGIC_OP_AND_REVERSE;
		case PL_LOGIC_OP_COPY: return VK_LOGIC_OP_COPY;
		case PL_LOGIC_OP_AND_INVERTED: return VK_LOGIC_OP_AND_INVERTED;
		case PL_LOGIC_OP_NO_OP: return VK_LOGIC_OP_NO_OP;
		case PL_LOGIC_OP_XOR: return VK_LOGIC_OP_XOR;
		case PL_LOGIC_OP_OR: return VK_LOGIC_OP_OR;
		case PL_LOGIC_OP_NOR: return VK_LOGIC_OP_NOR;
		case PL_LOGIC_OP_EQUIVALENT: return VK_LOGIC_OP_EQUIVALENT;
		case PL_LOGIC_OP_INVERT: return VK_LOGIC_OP_INVERT;
		case PL_LOGIC_OP_OR_REVERSE: return VK_LOGIC_OP_OR_REVERSE;
		case PL_LOGIC_OP_COPY_INVERTED: return VK_LOGIC_OP_COPY_INVERTED;
		case PL_LOGIC_OP_OR_INVERTED: return VK_LOGIC_OP_OR_INVERTED;
		case PL_LOGIC_OP_NAND: return VK_LOGIC_OP_NAND;
		case PL_LOGIC_OP_SET: return VK_LOGIC_OP_SET;
		default: return VK_LOGIC_OP_CLEAR;
		}
	}

	static VkStencilOp PlStencilOpToVkStencilOp(PlStencilOp op) {
		switch (op) {
		case PL_STENCIL_OP_KEEP: return VK_STENCIL_OP_KEEP;
		case PL_STENCIL_OP_ZERO: return VK_STENCIL_OP_ZERO;
		case PL_STENCIL_OP_REPLACE: return VK_STENCIL_OP_REPLACE;
		case PL_STENCIL_OP_INCREMENT_AND_CLAMP: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case PL_STENCIL_OP_DECREMENT_AND_CLAMP: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		case PL_STENCIL_OP_INVERT: return VK_STENCIL_OP_INVERT;
		case PL_STENCIL_OP_INCREMENT_AND_WRAP: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case PL_STENCIL_OP_DECREMENT_AND_WRAP: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		default: return VK_STENCIL_OP_KEEP;
		}
	}

	static VkDynamicState PlDynamicStateToVkDynamicState(PlDynamicState state) {
		switch (state) {
		case PL_DYNAMIC_STATE_VIEWPORT: return VK_DYNAMIC_STATE_VIEWPORT;
		case PL_DYNAMIC_STATE_SCISSOR: return VK_DYNAMIC_STATE_SCISSOR;
		case PL_DYNAMIC_STATE_LINE_WIDTH: return VK_DYNAMIC_STATE_LINE_WIDTH;
		case PL_DYNAMIC_STATE_DEPTH_BIAS: return VK_DYNAMIC_STATE_DEPTH_BIAS;
		case PL_DYNAMIC_STATE_BLEND_CONSTANTS: return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
		case PL_DYNAMIC_STATE_DEPTH_BOUNDS: return VK_DYNAMIC_STATE_DEPTH_BOUNDS;
		case PL_DYNAMIC_STATE_STENCIL_COMPARE_MASK: return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
		case PL_DYNAMIC_STATE_STENCIL_WRITE_MASK: return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
		case PL_DYNAMIC_STATE_STENCIL_REFERENCE: return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
		case PL_DYNAMIC_STATE_CULL_MODE: return VK_DYNAMIC_STATE_CULL_MODE;
		case PL_DYNAMIC_STATE_FRONT_FACE: return VK_DYNAMIC_STATE_FRONT_FACE;
		case PL_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY: return VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY;
		case PL_DYNAMIC_STATE_VIEWPORT_WITH_COUNT: return VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT;
		case PL_DYNAMIC_STATE_SCISSOR_WITH_COUNT: return VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT;
		case PL_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE: return VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE;
		case PL_DYNAMIC_STATE_DEPTH_TEST_ENABLE: return VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE;
		case PL_DYNAMIC_STATE_DEPTH_WRITE_ENABLE: return VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE;
		case PL_DYNAMIC_STATE_DEPTH_COMPARE_OP: return VK_DYNAMIC_STATE_DEPTH_COMPARE_OP;
		case PL_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE: return VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE;
		case PL_DYNAMIC_STATE_STENCIL_TEST_ENABLE: return VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE;
		case PL_DYNAMIC_STATE_STENCIL_OP: return VK_DYNAMIC_STATE_STENCIL_OP;
		case PL_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE: return VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE;
		case PL_DYNAMIC_STATE_DEPTH_BIAS_ENABLE: return VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE;
		case PL_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE: return VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE;
		case PL_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV: return VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV;
		case PL_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT: return VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT;
		case PL_DYNAMIC_STATE_DISCARD_RECTANGLE_ENABLE_EXT: return VK_DYNAMIC_STATE_DISCARD_RECTANGLE_ENABLE_EXT;
		case PL_DYNAMIC_STATE_DISCARD_RECTANGLE_MODE_EXT: return VK_DYNAMIC_STATE_DISCARD_RECTANGLE_MODE_EXT;
		case PL_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT: return VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT;
		case PL_DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR: return VK_DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR;
		case PL_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV: return VK_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV;
		case PL_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV: return VK_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV;
		case PL_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_ENABLE_NV: return VK_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_ENABLE_NV;
		case PL_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV: return VK_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV;
		case PL_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR: return VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR;
		case PL_DYNAMIC_STATE_LINE_STIPPLE_EXT: return VK_DYNAMIC_STATE_LINE_STIPPLE_EXT;
		case PL_DYNAMIC_STATE_VERTEX_INPUT_EXT: return VK_DYNAMIC_STATE_VERTEX_INPUT_EXT;
		case PL_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT: return VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT;
		case PL_DYNAMIC_STATE_LOGIC_OP_EXT: return VK_DYNAMIC_STATE_LOGIC_OP_EXT;
		case PL_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT: return VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT;
		case PL_DYNAMIC_STATE_TESSELLATION_DOMAIN_ORIGIN_EXT: return VK_DYNAMIC_STATE_TESSELLATION_DOMAIN_ORIGIN_EXT;
		case PL_DYNAMIC_STATE_DEPTH_CLAMP_ENABLE_EXT: return VK_DYNAMIC_STATE_DEPTH_CLAMP_ENABLE_EXT;
		case PL_DYNAMIC_STATE_POLYGON_MODE_EXT: return VK_DYNAMIC_STATE_POLYGON_MODE_EXT;
		case PL_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT: return VK_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT;
		case PL_DYNAMIC_STATE_SAMPLE_MASK_EXT: return VK_DYNAMIC_STATE_SAMPLE_MASK_EXT;
		case PL_DYNAMIC_STATE_ALPHA_TO_COVERAGE_ENABLE_EXT: return VK_DYNAMIC_STATE_ALPHA_TO_COVERAGE_ENABLE_EXT;
		case PL_DYNAMIC_STATE_ALPHA_TO_ONE_ENABLE_EXT: return VK_DYNAMIC_STATE_ALPHA_TO_ONE_ENABLE_EXT;
		case PL_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT: return VK_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT;
		case PL_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT: return VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT;
		case PL_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT: return VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT;
		case PL_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT: return VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT;
		case PL_DYNAMIC_STATE_RASTERIZATION_STREAM_EXT: return VK_DYNAMIC_STATE_RASTERIZATION_STREAM_EXT;
		case PL_DYNAMIC_STATE_CONSERVATIVE_RASTERIZATION_MODE_EXT: return VK_DYNAMIC_STATE_CONSERVATIVE_RASTERIZATION_MODE_EXT;
		case PL_DYNAMIC_STATE_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT: return VK_DYNAMIC_STATE_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT;
		case PL_DYNAMIC_STATE_DEPTH_CLIP_ENABLE_EXT: return VK_DYNAMIC_STATE_DEPTH_CLIP_ENABLE_EXT;
		case PL_DYNAMIC_STATE_SAMPLE_LOCATIONS_ENABLE_EXT: return VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_ENABLE_EXT;
		case PL_DYNAMIC_STATE_COLOR_BLEND_ADVANCED_EXT: return VK_DYNAMIC_STATE_COLOR_BLEND_ADVANCED_EXT;
		case PL_DYNAMIC_STATE_PROVOKING_VERTEX_MODE_EXT: return VK_DYNAMIC_STATE_PROVOKING_VERTEX_MODE_EXT;
		case PL_DYNAMIC_STATE_LINE_RASTERIZATION_MODE_EXT: return VK_DYNAMIC_STATE_LINE_RASTERIZATION_MODE_EXT;
		case PL_DYNAMIC_STATE_LINE_STIPPLE_ENABLE_EXT: return VK_DYNAMIC_STATE_LINE_STIPPLE_ENABLE_EXT;
		case PL_DYNAMIC_STATE_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT: return VK_DYNAMIC_STATE_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT;
		case PL_DYNAMIC_STATE_VIEWPORT_W_SCALING_ENABLE_NV: return VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_ENABLE_NV;
		case PL_DYNAMIC_STATE_VIEWPORT_SWIZZLE_NV: return VK_DYNAMIC_STATE_VIEWPORT_SWIZZLE_NV;
		case PL_DYNAMIC_STATE_COVERAGE_TO_COLOR_ENABLE_NV: return VK_DYNAMIC_STATE_COVERAGE_TO_COLOR_ENABLE_NV;
		case PL_DYNAMIC_STATE_COVERAGE_TO_COLOR_LOCATION_NV: return VK_DYNAMIC_STATE_COVERAGE_TO_COLOR_LOCATION_NV;
		case PL_DYNAMIC_STATE_COVERAGE_MODULATION_MODE_NV: return VK_DYNAMIC_STATE_COVERAGE_MODULATION_MODE_NV;
		case PL_DYNAMIC_STATE_COVERAGE_MODULATION_TABLE_ENABLE_NV: return VK_DYNAMIC_STATE_COVERAGE_MODULATION_TABLE_ENABLE_NV;
		case PL_DYNAMIC_STATE_COVERAGE_MODULATION_TABLE_NV: return VK_DYNAMIC_STATE_COVERAGE_MODULATION_TABLE_NV;
		case PL_DYNAMIC_STATE_SHADING_RATE_IMAGE_ENABLE_NV: return VK_DYNAMIC_STATE_SHADING_RATE_IMAGE_ENABLE_NV;
		case PL_DYNAMIC_STATE_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV: return VK_DYNAMIC_STATE_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV;
		case PL_DYNAMIC_STATE_COVERAGE_REDUCTION_MODE_NV: return VK_DYNAMIC_STATE_COVERAGE_REDUCTION_MODE_NV;
		case PL_DYNAMIC_STATE_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT: return VK_DYNAMIC_STATE_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT;
		default: return VK_DYNAMIC_STATE_VIEWPORT;
		}
	}

	static VkVertexInputRate PlVertexInputRateToVkVertexInputRate(PlVertexInputRate rate) {
		switch (rate) {
		case PL_VERTEX_INPUT_RATE_VERTEX: return VK_VERTEX_INPUT_RATE_VERTEX;
		case PL_VERTEX_INPUT_RATE_INSTANCE: return VK_VERTEX_INPUT_RATE_INSTANCE;
		default: return VK_VERTEX_INPUT_RATE_VERTEX;
		}
	}

	class VulkanBufferBinding : public PlazaBufferBinding {
	public:
		VulkanBufferBinding(uint64_t descriptorCount, uint8_t binding, PlBufferType type, PlRenderStage stage, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, const std::string& name)
			: PlazaBufferBinding(descriptorCount, binding, type, stage, maxItems, stride, bufferCount, name) {};
		virtual void Compile() override;
		virtual void Destroy() override;

		VkDescriptorSetLayoutBinding GetDescriptorLayout() {
			return plvk::descriptorSetLayoutBinding(mBinding, mDescriptorCount, PlBufferTypeToVkDescriptorType(mType), nullptr, PlRenderStageToVkShaderStage(mStage));
		}

		VkDescriptorBufferInfo GetBufferInfo(unsigned int currentFrame) {
			return  plvk::descriptorBufferInfo(mBuffer->GetBuffer(currentFrame), 0, mMaxItems * mStride);
		}

		VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet& descriptorSet, VkDescriptorBufferInfo* bufferInfo) {
			if (mType == PL_BUFFER_PUSH_CONSTANTS)
				return {};

			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mType), mDescriptorCount, nullptr, bufferInfo);
		}

		std::shared_ptr<PlVkBuffer> mBuffer = nullptr;
	private:
	};

	class VulkanTextureBinding : public PlazaTextureBinding {
	public:
		VulkanTextureBinding(uint64_t descriptorCount, uint8_t location, uint8_t binding, PlBufferType bufferType, PlRenderStage renderStage, PlImageLayout initialLayout, std::shared_ptr<Texture> texture)
			: PlazaTextureBinding(descriptorCount, location, binding, bufferType, renderStage, initialLayout, texture) {
			mName = texture->mName;
		};
		virtual void Compile() override;
		virtual void Destroy() override;

		VkDescriptorSetLayoutBinding GetDescriptorLayout() {
			return plvk::descriptorSetLayoutBinding(mBinding, mDescriptorCount, PlBufferTypeToVkDescriptorType(mBufferType), nullptr, PlRenderStageToVkShaderStage(mStage));
		}

		VkDescriptorImageInfo GetImageInfo() {
			if (mTexture == nullptr)
				return VkDescriptorImageInfo{};

			return plvk::descriptorImageInfo(GetTexture()->GetLayout(), GetTexture()->mImageView, GetTexture()->mSampler);
		}

		VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet& descriptorSet, VkDescriptorImageInfo* imageInfo) {
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mBufferType), mMaxBindlessResources > 0 ? 1 : mDescriptorCount, imageInfo, nullptr);
		}

		VulkanTexture* GetTexture() {
			return static_cast<VulkanTexture*>(mTexture.get());
		}

		//std::shared_ptr<VulkanTexture> mTexture = nullptr;
	private:

	};

	class VulkanRenderPass : public PlazaRenderPass {
	public:
		VulkanRenderPass(std::string name, int stage, PlRenderMethod renderMethod) : PlazaRenderPass(name, stage, renderMethod) {}
		//std::vector<std::shared_ptr<VulkanPlazaPipeline>> mPipelines = std::vector<std::shared_ptr<VulkanPlazaPipeline>>();

		virtual void Compile() override;
		virtual void BindRenderPass() override;
		virtual void RenderIndirectBuffer(PlazaRenderGraph* plazaRenderGraph) override;
		virtual void RenderFullScreenQuad(PlazaRenderGraph* plazaRenderGraph) override;
		virtual void CompilePipeline(PlPipelineCreateInfo createInfo) override;

		VulkanRenderPass* AddInputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mInputBindings.emplace(resource->mName, resource);
			return this;
		}

		VulkanRenderPass* AddOutputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mOutputBindings.emplace(resource->mName, resource);
			return this;
		}

		inline void UpdateCommandBuffer(VkCommandBuffer& commandBuffer) {
			mCommandBuffer = commandBuffer;
		}

		VkRenderPass mRenderPass = VK_NULL_HANDLE;
		VkFramebuffer mFrameBuffer = VK_NULL_HANDLE;

		std::vector<VkDescriptorSet> mDescriptorSets = std::vector<VkDescriptorSet>();
		VkDescriptorSetLayout mDescriptorSetLayout;
	private:
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	};

	class VulkanRenderGraph : public PlazaRenderGraph {
	public:
		void Execute(uint8_t imageIndex, uint8_t currentFrame) override;
		bool BindPass(std::string passName) override;
		//void Compile() override;

		void UpdateCommandBuffer(VkCommandBuffer& commandBuffer) {
			mCommandBuffer = &commandBuffer;
		}

		VulkanRenderPass* GetRenderPass(std::string name) {
			if (mPasses.find(name) != mPasses.end())
				return (VulkanRenderPass*)mPasses.find(name)->second.get();
			return nullptr;
		}

		void BuildDefaultRenderGraph() override;
		void AddPipeline() override;
		void CreatePipeline(PlPipelineCreateInfo createInfo) override;

	private:
		VkCommandBuffer* mCommandBuffer = VK_NULL_HANDLE;
	};
}