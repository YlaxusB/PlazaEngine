#pragma once
namespace Engine {
	namespace Editor {
		class File {
		public:
			std::string name;
			std::string directory;
			std::string extension;

			~File() = default;
		};


		class Icon {
		public:
			static std::vector<unsigned int> textures;
			static void Init() {
				unsigned int imageTextureID = 0;
				std::filesystem::path currentPath(__FILE__);
				std::string projectDirectory = currentPath.parent_path().parent_path().parent_path().string();
				std::cout << projectDirectory << std::endl;
				LoadImageToImGuiTexture((projectDirectory + "/Images/FileIcons/folderIcon.png").c_str(), &imageTextureID);
				textures.push_back(imageTextureID);
			}
			static void Update(std::string directory, std::string extension, std::string fileName) {
				if (textures.size() > 0) {
					ImGui::ImageButton(fileName.c_str(), ImTextureID(textures.front()), ImVec2(50, 50));
					ImGui::Text(fileName.c_str());
				}
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