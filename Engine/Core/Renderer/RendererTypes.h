#pragma once

namespace Plaza {
	enum PlTextureType {
		PL_TYPE_1D = 0,
		PL_TYPE_2D,
		PL_TYPE_3D,
	};

	enum PlViewType {
		PL_VIEW_TYPE_1D = 0,
		PL_VIEW_TYPE_2D = 1,
		PL_VIEW_TYPE_3D = 2,
		PL_VIEW_TYPE_CUBE = 3,
		PL_VIEW_TYPE_1D_ARRAY = 4,
		PL_VIEW_TYPE_2D_ARRAY = 5,
		PL_VIEW_TYPE_CUBE_ARRAY = 6
	};

	enum PlBufferType {
		PL_BUFFER_PUSH_CONSTANTS = 0,
		PL_BUFFER_UNIFORM_BUFFER,
		PL_BUFFER_STORAGE_BUFFER,
		PL_BUFFER_STORAGE_BUFFER_DYNAMIC,
		PL_BUFFER_SAMPLER,
		PL_BUFFER_COMBINED_IMAGE_SAMPLER
	};

	enum PlTextureFormat {
		PL_FORMAT_R32G32B32A32_SFLOAT = 0,
		PL_FORMAT_R32G32B32A32_SINT,
		PL_FORMAT_R32G32B32_SFLOAT,
		PL_FORMAT_R32G32_SFLOAT,
		PL_FORMAT_R32_SFLOAT,
		PL_FORMAT_R32_UINT,
		PL_FORMAT_R8G8B8A8_UNORM,
		PL_FORMAT_R8G8B8_UNORM,
		PL_FORMAT_D32_SFLOAT_S8_UINT
	};

	enum PlImageUsage {
		PL_IMAGE_USAGE_TRANSFER_SRC = 1 << 0,
		PL_IMAGE_USAGE_TRANSFER_DST = 1 << 1,
		PL_IMAGE_USAGE_SAMPLED = 1 << 2,
		PL_IMAGE_USAGE_STORAGE = 1 << 3,
		PL_IMAGE_USAGE_COLOR_ATTACHMENT = 1 << 4,
		PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 5,
		PL_IMAGE_USAGE_INPUT_ATTACHMENT = 1 << 6,
	};

	enum PlImageLayout {
		PL_IMAGE_LAYOUT_UNDEFINED = 0,
		PL_IMAGE_LAYOUT_GENERAL = 1,
		PL_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
		PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
		PL_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
		PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
		PL_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
		PL_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
		PL_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 8,
		PL_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 9,
		PL_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL = 10,
		PL_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL = 11,
		PL_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL = 12,
		PL_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL = 13,
		PL_IMAGE_LAYOUT_READ_ONLY_OPTIMAL = 14,
		PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL = 15,
		PL_IMAGE_LAYOUT_PRESENT_SRC_KHR = 16,
		PL_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR = 17,
		PL_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR = 18,
		PL_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR = 19,
		PL_IMAGE_LAYOUT_SHARED_PRESENT_KHR = 20,
		PL_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT = 21,
		PL_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR = 22,
	};

	enum PlRenderStage {
		PL_STAGE_VERTEX = 1 << 0,
		PL_STAGE_FRAGMENT = 1 << 1,
		PL_STAGE_COMPUTE = 1 << 2,
		PL_STAGE_GEOMETRY = 1 << 3,
		PL_STAGE_ALL = 1 << 4,
	};

	enum PlBindingType {
		PL_BINDING_UNDEFINED = 1 << 0,
		PL_BINDING_BUFFER = 1 << 1,
		PL_BINDING_TEXTURE = 1 << 2
	};

	enum PlMemoryProperty : uint32_t {
		PL_MEMORY_PROPERTY_DEVICE_LOCAL = 1 << 0,
		PL_MEMORY_PROPERTY_HOST_VISIBLE = 1 << 1,
		PL_MEMORY_PROPERTY_HOST_COHERENT = 1 << 2,
		PL_MEMORY_PROPERTY_HOST_CACHED = 1 << 3,
		PL_MEMORY_PROPERTY_LAZILY_ALLOCATED = 1 << 4,
		PL_MEMORY_PROPERTY_PROTECTED = 1 << 5
	};

	enum PlBufferUsage : uint32_t {
		PL_BUFFER_USAGE_TRANSFER_SRC = 1 << 0,
		PL_BUFFER_USAGE_TRANSFER_DST = 1 << 1,
		PL_BUFFER_USAGE_UNIFORM_BUFFER = 1 << 2,
		PL_BUFFER_USAGE_STORAGE_BUFFER = 1 << 3,
		PL_BUFFER_USAGE_INDEX_BUFFER = 1 << 4,
		PL_BUFFER_USAGE_VERTEX_BUFFER = 1 << 5,
		PL_BUFFER_USAGE_INDIRECT_BUFFER = 1 << 6,
	};

	enum PlMemoryUsage {
		PL_MEMORY_USAGE_UNKNOWN = 0,
		PL_MEMORY_USAGE_GPU_ONLY = 1,
		PL_MEMORY_USAGE_CPU_ONLY = 2,
		PL_MEMORY_USAGE_CPU_TO_GPU = 3,
		PL_MEMORY_USAGE_GPU_TO_CPU = 4,
		PL_MEMORY_USAGE_CPU_COPY = 5,
		PL_MEMORY_USAGE_GPU_LAZILY_ALLOCATED = 6,
		PL_MEMORY_USAGE_AUTO = 7,
		PL_MEMORY_USAGE_AUTO_PREFER_DEVICE = 8,
		PL_MEMORY_USAGE_AUTO_PREFER_HOST = 9,
	};

	enum PlRenderMethod {
		PL_RENDER_INDIRECT_BUFFER = 0,
		PL_RENDER_INDIRECT_BUFFER_SHADOW_MAP = 1,
		PL_RENDER_FULL_SCREEN_QUAD = 2
	};

	enum PlPrimitiveTopology {
		PL_TOPOLOGY_TRIANGLE_LIST = 0,
		PL_TOPOLOGY_LINE_LIST = 1,
		PL_TOPOLOGY_POINT_LIST = 2,
	};

	enum PlPolygonMode {
		PL_POLYGON_MODE_FILL = 0,
		PL_POLYGON_MODE_LINE = 1,
		PL_POLYGON_MODE_POINT = 2,
	};

	enum PlCullMode {
		PL_CULL_MODE_NONE = 0,
		PL_CULL_MODE_FRONT = 1,
		PL_CULL_MODE_BACK = 2,
		PL_CULL_MODE_FRONT_AND_BACK = 3,
	};

	enum PlFrontFace {
		PL_FRONT_FACE_COUNTER_CLOCKWISE = 0,
		PL_FRONT_FACE_CLOCKWISE = 1,
	};

	enum PlSampleCount {
		PL_SAMPLE_COUNT_1_BIT = 0,
		PL_SAMPLE_COUNT_2_BIT = 1,
		PL_SAMPLE_COUNT_4_BIT = 2,
		PL_SAMPLE_COUNT_8_BIT = 3,
		PL_SAMPLE_COUNT_16_BIT = 4,
		PL_SAMPLE_COUNT_32_BIT = 5,
		PL_SAMPLE_COUNT_64_BIT = 6,
	};

	enum PlCompareOp {
		PL_COMPARE_OP_NEVER = 0,
		PL_COMPARE_OP_LESS = 1,
		PL_COMPARE_OP_EQUAL = 2,
		PL_COMPARE_OP_LESS_OR_EQUAL = 3,
		PL_COMPARE_OP_GREATER = 4,
		PL_COMPARE_OP_NOT_EQUAL = 5,
		PL_COMPARE_OP_GREATER_OR_EQUAL = 6,
		PL_COMPARE_OP_ALWAYS = 7,
	};

	enum PlBlendFactor {
		PL_BLEND_FACTOR_ZERO = 0,
		PL_BLEND_FACTOR_ONE = 1,
		PL_BLEND_FACTOR_SRC_COLOR = 2,
		PL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 3,
		PL_BLEND_FACTOR_DST_COLOR = 4,
		PL_BLEND_FACTOR_ONE_MINUS_DST_COLOR = 5,
		PL_BLEND_FACTOR_SRC_ALPHA = 6,
		PL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 7,
		PL_BLEND_FACTOR_DST_ALPHA = 8,
		PL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 9,
		PL_BLEND_FACTOR_CONSTANT_COLOR = 10,
		PL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = 11,
		PL_BLEND_FACTOR_CONSTANT_ALPHA = 12,
		PL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,
		PL_BLEND_FACTOR_SRC_ALPHA_SATURATE = 14,
		PL_BLEND_FACTOR_SRC1_COLOR = 15,
		PL_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR = 16,
		PL_BLEND_FACTOR_SRC1_ALPHA = 17,
		PL_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA = 18,
		PL_BLEND_FACTOR_MAX_ENUM = 0x7FFFFFFF
	};

	enum PlBlendOp {
		PL_BLEND_OP_ADD = 0,
		PL_BLEND_OP_SUBTRACT = 1,
		PL_BLEND_OP_REVERSE_SUBTRACT = 2,
		PL_BLEND_OP_MIN = 3,
		PL_BLEND_OP_MAX = 4,
		PL_BLEND_OP_ZERO_EXT = 1000148000,
		PL_BLEND_OP_SRC_EXT = 1000148001,
		PL_BLEND_OP_DST_EXT = 1000148002,
		PL_BLEND_OP_SRC_OVER_EXT = 1000148003,
		PL_BLEND_OP_DST_OVER_EXT = 1000148004,
		PL_BLEND_OP_SRC_IN_EXT = 1000148005,
		PL_BLEND_OP_DST_IN_EXT = 1000148006,
		PL_BLEND_OP_SRC_OUT_EXT = 1000148007,
		PL_BLEND_OP_DST_OUT_EXT = 1000148008,
		PL_BLEND_OP_SRC_ATOP_EXT = 1000148009,
		PL_BLEND_OP_DST_ATOP_EXT = 1000148010,
		PL_BLEND_OP_XOR_EXT = 1000148011,
		PL_BLEND_OP_MULTIPLY_EXT = 1000148012,
		PL_BLEND_OP_SCREEN_EXT = 1000148013,
		PL_BLEND_OP_OVERLAY_EXT = 1000148014,
		PL_BLEND_OP_DARKEN_EXT = 1000148015,
		PL_BLEND_OP_LIGHTEN_EXT = 1000148016,
		PL_BLEND_OP_COLORDODGE_EXT = 1000148017,
		PL_BLEND_OP_COLORBURN_EXT = 1000148018,
		PL_BLEND_OP_HARDLIGHT_EXT = 1000148019,
		PL_BLEND_OP_SOFTLIGHT_EXT = 1000148020,
		PL_BLEND_OP_DIFFERENCE_EXT = 1000148021,
		PL_BLEND_OP_EXCLUSION_EXT = 1000148022,
		PL_BLEND_OP_INVERT_EXT = 1000148023,
		PL_BLEND_OP_INVERT_RGB_EXT = 1000148024,
		PL_BLEND_OP_LINEARDODGE_EXT = 1000148025,
		PL_BLEND_OP_LINEARBURN_EXT = 1000148026,
		PL_BLEND_OP_VIVIDLIGHT_EXT = 1000148027,
		PL_BLEND_OP_LINEARLIGHT_EXT = 1000148028,
		PL_BLEND_OP_PINLIGHT_EXT = 1000148029,
		PL_BLEND_OP_HARDMIX_EXT = 1000148030,
		PL_BLEND_OP_HSL_HUE_EXT = 1000148031,
		PL_BLEND_OP_HSL_SATURATION_EXT = 1000148032,
		PL_BLEND_OP_HSL_COLOR_EXT = 1000148033,
		PL_BLEND_OP_HSL_LUMINOSITY_EXT = 1000148034,
		PL_BLEND_OP_PLUS_EXT = 1000148035,
		PL_BLEND_OP_PLUS_CLAMPED_EXT = 1000148036,
		PL_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT = 1000148037,
		PL_BLEND_OP_PLUS_DARKER_EXT = 1000148038,
		PL_BLEND_OP_MINUS_EXT = 1000148039,
		PL_BLEND_OP_MINUS_CLAMPED_EXT = 1000148040,
		PL_BLEND_OP_CONTRAST_EXT = 1000148041,
		PL_BLEND_OP_INVERT_OVG_EXT = 1000148042,
		PL_BLEND_OP_RED_EXT = 1000148043,
		PL_BLEND_OP_GREEN_EXT = 1000148044,
		PL_BLEND_OP_BLUE_EXT = 1000148045,
		PL_BLEND_OP_MAX_ENUM = 0x7FFFFFFF
	};

	enum PlColorComponentFlags {
		PL_COLOR_COMPONENT_R_BIT = 0x00000001,
		PL_COLOR_COMPONENT_G_BIT = 0x00000002,
		PL_COLOR_COMPONENT_B_BIT = 0x00000004,
		PL_COLOR_COMPONENT_A_BIT = 0x00000008,
		PL_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
	};

	enum PlLogicOp {
		PL_LOGIC_OP_CLEAR = 0,
		PL_LOGIC_OP_AND = 1,
		PL_LOGIC_OP_AND_REVERSE = 2,
		PL_LOGIC_OP_COPY = 3,
		PL_LOGIC_OP_AND_INVERTED = 4,
		PL_LOGIC_OP_NO_OP = 5,
		PL_LOGIC_OP_XOR = 6,
		PL_LOGIC_OP_OR = 7,
		PL_LOGIC_OP_NOR = 8,
		PL_LOGIC_OP_EQUIVALENT = 9,
		PL_LOGIC_OP_INVERT = 10,
		PL_LOGIC_OP_OR_REVERSE = 11,
		PL_LOGIC_OP_COPY_INVERTED = 12,
		PL_LOGIC_OP_OR_INVERTED = 13,
		PL_LOGIC_OP_NAND = 14,
		PL_LOGIC_OP_SET = 15,
		PL_LOGIC_OP_MAX_ENUM = 0x7FFFFFFF
	};

	enum PlStencilOp {
		PL_STENCIL_OP_KEEP = 0,
		PL_STENCIL_OP_ZERO = 1,
		PL_STENCIL_OP_REPLACE = 2,
		PL_STENCIL_OP_INCREMENT_AND_CLAMP = 3,
		PL_STENCIL_OP_DECREMENT_AND_CLAMP = 4,
		PL_STENCIL_OP_INVERT = 5,
		PL_STENCIL_OP_INCREMENT_AND_WRAP = 6,
		PL_STENCIL_OP_DECREMENT_AND_WRAP = 7,
		PL_STENCIL_OP_MAX_ENUM = 0x7FFFFFFF
	};

	enum PlDynamicState {
		PL_DYNAMIC_STATE_VIEWPORT = 0,
		PL_DYNAMIC_STATE_SCISSOR = 1,
		PL_DYNAMIC_STATE_LINE_WIDTH = 2,
		PL_DYNAMIC_STATE_DEPTH_BIAS = 3,
		PL_DYNAMIC_STATE_BLEND_CONSTANTS = 4,
		PL_DYNAMIC_STATE_DEPTH_BOUNDS = 5,
		PL_DYNAMIC_STATE_STENCIL_COMPARE_MASK = 6,
		PL_DYNAMIC_STATE_STENCIL_WRITE_MASK = 7,
		PL_DYNAMIC_STATE_STENCIL_REFERENCE = 8,
		PL_DYNAMIC_STATE_CULL_MODE = 1000267000,
		PL_DYNAMIC_STATE_FRONT_FACE = 1000267001,
		PL_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY = 1000267002,
		PL_DYNAMIC_STATE_VIEWPORT_WITH_COUNT = 1000267003,
		PL_DYNAMIC_STATE_SCISSOR_WITH_COUNT = 1000267004,
		PL_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE = 1000267005,
		PL_DYNAMIC_STATE_DEPTH_TEST_ENABLE = 1000267006,
		PL_DYNAMIC_STATE_DEPTH_WRITE_ENABLE = 1000267007,
		PL_DYNAMIC_STATE_DEPTH_COMPARE_OP = 1000267008,
		PL_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE = 1000267009,
		PL_DYNAMIC_STATE_STENCIL_TEST_ENABLE = 1000267010,
		PL_DYNAMIC_STATE_STENCIL_OP = 1000267011,
		PL_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE = 1000377001,
		PL_DYNAMIC_STATE_DEPTH_BIAS_ENABLE = 1000377002,
		PL_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE = 1000377004,
		PL_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV = 1000087000,
		PL_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT = 1000099000,
		PL_DYNAMIC_STATE_DISCARD_RECTANGLE_ENABLE_EXT = 1000099001,
		PL_DYNAMIC_STATE_DISCARD_RECTANGLE_MODE_EXT = 1000099002,
		PL_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT = 1000143000,
		PL_DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR = 1000347000,
		PL_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV = 1000164004,
		PL_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV = 1000164006,
		PL_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_ENABLE_NV = 1000205000,
		PL_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV = 1000205001,
		PL_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR = 1000226000,
		PL_DYNAMIC_STATE_LINE_STIPPLE_EXT = 1000259000,
		PL_DYNAMIC_STATE_VERTEX_INPUT_EXT = 1000352000,
		PL_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT = 1000377000,
		PL_DYNAMIC_STATE_LOGIC_OP_EXT = 1000377003,
		PL_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT = 1000381000,
		PL_DYNAMIC_STATE_TESSELLATION_DOMAIN_ORIGIN_EXT = 1000455002,
		PL_DYNAMIC_STATE_DEPTH_CLAMP_ENABLE_EXT = 1000455003,
		PL_DYNAMIC_STATE_POLYGON_MODE_EXT = 1000455004,
		PL_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT = 1000455005,
		PL_DYNAMIC_STATE_SAMPLE_MASK_EXT = 1000455006,
		PL_DYNAMIC_STATE_ALPHA_TO_COVERAGE_ENABLE_EXT = 1000455007,
		PL_DYNAMIC_STATE_ALPHA_TO_ONE_ENABLE_EXT = 1000455008,
		PL_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT = 1000455009,
		PL_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT = 1000455010,
		PL_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT = 1000455011,
		PL_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT = 1000455012,
		PL_DYNAMIC_STATE_RASTERIZATION_STREAM_EXT = 1000455013,
		PL_DYNAMIC_STATE_CONSERVATIVE_RASTERIZATION_MODE_EXT = 1000455014,
		PL_DYNAMIC_STATE_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT = 1000455015,
		PL_DYNAMIC_STATE_DEPTH_CLIP_ENABLE_EXT = 1000455016,
		PL_DYNAMIC_STATE_SAMPLE_LOCATIONS_ENABLE_EXT = 1000455017,
		PL_DYNAMIC_STATE_COLOR_BLEND_ADVANCED_EXT = 1000455018,
		PL_DYNAMIC_STATE_PROVOKING_VERTEX_MODE_EXT = 1000455019,
		PL_DYNAMIC_STATE_LINE_RASTERIZATION_MODE_EXT = 1000455020,
		PL_DYNAMIC_STATE_LINE_STIPPLE_ENABLE_EXT = 1000455021,
		PL_DYNAMIC_STATE_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT = 1000455022,
		PL_DYNAMIC_STATE_VIEWPORT_W_SCALING_ENABLE_NV = 1000455023,
		PL_DYNAMIC_STATE_VIEWPORT_SWIZZLE_NV = 1000455024,
		PL_DYNAMIC_STATE_COVERAGE_TO_COLOR_ENABLE_NV = 1000455025,
		PL_DYNAMIC_STATE_COVERAGE_TO_COLOR_LOCATION_NV = 1000455026,
		PL_DYNAMIC_STATE_COVERAGE_MODULATION_MODE_NV = 1000455027,
		PL_DYNAMIC_STATE_COVERAGE_MODULATION_TABLE_ENABLE_NV = 1000455028,
		PL_DYNAMIC_STATE_COVERAGE_MODULATION_TABLE_NV = 1000455029,
		PL_DYNAMIC_STATE_SHADING_RATE_IMAGE_ENABLE_NV = 1000455030,
		PL_DYNAMIC_STATE_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV = 1000455031,
		PL_DYNAMIC_STATE_COVERAGE_REDUCTION_MODE_NV = 1000455032,
		PL_DYNAMIC_STATE_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT = 1000524000,
		PL_DYNAMIC_STATE_CULL_MODE_EXT = PL_DYNAMIC_STATE_CULL_MODE,
		PL_DYNAMIC_STATE_FRONT_FACE_EXT = PL_DYNAMIC_STATE_FRONT_FACE,
		PL_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT = PL_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
		PL_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT = PL_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
		PL_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT = PL_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
		PL_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT = PL_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE,
		PL_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT = PL_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
		PL_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT = PL_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
		PL_DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT = PL_DYNAMIC_STATE_DEPTH_COMPARE_OP,
		PL_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT = PL_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
		PL_DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT = PL_DYNAMIC_STATE_STENCIL_TEST_ENABLE,
		PL_DYNAMIC_STATE_STENCIL_OP_EXT = PL_DYNAMIC_STATE_STENCIL_OP,
		PL_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE_EXT = PL_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE,
		PL_DYNAMIC_STATE_DEPTH_BIAS_ENABLE_EXT = PL_DYNAMIC_STATE_DEPTH_BIAS_ENABLE,
		PL_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE_EXT = PL_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE,
		PL_DYNAMIC_STATE_MAX_ENUM = 0x7FFFFFFF
	};

	enum PlVertexInputRate {
		PL_VERTEX_INPUT_RATE_VERTEX = 0,
		PL_VERTEX_INPUT_RATE_INSTANCE = 1,
		PL_VERTEX_INPUT_RATE_MAX_ENUM = 0x7FFFFFFF
	};

	struct PlPipelineRasterizationStateCreateInfo {
		bool depthClampEnable;
		bool rasterizerDiscardEnable;
		PlPolygonMode polygonMode;
		PlCullMode cullMode;
		PlFrontFace frontFace;
		bool depthBiasEnable;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
		float lineWidth;
	};

	struct PlPipelineColorBlendAttachmentState {
		bool blendEnable;
		PlBlendFactor srcColorBlendFactor;
		PlBlendFactor dstColorBlendFactor;
		PlBlendOp colorBlendOp;
		PlBlendFactor srcAlphaBlendFactor;
		PlBlendFactor dstAlphaBlendFactor;
		PlBlendOp alphaBlendOp;
		PlColorComponentFlags colorWriteMask;
	};

	struct PlPipelineColorBlendStateCreateInfo {
		bool logicOpEnable;
		PlLogicOp logicOp;
		std::vector<PlPipelineColorBlendAttachmentState> attachments;
		float blendConstants[4];
	};

	struct PlStencilOpState {
		PlStencilOp    failOp;
		PlStencilOp    passOp;
		PlStencilOp    depthFailOp;
		PlCompareOp    compareOp;
		uint32_t       compareMask;
		uint32_t       writeMask;
		uint32_t       reference;
	};

	struct PlPipelineDepthStencilStateCreateInfo {
		bool depthTestEnable;
		bool depthWriteEnable;
		PlCompareOp depthCompareOp;
		bool depthBoundsTestEnable;
		bool stencilTestEnable;
		PlStencilOpState front;
		PlStencilOpState back;
		float minDepthBounds;
		float maxDepthBounds;
	};

	struct PlViewport {
		float    x;
		float    y;
		float    width;
		float    height;
		float    minDepth;
		float    maxDepth;
	};

	struct PlRect2D {
		int32_t offsetX;
		int32_t offsetY;
		uint32_t extentX;
		uint32_t extentY;
	};

	struct PlPipelineViewportStateCreateInfo {
		uint32_t viewportCount;
		uint32_t scissorsCount;
	};

	struct PlPipelineMultisampleStateCreateInfo {
		PlSampleCount rasterizationSamples;
		bool sampleShadingEnable;
	};

	struct PlVertexInputBindingDescription {
		uint32_t binding;
		uint32_t stride;
		PlVertexInputRate inputRate;
	};

	struct PlVertexInputAttributeDescription {
		uint32_t location;
		uint32_t binding;
		PlTextureFormat format;
		uint32_t offset;
	};

	struct PlPipelineShaderStageCreateInfo {
		PlRenderStage stage;
		std::string shadersPath;
		std::string name;
	};

	struct PlPushConstantRange {
		PlRenderStage stageFlags;
		uint32_t offset;
		uint32_t size;
	};

	struct PlPipelineCreateInfo {
		std::vector<PlPipelineShaderStageCreateInfo> shaderStages;
		std::vector<PlVertexInputBindingDescription> vertexBindingDescriptions;
		std::vector<PlVertexInputAttributeDescription> vertexAttributeDescriptions;
		PlPrimitiveTopology topology;
		bool primitiveRestartEnable;
		PlPipelineRasterizationStateCreateInfo rasterization;
		PlPipelineColorBlendStateCreateInfo colorBlendState;
		PlPipelineDepthStencilStateCreateInfo depthStencilState;
		PlPipelineViewportStateCreateInfo viewPortState;
		PlPipelineMultisampleStateCreateInfo multiSampleState;
		std::vector<PlDynamicState> dynamicStates;
		std::vector<PlPushConstantRange> pushConstants;
	};

#pragma region Initializators
	namespace pl {
		static PlPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(
			PlRenderStage stage,
			std::string shadersPath,
			std::string name
		) {
			PlPipelineShaderStageCreateInfo shaderStageInfo{};
			shaderStageInfo.stage = stage;
			shaderStageInfo.shadersPath = shadersPath;
			shaderStageInfo.name = name;
			return shaderStageInfo;
		}

		static PlViewport viewport(
			glm::vec2 position,
			glm::vec2 size,
			float minDepth = 1.0f,
			float maxDepth = 1.0f
		) {
			PlViewport viewport{};
			viewport.x = position.x;
			viewport.y = position.y;
			viewport.width = (float)size.x;
			viewport.height = (float)size.y;
			viewport.minDepth = minDepth;
			viewport.maxDepth = maxDepth;
			return viewport;
		}

		static PlRect2D rect2D(
			glm::vec2 offset,
			glm::vec2 size
		) {
			PlRect2D scissor{};
			scissor.offsetX = (int32_t)offset.x;
			scissor.offsetY = (int32_t)offset.y;
			scissor.extentX = (uint32_t)size.x;
			scissor.extentY = (uint32_t)size.y;
			return scissor;
		}

		static PlPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
			uint32_t viewportCount,
			uint32_t scissorsCount
		) {
			PlPipelineViewportStateCreateInfo viewportState{};
			viewportState.viewportCount = viewportCount;
			viewportState.scissorsCount = scissorsCount;
			return viewportState;
		}

		static PlPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
			bool depthClampEnable = false,
			bool rasterizerDiscardEnable = false,
			PlPolygonMode polygonMode = PL_POLYGON_MODE_FILL,
			float lineWidth = 1.0f,
			bool depthBiasEnable = false,
			float depthBiasConstantFactor = 0.0f,
			float depthBiasClamp = 0.0f,
			float depthBiasSlopeFactor = 0.0f,
			PlCullMode cullMode = PL_CULL_MODE_BACK,
			PlFrontFace frontFace = PL_FRONT_FACE_COUNTER_CLOCKWISE
		) {
			PlPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.depthClampEnable = depthClampEnable;
			rasterizer.rasterizerDiscardEnable = rasterizerDiscardEnable;
			rasterizer.polygonMode = polygonMode;
			rasterizer.lineWidth = lineWidth;
			rasterizer.depthBiasEnable = depthBiasEnable;
			rasterizer.depthBiasConstantFactor = depthBiasConstantFactor;
			rasterizer.depthBiasClamp = depthBiasClamp;
			rasterizer.depthBiasSlopeFactor = depthBiasSlopeFactor;
			rasterizer.cullMode = cullMode;
			rasterizer.frontFace = frontFace;
			return rasterizer;
		}

		static PlPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
			PlSampleCount rasterizationSamples = PL_SAMPLE_COUNT_1_BIT,
			bool sampleShadingEnable = false
		) {
			PlPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sampleShadingEnable = sampleShadingEnable;
			multisampling.rasterizationSamples = rasterizationSamples;
			return multisampling;
		}

		static PlPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
			bool depthTestEnable = true,
			bool depthWriteEnable = true,
			PlCompareOp depthCompareOp = PL_COMPARE_OP_LESS,
			bool depthBoundsTestEnable = false,
			bool stencilTestEnable = false
		) {
			PlPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.depthTestEnable = depthTestEnable;
			depthStencil.depthWriteEnable = depthWriteEnable;
			depthStencil.depthCompareOp = depthCompareOp;
			depthStencil.depthBoundsTestEnable = depthBoundsTestEnable;
			depthStencil.stencilTestEnable = stencilTestEnable;
			return depthStencil;
		}

		static PlPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
			bool blendEnable = true,
			PlBlendFactor srcColorBlendFactor = PL_BLEND_FACTOR_SRC_ALPHA,
			PlBlendFactor dstColorBlendFactor = PL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			PlBlendOp colorBlendOp = PL_BLEND_OP_ADD,
			PlBlendFactor srcAlphaBlendFactor = PL_BLEND_FACTOR_ONE,
			PlBlendFactor dstAlphaBlendFactor = PL_BLEND_FACTOR_ZERO,
			PlBlendOp alphaBlendOp = PL_BLEND_OP_ADD,
			PlColorComponentFlags colorWriteMask = PlColorComponentFlags(PL_COLOR_COMPONENT_R_BIT | PL_COLOR_COMPONENT_G_BIT | PL_COLOR_COMPONENT_B_BIT | PL_COLOR_COMPONENT_A_BIT)
		) {
			PlPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.blendEnable = blendEnable;
			colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
			colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
			colorBlendAttachment.colorBlendOp = colorBlendOp;
			colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
			colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
			colorBlendAttachment.alphaBlendOp = alphaBlendOp;
			colorBlendAttachment.colorWriteMask = colorWriteMask;
			return colorBlendAttachment;
		}

		static PlPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
			std::vector<PlPipelineColorBlendAttachmentState> attachments,
			bool logicOpEnable = false,
			PlLogicOp logicOp = PL_LOGIC_OP_COPY,
			std::vector<float> blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
		) {
			PlPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.logicOpEnable = logicOpEnable;
			colorBlending.logicOp = logicOp;
			colorBlending.attachments = attachments;
			colorBlending.blendConstants[0] = blendConstants[0];
			colorBlending.blendConstants[1] = blendConstants[1];
			colorBlending.blendConstants[2] = blendConstants[2];
			colorBlending.blendConstants[3] = blendConstants[3];
			return colorBlending;
		}

		static PlPushConstantRange pushConstantRange(
			PlRenderStage stage,
			uint32_t offset,
			uint32_t size
		) {
			PlPushConstantRange range{};
			range.stageFlags = stage;
			range.offset = offset;
			range.size = size;
			return range;
		}

		static PlVertexInputBindingDescription vertexInputBindingDescription(
			uint32_t binding,
			uint32_t stride,
			PlVertexInputRate inputRate) {
			PlVertexInputBindingDescription description{};
			description.binding = binding;
			description.stride = stride;
			description.inputRate = inputRate;
			return description;
		}

		static PlVertexInputAttributeDescription vertexInputAttributeDescription(
			uint32_t location,
			uint32_t binding,
			PlTextureFormat format,
			uint32_t offset) {
			PlVertexInputAttributeDescription attribute{};
			attribute.location = location;
			attribute.binding = binding;
			attribute.format = format;
			attribute.offset = offset;
			return attribute;
		}

		static PlPipelineCreateInfo pipelineCreateInfo(
			std::vector<PlPipelineShaderStageCreateInfo> shaderStages,
			std::vector<PlVertexInputBindingDescription> vertexBindingDescriptions,
			std::vector<PlVertexInputAttributeDescription> vertexAttributeDescriptions,
			PlPrimitiveTopology topology,
			bool primitiveRestartEnable,
			PlPipelineRasterizationStateCreateInfo rasterization,
			PlPipelineColorBlendStateCreateInfo colorBlendState,
			PlPipelineDepthStencilStateCreateInfo depthStencilState,
			PlPipelineViewportStateCreateInfo viewPortState,
			PlPipelineMultisampleStateCreateInfo multiSampleState,
			std::vector<PlDynamicState> dynamicStates,
			std::vector<PlPushConstantRange> pushConstants) {

			PlPipelineCreateInfo createInfo{
			shaderStages,
			 vertexBindingDescriptions,
			 vertexAttributeDescriptions,
			 topology,
			 primitiveRestartEnable,
			 rasterization,
			 colorBlendState,
			 depthStencilState,
			 viewPortState,
			 multiSampleState,
			 dynamicStates,
			 pushConstants
			};
			return createInfo;
		}
	}
#pragma endregion
}