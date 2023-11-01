#pragma once
#include "Engine/Core/Standards.h"
#include "Editor/GUI/Style/EditorStyle.h"
#include "Editor/GUI/guiMain.h"


namespace Plaza {
	namespace Editor {
		class File {
		public:
			static float iconSize;
			static float spacing;
			static ImVec2 currentPos;
			static bool firstFocus;
			static std::string changingName;

			std::string name;
			std::string directory;
			std::string extension;
			unsigned int textureId;

			File() = default;
			File(const File&) = default;
			~File() = default;
			void Update();
		};

		class IconTexture {
		public:
			unsigned int id;
			string extension; // The extension that will have this image
			IconTexture(unsigned int newId, string newExtension) : id(newId), extension(newExtension) {};
		};

		class Icon {
		public:
			static std::map<std::string, IconTexture> textures;
			static void Init() {
				unsigned int imageTextureID = 0;
				std::filesystem::path currentPath(__FILE__);
				std::string projectDirectory = currentPath.parent_path().parent_path().parent_path().string();
				std::cout << projectDirectory << std::endl;
				// Folder Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/folderIcon.png").c_str(), &imageTextureID);
				textures.emplace("", IconTexture(imageTextureID, ""));

				// Icon for unsupported extensions
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/blankFileIcon.png").c_str(), &imageTextureID);
				textures.emplace(".notFound", IconTexture(imageTextureID, ".notFound"));

				// Back button Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/backArrowIcon.png").c_str(), &imageTextureID);
				textures.emplace(".back", IconTexture(imageTextureID, ".back"));

				// Yaml Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/yamlIcon.png").c_str(), &imageTextureID);
				textures.emplace(".yaml", IconTexture(imageTextureID, ".yaml"));

				// Obj Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/objIcon.png").c_str(), &imageTextureID);
				textures.emplace(".obj", IconTexture(imageTextureID, ".obj"));

				// Model Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/modelIcon.png").c_str(), &imageTextureID);
				textures.emplace(Standards::modelExtName, IconTexture(imageTextureID, Standards::modelExtName));

				// Scene Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/sceneIcon.png").c_str(), &imageTextureID);
				textures.emplace(Standards::sceneExtName, IconTexture(imageTextureID, Standards::sceneExtName));
			}

			static void LoadImageToImGuiTexture(const char* path, unsigned int* out_textureID) {
				int width, height, channels;
				unsigned char* image_data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
				if (!image_data) {
					printf("Failed to load image: %s\n", path);
					return;	
				}

				// Create an ImGui texture from the loaded image data
				*out_textureID = 0; // Initialize to 0 to ensure it won't use an existing texture ID
				glGenTextures(1, out_textureID);
				glBindTexture(GL_TEXTURE_2D, *out_textureID);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
				glBindTexture(GL_TEXTURE_2D, 0);

				// Free the loaded image data
				stbi_image_free(image_data);
			}
		};
	}
}