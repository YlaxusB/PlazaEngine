#include "Engine/Core/PreCompiledHeaders.h"
#include "File.h"
namespace Engine {
	namespace Editor {
		std::vector<unsigned int> Icon::textures = std::vector<unsigned int>();
		float Icon::iconSize = 75.0f;
		float Icon::spacing = 5.0f;
		ImVec2 Icon::currentPos = ImVec2(-1.0f, 1.0f);
	}
}