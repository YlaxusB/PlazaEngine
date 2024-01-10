#pragma once
#include "Engine/Shaders/ComputeShader.h"
namespace Plaza {
	class Texture2D {
	public:
		GLuint id;
		GLuint format;
		glm::vec2 size;
		int mMipSize = 0;
		Texture2D();
		Texture2D(glm::vec2 resolution, GLuint format);
	};
	struct TexturePair {
		Texture2D texture1; /* Takes scene color (or the texture 3 from the last texture pair), blurs it horizontally and stores in this texture */
		Texture2D texture2; /* Takes the texture 1, blurs it vertically and stores in texture2 */
		//Texture2D texture3; /* Takes the texture 2, downscales it and stores on the texture 3 */
		//TexturePair(Texture2D text1, Texture2D text2, Texture2D text3) : texture1(text1), texture2(text2), texture3(text3) {};
		TexturePair(Texture2D text1, Texture2D text2) : texture1(text1), texture2(text2) {};
		TexturePair() {};
	};
	class Bloom {
	public:
		static OpenGLRenderer* mRenderer;
		static GLuint fbo;
		static ComputeShader* mBloomDownScaleShader;
		static ComputeShader* mBloomUpScaleShader;
		static ComputeShader* mBloomComputeShader;
		static ComputeShader* mBloomBlendComputeShader;
		static ComputeShader* mBloomBrightSeparatorComputeShader;
		static void Init();
		static void UpdateBloomTexturesSize();
		static void DrawBloom();
		static void UpSample();
		static void DownSample();
		static void BlendSceneWithBloom();
		static vector<TexturePair> mBloomTextures;
		static Texture2D* mTemporaryTexture;
		static TexturePair* mFinalTexturePair;
		static Texture2D* mBlendedTexture;
		static int mBloomPasses;
	};
}