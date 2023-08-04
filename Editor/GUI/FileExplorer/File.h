#pragma once
#include "Editor/GUI/Style/EditorStyle.h"
#include "Editor/GUI/guiMain.h"


namespace Engine {
	namespace Editor {
		class File {
		public:
			std::string name;
			std::string directory;
			std::string extension;

			File() = default;
			File(const File&) = default;
			~File() = default;
		};

		class IconTexture {
		public:
			unsigned int id;
			string extension; // The extension that will have this image
			IconTexture(unsigned int newId, string newExtension) : id(newId), extension(newExtension) {};
		};

		class Icon {
		public:
			static float iconSize; // Adjust the size as needed
			static float spacing; // Adjust the spacing between icons as needed
			static ImVec2 currentPos;
			static std::vector<IconTexture> textures;
			static void Init() {
				unsigned int imageTextureID = 0;
				std::filesystem::path currentPath(__FILE__);
				std::string projectDirectory = currentPath.parent_path().parent_path().parent_path().string();
				std::cout << projectDirectory << std::endl;
				// Folder Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/folderIcon.png").c_str(), &imageTextureID);
				textures.push_back(IconTexture(imageTextureID, ""));

				// Icon for unsupported extensions
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/blankFileIcon.png").c_str(), &imageTextureID);
				textures.push_back(IconTexture(imageTextureID, ".notFound"));

				// Back button Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/backArrowIcon.png").c_str(), &imageTextureID);
				textures.push_back(IconTexture(imageTextureID, ".back"));

				// Yaml Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/yamlIcon.png").c_str(), &imageTextureID);
				textures.push_back(IconTexture(imageTextureID, ".yaml"));

				// Obj Icon
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/objIcon.png").c_str(), &imageTextureID);
				textures.push_back(IconTexture(imageTextureID, ".obj"));
			}
			static void Update(std::string directory, std::string extension, std::string fileName, File file);

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