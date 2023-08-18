#include "Engine/Core/PreCompiledHeaders.h"
#include "HierarchyPopup.h"

#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Editor/GUI/Popups/NewEntityPopup.h"
GameObject* obj = nullptr;
namespace Engine::Editor {
	void HierarchyPopup::Update() {
		if (ImGui::BeginPopupContextWindow())
		{
			Popup::NewEntityPopup::Init(obj);

			ImGui::EndPopup();

		}
	}
}