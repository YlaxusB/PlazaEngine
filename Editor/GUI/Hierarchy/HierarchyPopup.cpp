#include "Engine/Core/PreCompiledHeaders.h"
#include "HierarchyPopup.h"

#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Editor/GUI/Popups/NewEntityPopup.h"
Entity* obj = nullptr;
namespace Plaza::Editor {
	void HierarchyPopup::Update(Scene* scene) {
		if (ImGui::BeginPopupContextWindow())
		{
			Popup::NewEntityPopup::Init(nullptr, scene);

			ImGui::EndPopup();

		}
	}
}