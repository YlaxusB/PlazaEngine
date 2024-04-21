#include "Engine/Core/PreCompiledHeaders.h"
#include "FileIcon.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Editor/GUI/Popups/FileExplorerFilePopup.h"
namespace Plaza {
	namespace Editor {
		std::map<std::string, IconTexture> Icon::textures = std::map<std::string, IconTexture>();

		float Editor::File::iconSize = 75.0f;
		float Editor::File::spacing = 5.0f;
		ImVec2 Editor::File::currentPos = ImVec2(-1.0f, 1.0f);
		bool Editor::File::firstFocus = false;
		std::string Editor::File::changingName = "";

		void Icon::LoadImageToImGuiTexture(const char* path, ImTextureID* outTextureID) {
			if (Application->mRenderer->api = RendererAPI::OpenGL)
			{
				int width, height, channels;
				unsigned char* image_data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
				if (!image_data) {
					printf("Failed to load image: %s\n", path);
					return;
				}

				// Create an ImGui texture from the loaded image data
				unsigned int id; // Initialize to 0 to ensure it won't use an existing texture ID
				glGenTextures(1, &id);
				glBindTexture(GL_TEXTURE_2D, id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
				glBindTexture(GL_TEXTURE_2D, 0);

				*outTextureID = (ImTextureID)id;

				// Free the loaded image data
				stbi_image_free(image_data);
			}
			else if (Application->mRenderer->api = RendererAPI::Vulkan) {
				*outTextureID = Application->mRenderer->LoadImGuiTexture(path)->GetImGuiTextureID();
			}
		}
	}
}