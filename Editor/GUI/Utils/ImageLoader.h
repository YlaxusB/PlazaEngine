#pragma once
#include <ThirdParty/stb/stb_image.h>
#include <ThirdParty/glad/glad.h>
namespace Plaza::Editor::Utils {
	static ImTextureID LoadImageToImGuiTexture(const char* path) {
		return Application::Get()->mRenderer->LoadImGuiTexture(path)->GetImGuiTextureID();
	}
}