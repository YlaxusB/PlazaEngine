#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Components/Core/Entity.h"

namespace Plaza {
	class VulkanRenderer : public Renderer {
	public:
		void Init() override;
		void InitShaders(std::string shadersFolder) override;
		void AddInstancesToRender() override;
		void RenderShadowMap(Shader& shader) override;
		void RenderInstances(Shader& shader) override;
		void RenderBloom() override;
		void RenderScreenSpaceReflections() override;
		void RenderFullScreenQuad() override;
		void RenderOutline() override;
		void RenderHDR() override;
		void CopyLastFramebufferToFinalDrawBuffer() override;
	private:

	};
	/*
	class VulkanRenderer : public Renderer {
	public:
		static Shader* mergeShader;
		static Shader* blurShader;
		static FrameBuffer* hdrFramebuffer;
		static FrameBuffer* bloomBlurFrameBuffer;
		static FrameBuffer* bloomFrameBuffer;
		/// <summary>
		/// Init the HDR framebuffer and others
		/// </summary>
		void Init();
		/// <summary>
		/// Loops through the GameObjects list, if Entity haves a MeshRenderer then it draws it with its transformation
		/// </summary>
		/// <param name="shader"></param>
		void Render(Shader& shader);

		/// <summary>
		/// Loops  through the meshes list of the active scene and Draw all instanced meshes
		/// </summary>
		void RenderInstances(Shader& shader);
		void RenderInstancesShadowMap(Shader& shader);

		/// <summary>
		/// Renders the Outline of the Selected Entity and its children
		/// </summary>
		/// <param name="outlineShader"></param>
		static void RenderOutline(Shader outlineShader);

		/// <summary>
		/// Renders a fullscreen quad with HDR using the scene color buffer
		/// </summary>
		static void RenderHDR();

		/// <summary>
		/// Renders a fullscreen quad blurring all fragments that its color exceeds a clamp, then merges it into the scene buffer
		/// </summary>
		static void RenderBloom();

		/// <summary>
		/// <p>Renders a full quad on the screen with a Shader</p>
		/// <p>Optimal for Post-Processing</p>
		/// </summary>
		static void RenderQuadOnScreen();

		static unsigned int quadVAO;
		static unsigned int quadVBO;
		static void InitQuad();

		static void BlurBuffer(GLint colorBuffer, int passes);
		/// <summary>
		/// Merge two textures
		/// </summary>
		/// <param name="outBuffer"></param>
		/// <param name="colorBuffer"></param>
		static void MergeColors(GLint texture1, GLint texture2);

		static void CopyFrameBufferColor(GLint readBuffer, GLint drawBuffer);
	private:
		static unsigned int pingpongFBO[2];
		static unsigned int pingpongColorbuffers[2];
	};
	*/
}

