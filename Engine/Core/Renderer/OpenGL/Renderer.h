#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Components/Core/Entity.h"

#include "OpenGLMesh.h"
#include "Engine/Core/Renderer/Mesh.h"
#include "Engine/Core/FrameBuffer.h"

namespace Plaza {
	class OpenGLRenderer : public Renderer {
	public:
		RendererAPI api = RendererAPI::OpenGL;

		static OpenGLRenderer* GetRenderer();

		static Shader* mergeShader;
		static Shader* blurShader;
		static FrameBuffer* hdrFramebuffer;
		static FrameBuffer* bloomBlurFrameBuffer;
		static FrameBuffer* bloomFrameBuffer;
		/// <summary>
		/// Init the HDR framebuffer and others
		/// </summary>
		void Init() override;
		void UpdateProjectManager() override;

		void InitShaders(std::string shadersFolder) override;

		void AddInstancesToRender() override;
		void RenderShadowMap(Shader& shader) override;
		void RenderInstances(Shader& shader) override;
		void RenderBloom() override;
		void RenderScreenSpaceReflections() override;
		void RenderFullScreenQuad() override;
		void Destroy() override;

		Shadows* GetShadows() override;
		/// <summary>
		/// Loops through the GameObjects list, if Entity haves a MeshRenderer then it draws it with its transformation
		/// </summary>
		/// <param name="shader"></param>
//		void Render(Shader& shader) override;

		/// <summary>
		/// Loops  through the meshes list of the active scene and Draw all instanced meshes
		/// </summary>
//		void RenderInstances(Shader& shader) override;
//		void RenderInstancesShadowMap(Shader& shader) override;

		/// <summary>
		/// Renders the Outline of the Selected Entity and its children
		/// </summary>
		/// <param name="outlineShader"></param>
		void RenderOutline() override;

		/// <summary>
		/// Renders a fullscreen quad with HDR using the scene color buffer
		/// </summary>
		void RenderHDR() override;

		/// <summary>
		/// Renders a fullscreen quad blurring all fragments that its color exceeds a clamp, then merges it into the scene buffer
		/// </summary>
//		void RenderBloom() override;

		/// <summary>
		/// <p>Renders a full quad on the screen with a Shader</p>
		/// <p>Optimal for Post-Processing</p>
		/// </summary>
//		void RenderFullScreenQuad() override;

		void CopyLastFramebufferToFinalDrawBuffer() override;

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

		void InitGUI() override;
		void NewFrameGUI() override;
		void UpdateGUI() override;
		ImTextureID GetFrameImage() override;

		Mesh& CreateNewMesh(
			vector<glm::vec3> vertices,
			vector<glm::vec3> normals,
			vector<glm::vec2> uvs,
			vector<glm::vec3> tangent,
			vector<glm::vec3> bitangent,
			vector<unsigned int> indices,
			Material& material,
			bool usingNormal,
			vector<BonesHolder> bonesHolder = vector<BonesHolder>()) override;

		Mesh* RestartMesh(Mesh* mesh) override;
		void DrawRenderGroupInstanced(RenderGroup* renderGroup) override;
		Texture* LoadTexture(std::string path, uint64_t uuid = 0) override;
	private:
		static unsigned int pingpongFBO[2];
		static unsigned int pingpongColorbuffers[2];
	};
}

