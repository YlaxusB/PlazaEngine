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
		PL_BUFFER_STORAGE_BUFFER
	};

	enum PlazaTextureFormat {
		PL_FORMAT_R32G32B32A32_SFLOAT = 1 << 0,
		PL_FORMAT_R32G32B32_SFLOAT = 1 << 1,
		PL_FORMAT_R8G8B8A8_SFLOAT = 1 << 2,
		PL_FORMAT_R8G8B8_SFLOAT = 1 << 3,
	};

	enum PlazaRenderStage {
		PL_STAGE_VERTEX = 1 << 0,
		PL_STAGE_FRAGMENT = 1 << 1,
		PL_STAGE_COMPUTE = 1 << 2
	};

	class PlazaShadersBinding {
	public:
		std::string mName;
		uint8_t mLocation = 0;
		uint8_t mBinding = 0;

		virtual void Compile() {};
	};

	class PlazaBufferBinding : public PlazaShadersBinding {
	public:
		PlazaBufferBinding(const std::string& name, uint8_t location, uint8_t binding, PlazaBufferType type)
			: mType(type) {
			mName = name;
			mLocation = location;
			mBinding = binding;
		}

		PlazaBufferType mType = PL_BUFFER_UNIFORM_BUFFER;
		uint64_t mStride = 0;
		uint64_t mBufferSize = 0;
		uint64_t mItemCount = 0;
		uint64_t mMaxItems = 0;

		virtual void Compile() {};
		virtual void Destroy() {};
	};

	class PlazaTextureBinding : public PlazaShadersBinding {
	public:
		PlazaTextureBinding(const std::string& name, uint8_t location, uint8_t binding, PlazaTextureType type, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount)
			: mType(type), mFormat(format), mResolution(resolution), mMipCount(mipCount), mLayersCount(layersCount) {
			mName = name;
			mLocation = location;
			mBinding = binding;
		}

		PlazaTextureType mType = PL_TYPE_2D;
		PlazaTextureFormat mFormat = PL_FORMAT_R8G8B8A8_SFLOAT;
		glm::vec3 mResolution = glm::vec3(1, 1, 1);
		uint8_t mMipCount = 1;
		uint16_t mLayersCount = 1;

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
	};

	class PlazaRenderGraph {
	public:
		virtual void BuildDefaultRenderGraph() {
			this->AddRenderPass(std::make_shared<PlazaRenderPass>("Deferred Geometry Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
				->AddInputResource(std::make_shared<PlazaBufferBinding>("UniformBufferObject", 0, 0, PlazaBufferType::PL_BUFFER_UNIFORM_BUFFER))
				->AddOutputResource(std::make_shared<PlazaTextureBinding>("GPosition", 0, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<PlazaTextureBinding>("GDiffuse", 1, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<PlazaTextureBinding>("GNormal", 2, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<PlazaTextureBinding>("GOthers", 3, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1));

			this->AddRenderPass(std::make_shared<PlazaRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
				->AddInputResource(std::make_shared<PlazaTextureBinding>("GPosition", 0, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddInputResource(std::make_shared<PlazaTextureBinding>("GDiffuse", 0, 1, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddInputResource(std::make_shared<PlazaTextureBinding>("GNormal", 0, 2, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddInputResource(std::make_shared<PlazaTextureBinding>("GOthers", 0, 3, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<PlazaTextureBinding>("SceneTexture", 0, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1));
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
					if (mShadersBindings.find(bindingName) != mShadersBindings.end())
						binding->Compile();
				}
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