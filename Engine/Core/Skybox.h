#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fileSystem>
#include "ThirdParty/filesystem/filesys.h"

#include "Engine/Shaders/Shader.h"
namespace Plaza {
	class OpenGLSkybox {
	public:
		static Shader* skyboxShader;
		static unsigned int vao, vbo, cubemapTexture;

		static void Init() {

		}
		static void Update() {

		}
		static void Terminate() {

		}

	private:
		static unsigned int loadCubemap(vector<std::string> faces)
		{
			unsigned int textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			int width, height, nrChannels;
			return textureID;
		}
	};
}
inline Shader* Plaza::OpenGLSkybox::skyboxShader = nullptr;
inline unsigned int Plaza::OpenGLSkybox::vao = 0;
inline unsigned int Plaza::OpenGLSkybox::vbo = 0;
inline unsigned int Plaza::OpenGLSkybox::cubemapTexture = 0;