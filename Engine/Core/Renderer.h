#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Components/Core/Entity.h"

namespace Plaza {
	class Renderer {
	public:
		static FrameBuffer* hdrFramebuffer;
		/// <summary>
		/// Init the HDR framebuffer and others
		/// </summary>
		static void Init();
		/// <summary>
		/// Loops through the GameObjects list, if Entity haves a MeshRenderer then it draws it with its transformation
		/// </summary>
		/// <param name="shader"></param>
		static void Render(Shader& shader);

		/// <summary>
		/// Loops  through the meshes list of the active scene and Draw all instanced meshes
		/// </summary>
		static void RenderInstances(Shader& shader);
		static void RenderInstancesShadowMap(Shader& shader);

		/// <summary>
		/// Renders the Outline of the Selected Entity and its children
		/// </summary>
		/// <param name="outlineShader"></param>
		static void RenderOutline(Shader outlineShader);

		/// <summary>
		/// Renders the plane with HDR using the scene color buffer
		/// </summary>
		static void RenderHDR();

		/// <summary>
		/// <p>Renders a full quad on the screen with a Shader</p>
		/// <p>Optimal for Post-Processing</p>
		/// </summary>
		static void RenderQuadOnScreen();

		static unsigned int quadVAO;
		static unsigned int quadVBO;
		static void InitQuad();
	private:
		static void BlurBuffer();
	};
}

