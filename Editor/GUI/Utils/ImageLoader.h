#pragma once
#include <stb/stb_image.h>
#include <glad/glad.h>
namespace Plaza::Editor::Utils {
	static unsigned int LoadImageToImGuiTexture(const char* path) {
		int width, height, channels;
		unsigned char* image_data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
		if (!image_data) {
			printf("Failed to load image: %s\n", path);
			return 0;
		}

		// Create an ImGui texture from the loaded image data
		unsigned int textureId = 0; // Initialize to 0 to ensure it won't use an existing texture ID
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Free the loaded image data
		stbi_image_free(image_data);
		return textureId;
	}
}