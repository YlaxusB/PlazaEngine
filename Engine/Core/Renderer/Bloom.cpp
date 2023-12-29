#include "Engine/Core/PreCompiledHeaders.h"
#include "Bloom.h"
#include "Engine/Core/Renderer.h"

namespace Plaza {
	Texture2D::Texture2D() {};
	Texture2D::Texture2D(glm::vec2 resolution, GLuint format) {
		this->size = resolution;
		this->format = format;
		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_2D, this->id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);
		//glTextureStorage2D(this->id, 8, GL_RGBA32F, resolution.x, resolution.y);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->id, 0);
	}

	uint8_t calculateMipmapLevels(int m_width, int m_height, int m_max_iterations, int m_downscale_limit)
	{
		uint32_t width = m_width / 2;
		uint32_t height = m_height / 2;
		uint8_t  mip_levels = 1;

		printf("Mip level %d: %d x %d\n", 0, m_width, m_height);
		printf("Mip level %d: %d x %d\n", mip_levels, width, height);

		for (uint8_t i = 0; i < m_max_iterations; ++i)
		{
			width = width / 2;
			height = height / 2;

			if (width < m_downscale_limit || height < m_downscale_limit) break;

			++mip_levels;

			printf("Mip level %d: %d x %d\n", mip_levels, width, height);
		}

		return mip_levels + 1;
	}

	GLuint Bloom::fbo = 0;
	ComputeShader* Bloom::mBloomDownScaleShader = nullptr;
	ComputeShader* Bloom::mBloomUpScaleShader = nullptr;
	ComputeShader* Bloom::mBloomComputeShader = nullptr;
	ComputeShader* Bloom::mBloomBlendComputeShader = nullptr;
	ComputeShader* Bloom::mBloomBrightSeparatorComputeShader = nullptr;
	int Bloom::mBloomPasses = 7;
	vector<TexturePair> Bloom::mBloomTextures = vector<TexturePair>();
	Texture2D* Bloom::mTemporaryTexture = new Texture2D();
	TexturePair* Bloom::mFinalTexturePair = nullptr;
	Texture2D* Bloom::mBlendedTexture = new Texture2D();
	void Bloom::Init() {
		Bloom::UpdateBloomTexturesSize();

	}

	void Bloom::UpdateBloomTexturesSize() {
		mBloomTextures.clear();
		mBlendedTexture = new Texture2D(Application->appSizes->sceneSize, GL_RGBA32F);

		mTemporaryTexture = new Texture2D(Application->appSizes->sceneSize, GL_RGBA32F);

		mFinalTexturePair = new TexturePair();
		mFinalTexturePair->texture1 = *new Texture2D();
		mFinalTexturePair->texture1.size = Application->appSizes->sceneSize; 
		mFinalTexturePair->texture1.mMipSize = calculateMipmapLevels(Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 16, 10);

		glCreateFramebuffers(1, &Bloom::fbo);

		glCreateTextures(GL_TEXTURE_2D, 1, &mFinalTexturePair->texture1.id);
		glTextureStorage2D(mFinalTexturePair->texture1.id, mFinalTexturePair->texture1.mMipSize, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y); // internalformat = GL_RGB32F

		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLuint m_rbo_id = 0;
		glCreateRenderbuffers(1, &m_rbo_id);
		glNamedRenderbufferStorage(m_rbo_id, GL_DEPTH24_STENCIL8, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);

		glNamedFramebufferTexture(Bloom::fbo, GL_COLOR_ATTACHMENT0, mFinalTexturePair->texture1.id, 0);
		glNamedFramebufferRenderbuffer(Bloom::fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo_id);

		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(mFinalTexturePair->texture1.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glGenFramebuffers(1, &Bloom::fbo);
		//glBindFramebuffer(GL_FRAMEBUFFER, Bloom::fbo);
		//
		//
		//
		////mFinalTexturePair = new TexturePair(Texture2D(Application->appSizes->sceneSize, GL_RGBA32F), Texture2D(Application->appSizes->sceneSize, GL_RGBA32F));
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, mFinalTexturePair->texture1.id);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFinalTexturePair->texture1.id, 0);
		//GLenum geometryAttachments[1] = { GL_COLOR_ATTACHMENT0 };
		//glDrawBuffers(1, geometryAttachments);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glm::vec2 currentRes = (Application->appSizes->sceneSize / glm::vec2(1));
		for (int i = 1; i < mBloomPasses + 1; ++i) {
			mBloomTextures.push_back(TexturePair(Texture2D(currentRes / glm::vec2(2), GL_RGBA32F), Texture2D(currentRes / glm::vec2(2), GL_RGBA32F)));

			currentRes = (currentRes / glm::vec2(2));
		}
		//currentRes = (currentRes * glm::vec2(1));
		//for (int i = 0; i < mBloomPasses; ++i) {
		//	mBloomTextures.push_back(TexturePair(Texture2D(currentRes, GL_RGBA32F), Texture2D(currentRes, GL_RGBA32F), Texture2D(currentRes * glm::vec2(2), GL_RGBA32F)));
		//
		//	currentRes = (currentRes * glm::vec2(2));
		//}
	}

	void Bloom::DownSample() {

	}

	void Bloom::UpSample() {

	}

	void Bloom::DrawBloom() {
		glBindFramebuffer(GL_FRAMEBUFFER, Bloom::fbo);
		glViewport(0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mBloomComputeShader->use();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glBindFramebuffer(GL_FRAMEBUFFER, Bloom::fbo);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		//Renderer::CopyFrameBufferColor(Application->hdrFramebuffer, Bloom::fbo);
		glCopyImageSubData(
			Application->hdrSceneColor, GL_TEXTURE_2D, 0, 0, 0, 0,
			mFinalTexturePair->texture1.id, GL_TEXTURE_2D, 0, 0, 0, 0,
			Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 1
		);

		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::bloomFrameBuffer->buffer);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);


		/* Bind the textures */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
		glBindImageTexture(0, Application->hdrSceneColor, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Renderer::bloomFrameBuffer->colorBuffer);
		glBindImageTexture(1, Renderer::bloomFrameBuffer->colorBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Bloom::mTemporaryTexture->id);
		glBindImageTexture(2, Bloom::mTemporaryTexture->id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);



		glm::vec2 lastSize = Application->appSizes->sceneSize;
		mBloomComputeShader->setBool("firstPass", true);



		float mThreshold = 1.5f;
		float mKnee = 0.1f;
		float m_bloom_intensity = 16.0f;
		float m_bloom_dirt_intensity = 1.0f;
		/* DownSample */
		mBloomDownScaleShader->use();
		mBloomDownScaleShader->setVec4("u_threshold", glm::vec4(mThreshold, mThreshold - mKnee, 2.0f * mKnee, 0.25f * mKnee));
		//m_tmo_ps->rt->bindTexture();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, mFinalTexturePair->texture1.id);
		glBindTextureUnit(0, mFinalTexturePair->texture1.id);
		glm::uvec2 mip_size = glm::uvec2(mFinalTexturePair->texture1.size.x / 2, mFinalTexturePair->texture1.size.y / 2);

		/* Bloom: downscale */
		for (uint8_t i = 0; i < mFinalTexturePair->texture1.mMipSize - 1; ++i)
		{
			mBloomDownScaleShader->setVec2("u_texel_size", 1.0f / glm::vec2(mip_size));
			mBloomDownScaleShader->setInt("u_mip_level", i);
			mBloomDownScaleShader->setBool("u_use_threshold", i == 0);

			glBindImageTexture(0, mFinalTexturePair->texture1.id, i + 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			//m_tmo_ps->rt->bindImageForWrite(IMAGE_UNIT_WRITE, i + 1);

			glDispatchCompute(glm::ceil(float(mip_size.x) / 8), glm::ceil(float(mip_size.y) / 8), 1);

			mip_size = mip_size / 2u;

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		}

		/* Bloom: upscale */
		mBloomUpScaleShader->use();
		mBloomUpScaleShader->setFloat("u_bloom_intensity", m_bloom_intensity);
		mBloomUpScaleShader->setFloat("u_dirt_intensity", m_bloom_dirt_intensity);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, mFinalTexturePair->texture1.id);
		glBindTextureUnit(0, mFinalTexturePair->texture1.id);
		//m_bloom_dirt_texture->Bind(1);

		for (uint8_t i = mFinalTexturePair->texture1.mMipSize - 1; i >= 1; --i)
		{
			mip_size.x = glm::max(1.0, glm::floor(float(Application->appSizes->sceneSize.x) / glm::pow(2.0, i - 1)));
			mip_size.y = glm::max(1.0, glm::floor(float(Application->appSizes->sceneSize.y) / glm::pow(2.0, i - 1)));

			mBloomUpScaleShader->setVec2("u_texel_size", 1.0f / glm::vec2(mip_size));
			mBloomUpScaleShader->setInt("u_mip_level", i);

			glBindImageTexture(0, mFinalTexturePair->texture1.id, i - 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			//m_tmo_ps->rt->bindImageForReadWrite(IMAGE_UNIT_WRITE, i - 1);

			glDispatchCompute(glm::ceil(float(mip_size.x) / 8), glm::ceil(float(mip_size.y) / 8), 1);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		}

		//mBloomComputeShader->setVec2("inputResolution", currentRes);
		//mBloomComputeShader->setVec2("outputResolution", currentRes * glm::vec2(2));
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Bloom::mBloomTextures.front().texture2.id);
		//glBindImageTexture(0, Bloom::mBloomTextures.front().texture2.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, Bloom::mFinalTexturePair->texture2.id);
		//glBindImageTexture(1, Bloom::mFinalTexturePair->texture2.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//glDispatchCompute(currentRes.x / 8, currentRes.y / 8, 1);
		//glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

		Bloom::BlendSceneWithBloom();

		glCopyImageSubData(
			mBlendedTexture->id, GL_TEXTURE_2D, 0, 0, 0, 0,
			Renderer::bloomFrameBuffer->colorBuffer, GL_TEXTURE_2D, 0, 0, 0, 0,
			Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 1
		);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::bloomFrameBuffer->buffer);


	}

	void Bloom::BlendSceneWithBloom() {
		Bloom::mBloomBlendComputeShader->use();
		glBindImageTexture(0, Application->hdrSceneColor, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Bloom::mFinalTexturePair->texture1.id);
		glBindImageTexture(3, Bloom::mFinalTexturePair->texture1.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mBlendedTexture->id);
		glBindImageTexture(2, mBlendedTexture->id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glDispatchCompute(Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

/*
		bool readFirst = true;
		int downScalePasses = 4;
		int upscalePasses = 4;
		int factor = 2;
		glm::vec2 currentRes = Application->appSizes->sceneSize;
		mBloomComputeShader->setBool("upscale", false);
		for (int i = 0; i < downScalePasses; ++i) {
			mBloomComputeShader->setBool("readFirst", readFirst);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
			glBindImageTexture(0, Application->hdrSceneColor, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Renderer::bloomFrameBuffer->colorBuffer);
			glBindImageTexture(1, Renderer::bloomFrameBuffer->colorBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glDispatchCompute(currentRes.x, currentRes.y, 1);
			glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
			readFirst = !readFirst;
			currentRes /= 2;
		}
		currentRes *= 2;
		mBloomComputeShader->setVec2("currentRes", currentRes);

		mBloomComputeShader->setBool("upscale", true);
		for (int i = 0; i < upscalePasses; ++i) {
			mBloomComputeShader->setBool("readFirst", readFirst);
			mBloomComputeShader->setBool("blur", false);
			mBloomComputeShader->setVec2("currentRes", currentRes);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
			glBindImageTexture(0, Application->hdrSceneColor, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			// Create a new texture with the desired size
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentRes.x, currentRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			// Copy the data from the original texture
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, currentRes.x / 2, currentRes.y /2, 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Renderer::bloomFrameBuffer->colorBuffer);
			glBindImageTexture(1, Renderer::bloomFrameBuffer->colorBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			// Create a new texture with the desired size
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentRes.x, currentRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			// Copy the data from the original texture
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, currentRes.x / 2, currentRes.y / 2, 0);

			mBloomComputeShader->setBool("blur", true);
			glDispatchCompute(currentRes.x, currentRes.y, 1);
			glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
			readFirst = !readFirst;
			currentRes *= 2;
		}

*/