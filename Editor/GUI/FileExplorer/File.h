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

			~File() = default;
		};



		class Icon {
		public:
			static float iconSize; // Adjust the size as needed
			static float spacing; // Adjust the spacing between icons as needed
			static ImVec2 currentPos;
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
				if (currentPos.x == -1.0f) {
					currentPos = ImGui::GetCursorScreenPos();
				}
				if (textures.size() > 0) {
					ImGui::SetCursorScreenPos(currentPos);

					ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing;
					windowFlags |= ImGuiWindowFlags_NoScrollbar;
					if (ImGui::BeginChild(ImGui::GetID(fileName.c_str()), ImVec2(75, 75), false, windowFlags)) {
						ImVec4 backgroundColor = editorStyle.hierarchyBackgroundColor;

						if (ImGui::IsWindowHovered()) {
							backgroundColor = editorStyle.treeNodeHoverBackgroundColor;
						}

						ImDrawList* drawList = ImGui::GetWindowDrawList();
						ImVec2 size = ImGui::imVec2(ImGui::glmVec2(ImGui::GetWindowPos()) + glm::vec2(75, 75));
						drawList->AddRectFilled(ImGui::GetWindowPos(), size, ImGui::ColorConvertFloat4ToU32(backgroundColor));

						ImGui::SetCursorPos( ImGui::imVec2(ImGui::glmVec2(ImGui::GetCursorPos()) + glm::vec2(75 / 2 - 25)));
						ImGui::Image(ImTextureID(textures.front()), ImVec2(50, 50));
						ImGui::SetCursorPos(ImGui::imVec2(ImGui::glmVec2(ImGui::GetCursorPos()) - glm::vec2(0, 10)));
						ImGui::Text(fileName.c_str());
					}
					ImGui::EndChild();
					if (ImGui::IsItemClicked()) {
						std::cout << "eae" << std::endl;
					}


					currentPos.x += iconSize + spacing;
					if (currentPos.x + iconSize > ImGui::GetWindowPos().x + ImGui::GetWindowWidth()) {
						currentPos.x = ImGui::GetCursorScreenPos().x;
						currentPos.y += iconSize + spacing;
					}
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