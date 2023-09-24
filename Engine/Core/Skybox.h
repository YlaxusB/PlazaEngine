#pragma once
#include "Engine/Vendor/stb/stb_image.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fileSystem>
#include "Engine/Vendor/filesystem/filesys.h"

#include "Engine/Shaders/Shader.h"
namespace Plaza {
	class Skybox {
	public:
		static Shader* skyboxShader;
		static unsigned int vao, vbo, cubemapTexture;

		static void Init() {

			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			for (unsigned int i = 0; i < sizeof(skyboxVertices) / sizeof(float); i++) {
				skyboxVertices[i] = skyboxVertices[i] * 200;
			}

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

#ifdef GAME_REL
			std::string faceDirectory = Application->projectPath + "\\DefaultAssets\\Skybox\\";
#else
			std::string faceDirectory = Application->enginePath + "\\DefaultAssets\\Skybox\\";
#endif // GAME_REL


			std::vector<std::string> faces
			{
				faceDirectory + "right.jpg",
				faceDirectory + "left.jpg",
				faceDirectory + "top.jpg",
				faceDirectory + "bottom.jpg",
				faceDirectory + "front.jpg",
				faceDirectory + "back.jpg",
			};

			stbi_set_flip_vertically_on_load(false);
			cubemapTexture = loadCubemap(faces);
			//Shader skyboxShader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\skybox\\skyboxVertex.glsl", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\skybox\\skyboxFragment.glsl");
			//skyboxShader.use();
			//skyboxShader.setInt("skybox", 0);
			skyboxShader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			skyboxShader->setInt("skybox", 0);
		}
		static void Update() {
			static constexpr tracy::SourceLocationData __tracy_source_location92{ "Skybox", __FUNCTION__, "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Core\\Skybox.h", (uint32_t)92, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location92, true);
			//Shader& skyboxShader = skyboxShader;
			// Render Skybox
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glStencilMask(0xFF);
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader->use();
			glm::mat4 view = glm::mat4(glm::mat3(Application->activeCamera->GetViewMatrix())); // remove translation from the view matrix
			//glm::mat4 projection = glm::perspective(glm::radians(90.0f), (Application->appSizes->sceneSize.x / Application->appSizes->sceneSize.y), Application->activeCamera->nearPlane, Application->activeCamera->farPlane);;//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
			glm::mat4 projection = Application->activeCamera->GetProjectionMatrix();//glm::perspective(glm::radians(activeCamera->Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 10000.0f);
			skyboxShader->setMat4("view", view);
			skyboxShader->setMat4("projection", projection);
			// skybox cube
			glBindVertexArray(vao);

			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);

			glStencilMask(0xFF);
		}
		static void Terminate() {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
		}

	private:
		static unsigned int loadCubemap(vector<std::string> faces)
		{
			unsigned int textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			int width, height, nrChannels;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return textureID;
		}
	};
}
inline Shader* Plaza::Skybox::skyboxShader = nullptr;
inline unsigned int Plaza::Skybox::vao = 0;
inline unsigned int Plaza::Skybox::vbo = 0;
inline unsigned int Plaza::Skybox::cubemapTexture = 0;