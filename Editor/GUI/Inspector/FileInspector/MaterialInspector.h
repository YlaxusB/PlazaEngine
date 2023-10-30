#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/Material.h"
#include "Editor/GUI/Utils/Utils.h"

namespace Plaza::Editor {
	static class MaterialFileInspector {
	public:
		MaterialFileInspector(File* file) {
			ImGui::Text(file->directory.c_str());
		}
	};
}