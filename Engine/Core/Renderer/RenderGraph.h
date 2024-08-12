#pragma once 
#include "PlazaShaders.h"
#include "Texture.h"

namespace Plaza {
	class PlazaShadersBinding {
	public:
		std::string mName;
		uint64_t mDescriptorCount = 1;
		PlRenderStage mStage = PL_STAGE_ALL;
		uint8_t mLocation = 0;
		uint8_t mBinding = 0;
		PlBindingType mBindingType;
		uint64_t mMaxBindlessResources = 0;

		virtual void Compile() {};
		virtual void Destroy() {};
	};

	class PlazaBufferBinding : public PlazaShadersBinding {
	public:
		PlazaBufferBinding(uint64_t descriptorCount, uint8_t binding, PlBufferType type, PlRenderStage stage, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, const std::string& name)
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

		PlBufferType mType = PL_BUFFER_UNIFORM_BUFFER;
		uint64_t mStride = 0;
		uint64_t mBufferSize = 0;
		uint64_t mItemCount = 0;
		uint64_t mMaxItems = 0;
		uint8_t mBufferCount;
		virtual void Compile() {};
		virtual void Destroy() {};

		template<typename T>
		T* SetMemoryProperties(uint32_t newProperties) {
			mMemoryProperties = static_cast<PlMemoryProperty>(newProperties);
			return dynamic_cast<T*>(this);
		}
		template<typename T>
		T* SetBufferUsage(uint32_t newUsage) {
			mBufferUsage = static_cast<PlBufferUsage>(newUsage);
			return dynamic_cast<T*>(this);
		}
		template<typename T>
		T* SetMemoryUsage(uint32_t newUsage) {
			mMemoryUsage = static_cast<PlMemoryUsage>(newUsage);
			return dynamic_cast<T*>(this);
		}

		PlMemoryProperty GetMemoryProperty() {
			return mMemoryProperties;
		}
		PlBufferUsage GetBufferUsage() {
			return mBufferUsage;
		}
		PlMemoryUsage GetMemoryUsage() {
			return mMemoryUsage;
		}
	private:
		PlBufferUsage mBufferUsage{};
		PlMemoryUsage mMemoryUsage{};
		PlMemoryProperty mMemoryProperties{};
	};

	class PlazaTextureBinding : public PlazaShadersBinding {
	public:
		PlazaTextureBinding(uint64_t descriptorCount, uint8_t location, uint8_t binding, PlBufferType bufferType, PlRenderStage renderStage, PlImageLayout initialLayout, std::shared_ptr<Texture> texture)
			: mBufferType(bufferType) {
			mLocation = location;
			mBinding = binding;
			mDescriptorCount = descriptorCount;
			mBindingType = PL_BINDING_TEXTURE;
			mStage = renderStage;
			mTexture = texture;
			mInitialLayout = initialLayout;
		}

		PlBufferType mBufferType = PL_BUFFER_SAMPLER;
		PlImageLayout mInitialLayout = PL_IMAGE_LAYOUT_UNDEFINED;

		virtual void Compile() {};
		virtual void Destroy() {};

		std::shared_ptr<Texture> mTexture = nullptr;
	private:
	};

	class PlazaRenderGraph;
	class PlazaRenderPass {
	public:
		PlazaRenderPass(std::string name, int stage, PlRenderMethod renderMethod) : mName(name), mStage(stage), mRenderMethod(renderMethod) {}

		std::string mName = "";
		int mStage = 0;
		PlRenderMethod mRenderMethod = PL_RENDER_FULL_SCREEN_QUAD;

		std::vector<std::shared_ptr<PlazaPipeline>> mPipelines = std::vector<std::shared_ptr<PlazaPipeline>>();
		std::vector<PlPipelineCreateInfo> mPipelinesCreateInfo = std::vector<PlPipelineCreateInfo>();
		std::map<std::string, shared_ptr<PlazaShadersBinding>> mInputBindings = std::map<std::string, shared_ptr<PlazaShadersBinding>>();
		std::map<std::string, shared_ptr<PlazaShadersBinding>> mOutputBindings = std::map<std::string, shared_ptr<PlazaShadersBinding>>();
		std::function<void(PlazaRenderGraph*, PlazaRenderPass*)> mCallback = [](PlazaRenderGraph*, PlazaRenderPass*) {};

		virtual void Compile() {};
		virtual void Execute(PlazaRenderGraph* renderGraph) {
			mCallback(renderGraph, this);
			switch (mRenderMethod) {
			case PL_RENDER_FULL_SCREEN_QUAD:
				this->RenderFullScreenQuad(renderGraph);
				break;
			case PL_RENDER_INDIRECT_BUFFER:
				this->RenderIndirectBuffer(renderGraph);
				break;
			}
		};
		virtual void BindRenderPass() {};

		virtual void RenderIndirectBuffer(PlazaRenderGraph* plazaRenderGraph) { };
		virtual void RenderFullScreenQuad(PlazaRenderGraph* plazaRenderGraph) { };
		virtual void CompilePipeline(PlPipelineCreateInfo createInfo) {};

		void AddPipeline(PlPipelineCreateInfo createInfo) {
			mPipelinesCreateInfo.push_back(createInfo);
		};

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

	private:
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
					if (mCompiledBindings.find(bindingName) == mCompiledBindings.end()) {
						binding->Compile();
						mCompiledBindings.insert(bindingName);
					}
				}

				for (auto& [bindingName, binding] : pass->mOutputBindings) {
					if (mCompiledBindings.find(bindingName) == mCompiledBindings.end()) {
						binding->Compile();
						mCompiledBindings.insert(bindingName);
					}
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

		void AddTexture(std::shared_ptr<Texture> texture) {
			mTextures.emplace(texture->mName, texture);
		}

		template<typename T>
		T* GetTexture(std::string name) {
			assert(mTextures.find(name) != mTextures.end());
			return dynamic_cast<T*>(mTextures.at(name).get());
		}

		virtual void AddPipeline() {};
		virtual void CreatePipeline(PlPipelineCreateInfo createInfo) {};

		//template<typename T>
		std::shared_ptr<Texture> GetSharedTexture(std::string name) {
			assert(mTextures.find(name) != mTextures.end());
			return mTextures.at(name);//dynamic_cast<std::shared_ptr<T*>>(mTextures.at(name));
		}

		std::map<std::string, std::shared_ptr<PlazaRenderPass>> mPasses = std::map<std::string, std::shared_ptr<PlazaRenderPass>>();
		std::map<std::string, std::shared_ptr<PlazaShadersBinding>> mShadersBindings = std::map<std::string, std::shared_ptr<PlazaShadersBinding>>();
		std::set<std::string> mCompiledBindings = std::set<std::string>();

		static std::vector<PlVertexInputBindingDescription> VertexGetBindingDescription() {
			std::vector<PlVertexInputBindingDescription> bindingDescriptions{};
			bindingDescriptions.push_back(pl::vertexInputBindingDescription(0, sizeof(Vertex), PL_VERTEX_INPUT_RATE_VERTEX));
			bindingDescriptions.push_back(pl::vertexInputBindingDescription(1, sizeof(glm::vec4) * 4, PL_VERTEX_INPUT_RATE_INSTANCE));
			return bindingDescriptions;
		}

		static std::vector<PlVertexInputAttributeDescription> VertexGetAttributeDescriptions() {
			std::vector<PlVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(0, 0, PL_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(1, 0, PL_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(2, 0, PL_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(3, 0, PL_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(4, 1, PL_FORMAT_R32G32B32A32_SFLOAT, 0));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(5, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 4));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(6, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 8));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(7, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 12));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(8, 0, PL_FORMAT_R32G32B32A32_SINT, offsetof(Vertex, boneIds)));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(9, 0, PL_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weights)));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(10, 0, PL_FORMAT_R32_UINT, offsetof(Vertex, materialIndex)));
			return attributeDescriptions;
		}

	private:
		std::map<std::string, std::shared_ptr<Texture>> mTextures = std::map<std::string, std::shared_ptr<Texture>>();

	};
}