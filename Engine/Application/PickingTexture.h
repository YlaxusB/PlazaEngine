#pragma once

#include "ThirdParty/GLFW/glfw3.h"
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"

#include "ThirdParty/stb/stb_image.h"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw.h"
#include "ThirdParty/imgui/imgui_impl_opengl3.h"

#include "Engine/Application/Application.h"

class PickingTexture {
public:
	unsigned int framebufferId;
	unsigned int pickingTextureId;
	unsigned int depthTextureId;
	bool init(int width, int height) {
		glGenFramebuffers(1, &framebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
		// create a color attachment texture

		glGenTextures(1, &pickingTextureId);
		glBindTexture(GL_TEXTURE_2D, pickingTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, pickingTextureId, 0);

		// Create the texture object for the depth buffer
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &depthTextureId);
		glBindTexture(GL_TEXTURE_2D, depthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureId, 0);

		// Disable reading
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_COLOR_ATTACHMENT2);

		// Unbind the texture and framebuffer
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	void updateSize(glm::vec2 newSize) {
		glBindTexture(GL_TEXTURE_2D, pickingTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, newSize.x, newSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

		glBindTexture(GL_TEXTURE_2D, depthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, newSize.x, newSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	void enableWriting() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferId);
	}

	void disableWriting() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	uint64_t readPixel(int x, int y) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferId);
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		uint32_t pixels[3];
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixels);
		uint32_t value1 = pixels[0]; // Assuming value1 is the first pixel value
		uint32_t value2 = pixels[1];
		uint64_t reconstructedValue = (static_cast<uint64_t>(value2) << 32) | value1;
		std::cout << reconstructedValue << "\n";

		return reconstructedValue;
	}

	/// <summary>
	/// Generates the picking texture, every object is rendered and each pixel have its id
	/// </summary>
	void GenerateTexture();

	/// <summary>
	/// Render the scene with Object ID
	/// </summary>
	/// <param name="shader"></param>
	void Render(Shader& shader);
};