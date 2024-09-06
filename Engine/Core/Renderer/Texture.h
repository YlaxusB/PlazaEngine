#pragma once
#include <string>
#include <vector>
#include "RendererTypes.h"
#include "Engine/Core/AssetsManager/Asset.h"

namespace Plaza {
	struct TextureInfo {
	public:
		uint64_t mDescriptorCount = 1;
		PlTextureType mTextureType = PL_TYPE_2D;
		PlViewType mViewType = PL_VIEW_TYPE_2D;
		PlTextureFormat mFormat = PL_FORMAT_R8G8B8A8_UNORM;
		PlImageUsage mImageUsage = PL_IMAGE_USAGE_COLOR_ATTACHMENT;
		uint16_t mLayersCount = 1;
		PlImageLayout mInitialLayout = PL_IMAGE_LAYOUT_UNDEFINED;
		std::string mPath = "";
		PlSamplerAddressMode mSamplerAddressMode = PL_SAMPLER_ADDRESS_MODE_REPEAT;
		bool mIsHdr = false;

		bool operator==(const TextureInfo& other) const {
			return mDescriptorCount == other.mDescriptorCount &&
				mTextureType == other.mTextureType &&
				mViewType == other.mViewType &&
				mFormat == other.mFormat &&
				mImageUsage == other.mImageUsage &&
				mLayersCount == other.mLayersCount &&
				mInitialLayout == other.mInitialLayout &&
				mPath == other.mPath &&
				mSamplerAddressMode == other.mSamplerAddressMode &&
				mIsHdr == other.mIsHdr;
		}
	};

	struct TextureInfoHash {
		std::size_t operator()(const TextureInfo& info) const {
			std::size_t hash = std::hash<uint64_t>{}(info.mDescriptorCount);
			hash ^= std::hash<int>{}(info.mTextureType) << 1;
			hash ^= std::hash<int>{}(info.mViewType) << 2;
			hash ^= std::hash<int>{}(info.mFormat) << 3;
			hash ^= std::hash<int>{}(info.mImageUsage) << 4;
			hash ^= std::hash<uint16_t>{}(info.mLayersCount) << 5;
			hash ^= std::hash<int>{}(info.mInitialLayout) << 6;
			hash ^= std::hash<std::string>{}(info.mPath) << 7;
			hash ^= std::hash<int>{}(info.mSamplerAddressMode) << 8;
			hash ^= std::hash<bool>{}(info.mIsHdr) << 9;
			return hash;
		}
	};

	class Texture : public Asset {
	public:
		glm::vec4 rgba = glm::vec4(1.0f);
		int mIndexHandle = -1;
		float mIntensity = 1.0f;
		glm::vec3 mResolution = glm::vec3(1, 1, 1);
		uint8_t mMipCount = 1;

		std::vector<PlImageLayout> mFutureLayouts = std::vector<PlImageLayout>();

		Texture() {
			this->SetTextureInfo(TextureInfo{});
		}
		Texture(glm::vec4 rgba, float intensity = 1.0f) {
			this->rgba = rgba;
			this->mIntensity = intensity;
			this->SetTextureInfo(TextureInfo{});
		}
		Texture(uint64_t descriptorCount, PlImageUsage imageUsage, PlTextureType imageType, PlViewType viewType, PlTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name) {
			this->SetTextureInfo(TextureInfo{ descriptorCount, imageType, viewType, format, imageUsage, layersCount, PL_IMAGE_LAYOUT_UNDEFINED, "", PL_SAMPLER_ADDRESS_MODE_REPEAT, false });
			mResolution = resolution;
			mMipCount = mipCount;
			mAssetName = name;
		}

		bool SameAs(Texture& other) {
			return (
				this->mAssetPath == other.mAssetPath &&
				this->rgba == other.rgba
				);
		}

		virtual void Load(std::string relativePath = "") {}
		virtual VkDescriptorSet GetDescriptorSet() {
			return VkDescriptorSet();
		}

		//// Assignment operator
		//Texture& operator=(const Texture& other) {
		//	if (this != &other) {
		//		path = other.path;
		//		rgba = other.rgba;
		//	}
		//	return *this;
		//}

		bool IsTextureEmpty() const {
			return false;
		}

		virtual unsigned int GetTextureID() {
			return 0;
		}

		virtual ImTextureID GetImGuiTextureID() {
			return 0;
		}

		virtual void Destroy() {
			return;
		}

		~Texture() = default;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mAssetUuid, rgba);
		}

		void SetTextureInfo(TextureInfo textureInfo) {
			mTextureInfo = Texture::CreateTextureInfo(textureInfo);
		};
		const TextureInfo& GetTextureInfo() { return *mTextureInfo; };
	private:
		TextureInfo* mTextureInfo = nullptr;


	public:
		/* Static */
		static TextureInfo* CreateTextureInfo(TextureInfo info) {
			if (!Texture::HasTextureInfo(info))
				mTexturesInfo.emplace(info);
			return Texture::GetTextureInfo(info);
		}
		static bool HasTextureInfo(TextureInfo& info) {
			return mTexturesInfo.find(info) != mTexturesInfo.end();
		}
		static TextureInfo* GetTextureInfo(TextureInfo& info) {
			auto it = mTexturesInfo.find(info);
			if (it != mTexturesInfo.end()) {
				return const_cast<TextureInfo*>(&(*it));
			}
			return nullptr;
		}
	private:
		static inline std::unordered_set<TextureInfo, TextureInfoHash> mTexturesInfo = std::unordered_set<TextureInfo, TextureInfoHash>();
	};
}