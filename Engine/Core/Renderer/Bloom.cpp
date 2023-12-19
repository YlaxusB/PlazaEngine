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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->id, 0);
	}

	ComputeShader* Bloom::mBloomComputeShader = nullptr;
	ComputeShader* Bloom::mBloomBlendComputeShader = nullptr;
	ComputeShader* Bloom::mBloomBrightSeparatorComputeShader = nullptr;
	int Bloom::mBloomPasses = 5;
	vector<TexturePair> Bloom::mBloomTextures = vector<TexturePair>();
	Texture2D* Bloom::mBlendedTexture = new Texture2D();
	void Bloom::Init() {
		Bloom::UpdateBloomTexturesSize();
	}

	void Bloom::UpdateBloomTexturesSize() {
		mBloomTextures.clear();
		mBlendedTexture = new Texture2D(Application->appSizes->sceneSize, GL_RGBA32F);

		glm::vec2 currentRes = (Application->appSizes->sceneSize / glm::vec2(2));
		for (int i = 0; i < mBloomPasses; ++i) {
			mBloomTextures.push_back(TexturePair(Texture2D(currentRes, GL_RGBA32F), Texture2D(currentRes, GL_RGBA32F)));

			currentRes = (currentRes / glm::vec2(2));
		}
		currentRes = (currentRes * glm::vec2(4));
		for (int i = 0; i < mBloomPasses; ++i) {
			mBloomTextures.push_back(TexturePair(Texture2D(currentRes, GL_RGBA32F), Texture2D(currentRes, GL_RGBA32F)));

			currentRes = (currentRes * glm::vec2(2));
		}
	}

	void Bloom::DrawBloom() {
		mBloomComputeShader->use();

		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::bloomFrameBuffer->buffer);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
		glBindImageTexture(0, Application->hdrSceneColor, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Renderer::bloomFrameBuffer->colorBuffer);
		glBindImageTexture(1, Renderer::bloomFrameBuffer->colorBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glm::vec2 lastSize = Application->appSizes->sceneSize;
		mBloomComputeShader->setBool("firstPass", true);
		for (int i = 0; i < mBloomTextures.size(); i++) {
			/* Get the input texture */
			Texture2D inputTexture;
			Texture2D outputTexture;
			if (i > 0) {
				inputTexture = mBloomTextures[i - 1].texture2;
				outputTexture = mBloomTextures[i].texture1;
			}
			else
			{
				inputTexture = Texture2D();
				inputTexture.size = Application->appSizes->sceneSize;
				inputTexture.format = GL_RGBA32F;
				inputTexture.id = Application->hdrBloomColor;
				outputTexture = mBloomTextures[i].texture2;
			}



			/* Set the input and output textures */
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, inputTexture.id);
			glBindImageTexture(0, inputTexture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, outputTexture.id);
			glBindImageTexture(1, outputTexture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

			glm::vec2 dispatchSize = inputTexture.size;
			bool upscale;
			if (outputTexture.size.x + outputTexture.size.y > lastSize.x + lastSize.y) {
				dispatchSize = outputTexture.size;
				upscale = true;
				mBloomComputeShader->setBool("upscale", true);
			}
			else
			{
				upscale = false;
				mBloomComputeShader->setBool("upscale", false);
			}


			mBloomComputeShader->setVec2("inputResolution", inputTexture.size);
			mBloomComputeShader->setVec2("outputResolution", outputTexture.size);

			mBloomComputeShader->setBool("horizontal", true);

			/* First Horizontal pass*/
			glDispatchCompute(dispatchSize.x, dispatchSize.y, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			if (i > 0) {
				/* Prepare for second pass */
				inputTexture = mBloomTextures[i].texture1;
				outputTexture = mBloomTextures[i].texture2;

				/* Copy the output to the input texture */
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, inputTexture.id);
				glBindImageTexture(0, inputTexture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, outputTexture.id);
				glBindImageTexture(1, outputTexture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
				mBloomComputeShader->setBool("horizontal", false);

				mBloomComputeShader->setVec2("inputResolution", outputTexture.size);
				mBloomComputeShader->setVec2("outputResolution", outputTexture.size);

				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
				/* Second Vertical pass*/
				glDispatchCompute(outputTexture.size.x, outputTexture.size.y, 1);
				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			}
			lastSize = outputTexture.size;
			mBloomComputeShader->setBool("firstPass", false);
		}

		Bloom::BlendSceneWithBloom();

		glCopyImageSubData(
			mBlendedTexture->id, GL_TEXTURE_2D, 0, 0, 0, 0,
			Renderer::bloomFrameBuffer->colorBuffer, GL_TEXTURE_2D, 0, 0, 0, 0,
			Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 1
		);



		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::bloomFrameBuffer->buffer);
	}

	void Bloom::BlendSceneWithBloom() {
		Bloom::mBloomBlendComputeShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
		glBindImageTexture(0, Application->hdrSceneColor, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mBloomTextures.back().texture2.id);
		glBindImageTexture(1, mBloomTextures.back().texture2.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mBlendedTexture->id);
		glBindImageTexture(2, mBlendedTexture->id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glDispatchCompute(Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 1);
		glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
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