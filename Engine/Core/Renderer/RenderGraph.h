#pragma once 
#include "PlazaShaders.h"

namespace Plaza {
	enum PlazaTextureType {
		PL_TYPE_1D = 0,
		PL_TYPE_2D,
		PL_TYPE_3D
	};

	enum PlazaBufferType {
		PL_BUFFER_PUSH_CONSTANTS = 0,
		PL_BUFFER_UNIFORM_BUFFER,
		PL_BUFFER_STORAGE_BUFFER,
		PL_BUFFER_STORAGE_BUFFER_DYNAMIC,
		PL_BUFFER_SAMPLER,
		PL_BUFFER_COMBINED_IMAGE_SAMPLER
	};

	enum PlazaTextureFormat {
		PL_FORMAT_R32G32B32A32_SFLOAT = 0,
		PL_FORMAT_R32G32B32_SFLOAT,
		PL_FORMAT_R8G8B8A8_UNORM,
		PL_FORMAT_R8G8B8_UNORM,
		PL_FORMAT_D32_SFLOAT_S8_UINT
	};

	enum PlazaImageUsage {
		PL_IMAGE_USAGE_TRANSFER_SRC = 1 << 0,
		PL_IMAGE_USAGE_TRANSFER_DST = 1 << 1,
		PL_IMAGE_USAGE_SAMPLED = 1 << 2,
		PL_IMAGE_USAGE_STORAGE = 1 << 3,
		PL_IMAGE_USAGE_COLOR_ATTACHMENT = 1 << 4,
		PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 5,
		PL_IMAGE_USAGE_INPUT_ATTACHMENT = 1 << 6,
	};

	enum PlazaRenderStage {
		PL_STAGE_VERTEX = 1 << 0,
		PL_STAGE_FRAGMENT = 1 << 1,
		PL_STAGE_COMPUTE = 1 << 2,
		PL_STAGE_ALL = 1 << 3,
	};

	enum PlazaBindingType {
		PL_BINDING_UNDEFINED = 1 << 0,
		PL_BINDING_BUFFER = 1 << 1,
		PL_BINDING_TEXTURE = 1 << 2
	};

	enum PlazaMemoryProperty : uint32_t {
		PL_MEMORY_PROPERTY_DEVICE_LOCAL = 1 << 0,
		PL_MEMORY_PROPERTY_HOST_VISIBLE = 1 << 1,
		PL_MEMORY_PROPERTY_HOST_COHERENT = 1 << 2,
		PL_MEMORY_PROPERTY_HOST_CACHED = 1 << 3,
		PL_MEMORY_PROPERTY_LAZILY_ALLOCATED = 1 << 4,
		PL_MEMORY_PROPERTY_PROTECTED = 1 << 5
	};

	enum PlazaBufferUsage : uint32_t {
		PL_BUFFER_USAGE_TRANSFER_SRC = 1 << 0,
		PL_BUFFER_USAGE_TRANSFER_DST = 1 << 1,
		PL_BUFFER_USAGE_UNIFORM_BUFFER = 1 << 2,
		PL_BUFFER_USAGE_STORAGE_BUFFER = 1 << 3,
		PL_BUFFER_USAGE_INDEX_BUFFER = 1 << 4,
		PL_BUFFER_USAGE_VERTEX_BUFFER = 1 << 5,
		PL_BUFFER_USAGE_INDIRECT_BUFFER = 1 << 6,
	};

	enum PlazaMemoryUsage {
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

	class PlazaShadersBinding {
	public:
		std::string mName;
		uint64_t mDescriptorCount = 1;
		PlazaRenderStage mStage = PL_STAGE_ALL;
		uint8_t mLocation = 0;
		uint8_t mBinding = 0;
		PlazaBindingType mBindingType;
		uint64_t mMaxBindlessResources = 0;

		virtual void Compile() {};
		virtual void Destroy() {};
	};

	class PlazaBufferBinding : public PlazaShadersBinding {
	public:
		PlazaBufferBinding(uint64_t descriptorCount, uint8_t binding, PlazaBufferType type, PlazaRenderStage stage, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, const std::string& name)
			: mType(type) {
			mName = name;
			mBinding = binding;
			mDescriptorCount = descriptorCount;
			mStage = stage;
			mBindingType = PL_BINDING_BUFFER;
			mStride = stride;
			mMaxItems = maxItems;
			mBufferCount = bufferCount;
		}

		PlazaBufferType mType = PL_BUFFER_UNIFORM_BUFFER;
		uint64_t mStride = 0;
		uint64_t mBufferSize = 0;
		uint64_t mItemCount = 0;
		uint64_t mMaxItems = 0;
		uint8_t mBufferCount;
		virtual void Compile() {};
		virtual void Destroy() {};

		template<typename T>
		T* SetMemoryProperties(uint32_t newProperties) {
			mMemoryProperties = static_cast<PlazaMemoryProperty>(newProperties);
			return dynamic_cast<T*>(this);
		}
		template<typename T>
		T* SetBufferUsage(uint32_t newUsage) {
			mBufferUsage = static_cast<PlazaBufferUsage>(newUsage);
			return dynamic_cast<T*>(this);
		}
		template<typename T>
		T* SetMemoryUsage(uint32_t newUsage) {
			mMemoryUsage = static_cast<PlazaMemoryUsage>(newUsage);
			return dynamic_cast<T*>(this);
		}

		PlazaMemoryProperty GetMemoryProperty() {
			return mMemoryProperties;
		}
		PlazaBufferUsage GetBufferUsage() {
			return mBufferUsage;
		}
		PlazaMemoryUsage GetMemoryUsage() {
			return mMemoryUsage;
		}
	private:
		PlazaBufferUsage mBufferUsage{};
		PlazaMemoryUsage mMemoryUsage{};
		PlazaMemoryProperty mMemoryProperties{};
	};

	class PlazaTextureBinding : public PlazaShadersBinding {
	public:
		PlazaTextureBinding(uint64_t descriptorCount, uint8_t location, uint8_t binding, PlazaBufferType bufferType, PlazaImageUsage imageUsage, PlazaRenderStage renderStage, PlazaTextureType textureType, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name)
			: mBufferType(bufferType), mTextureType(textureType), mFormat(format), mResolution(resolution), mMipCount(mipCount), mLayersCount(layersCount) {
			mName = name;
			mLocation = location;
			mBinding = binding;
			mDescriptorCount = descriptorCount;
			mBindingType = PL_BINDING_TEXTURE;
			mStage = renderStage;
			mImageUsage = imageUsage;
		}

		PlazaBufferType mBufferType = PL_BUFFER_SAMPLER;
		PlazaTextureType mTextureType = PL_TYPE_2D;
		PlazaTextureFormat mFormat = PL_FORMAT_R8G8B8A8_UNORM;
		glm::vec3 mResolution = glm::vec3(1, 1, 1);
		uint8_t mMipCount = 1;
		uint16_t mLayersCount = 1;
		PlazaImageUsage mImageUsage{};

		virtual void Compile() {};
		virtual void Destroy() {};
	};

	class PlazaRenderGraph;
	class PlazaRenderPass {
	public:
		PlazaRenderPass(std::string name, int stage) : mName(name), mStage(stage) {}

		std::string mName = "";
		int mStage = 0;
		std::vector<std::shared_ptr<PlazaPipeline>> mPipelines = std::vector<std::shared_ptr<PlazaPipeline>>();
		std::map<std::string, shared_ptr<PlazaShadersBinding>> mInputBindings = std::map<std::string, shared_ptr<PlazaShadersBinding>>();
		std::map<std::string, shared_ptr<PlazaShadersBinding>> mOutputBindings = std::map<std::string, shared_ptr<PlazaShadersBinding>>();
		std::function<void(PlazaRenderGraph*, PlazaRenderPass*)> mCallback = [](PlazaRenderGraph*, PlazaRenderPass*) {};

		virtual void Compile() {};
		virtual void Execute(PlazaRenderGraph* renderGraph) { mCallback(renderGraph, this); };
		virtual void BindRenderPass() {};

		void SetRecordingCallback(std::function<void(PlazaRenderGraph*, PlazaRenderPass*)> callback) {
			mCallback = callback;
		}

		PlazaRenderPass* AddInputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mInputBindings.emplace(resource->mName, resource);
			return this;
		}

		PlazaRenderPass* AddOutputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mOutputBindings.emplace(resource->mName, resource);
			return this;
		}

		template<typename T>
		T* GetInputResource(std::string name) {
			assert(mInputBindings.find(name) != mInputBindings.end());
			return dynamic_cast<T*>(mInputBindings.at(name).get());
		}
	};

	class PlazaRenderGraph {
	public:
		virtual void BuildDefaultRenderGraph() {
			//this->AddRenderPass(std::make_shared<PlazaRenderPass>("Deferred Geometry Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 0, PlazaBufferType::PL_BUFFER_UNIFORM_BUFFER, PL_STAGE_ALL, "UniformBufferObject"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 20, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "TexturesBuffer"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 6, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "samplerBRDFLUT"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 7, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "prefilterMap"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 8, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "irradianceMap"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 9, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "shadowsDepthMap"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 19, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "materialsBuffer"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 1, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "boneMatrices"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 2, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "renderGroupOffsetsBuffer"))
			//	->AddInputResource(std::make_shared<PlazaBufferBinding>(1, 3, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "renderGroupMaterialsOffsetsBuffer"))
			//	->AddOutputResource(std::make_shared<PlazaTextureBinding>(1, 0, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GPosition"))
			//	->AddOutputResource(std::make_shared<PlazaTextureBinding>(1, 1, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GDiffuse"))
			//	->AddOutputResource(std::make_shared<PlazaTextureBinding>(1, 2, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GNormal"))
			//	->AddOutputResource(std::make_shared<PlazaTextureBinding>(1, 3, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GOthers"));
			//
			//this->AddRenderPass(std::make_shared<PlazaRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
			//	->AddInputResource(std::make_shared<PlazaTextureBinding>(1, 0, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GPosition"))
			//	->AddInputResource(std::make_shared<PlazaTextureBinding>(1, 0, 1, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GDiffuse"))
			//	->AddInputResource(std::make_shared<PlazaTextureBinding>(1, 0, 2, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GNormal"))
			//	->AddInputResource(std::make_shared<PlazaTextureBinding>(1, 0, 3, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "GOthers"))
			//	->AddOutputResource(std::make_shared<PlazaTextureBinding>(1, 0, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "SceneTexture"));
		}

		virtual void Execute(uint8_t imageIndex, uint8_t currentFrame) {};
		void ExecuteRenderPasses() {
			for (auto& [key, value] : mPasses) {
				value->mCallback;
			}
		}
		void Compile() {
			for (auto& [passName, pass] : mPasses) {

				for (auto& [bindingName, binding] : pass->mInputBindings) {
					if (mShadersBindings.find(bindingName) == mShadersBindings.end())
						binding->Compile();
				}

				for (auto& [bindingName, binding] : pass->mOutputBindings) {
					if (mShadersBindings.find(bindingName) == mShadersBindings.end())
						binding->Compile();
				}

				pass->Compile();
			}
		}

		virtual bool BindPass(std::string passName) { return false; };

		PlazaRenderPass* AddRenderPass(std::shared_ptr<PlazaRenderPass> newRenderPass) {
			mPasses.emplace(newRenderPass->mName, newRenderPass);
			return mPasses[newRenderPass->mName].get();
		}

		void AddRenderPassCallback(std::string passName, std::function<void(PlazaRenderGraph*, PlazaRenderPass*)> callback) {
			if (mPasses.find(passName) != mPasses.end())
				mPasses[passName]->SetRecordingCallback(callback);
		}

		std::map<std::string, std::shared_ptr<PlazaRenderPass>> mPasses = std::map<std::string, std::shared_ptr<PlazaRenderPass>>();
		std::map <std::string, std::shared_ptr<PlazaShadersBinding>> mShadersBindings = std::map <std::string, std::shared_ptr<PlazaShadersBinding>>();
	};
}