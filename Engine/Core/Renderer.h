#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Components/Core/GameObject.h"
namespace Engine {
	class Renderer {
	public:
		/// <summary>
		/// Loops through the GameObjects list, if GameObject haves a MeshRenderer then it draws it with its transformation
		/// </summary>
		/// <param name="shader"></param>
		static void Render(Shader& shader);

		/// <summary>
		/// Renders the Outline of the Selected GameObject and its children
		/// </summary>
		/// <param name="outlineShader"></param>
		static void RenderOutline(Shader outlineShader);

	private:
		static void BlurBuffer();
	};
}

