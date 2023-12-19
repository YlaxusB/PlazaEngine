#pragma once
#include "Engine/Shaders/ComputeShader.h"
namespace Plaza {
	class Texture2D {
	public:
		GLuint id;
		GLuint format;
		glm::vec2 size;
		Texture2D();
		Texture2D(glm::vec2 resolution, GLuint format);
	};
	struct TexturePair {
		Texture2D texture1;
		Texture2D texture2;
		TexturePair(Texture2D text1, Texture2D text2) : texture1(text1), texture2(text2) {};
	};
	class Bloom {
	public:
		static ComputeShader* mBloomComputeShader;
		static ComputeShader* mBloomBlendComputeShader;
		static ComputeShader* mBloomBrightSeparatorComputeShader;
		static void Init();
		static void UpdateBloomTexturesSize();
		static void DrawBloom();
		static void BlendSceneWithBloom();
		static vector<TexturePair> mBloomTextures;
		static Texture2D* mBlendedTexture;
		static int mBloomPasses;
	};
}