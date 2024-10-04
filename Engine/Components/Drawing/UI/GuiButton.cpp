#include "Engine/Core/PreCompiledHeaders.h"
#include "GuiButton.h"

namespace Plaza {
	void GuiButton::CallScriptsCallback() {
		for (auto& [key, value] : mScriptsFunctionNameToCallWhenClicked) {
			if (!Scene::GetActiveScene()->HasComponent<CsScriptComponent>(key))
				continue;

			for (auto& [scriptKey, scriptValue] : Scene::GetActiveScene()->GetComponent<CsScriptComponent>(key)->scriptClasses)
				Mono::CallMethod(scriptValue->monoObject, value);
		}
	}

	bool GuiButton::MouseIsInsideButton(glm::vec2 mousePos) {
#ifdef EDITOR_MODE
		mousePos.x -= Application::Get()->appSizes->hierarchySize.x;
		mousePos.y -= Application::Get()->appSizes->sceneImageStart.y;
#endif
		glm::vec2 worldPosition = this->GetWorldPosition();
		glm::vec2 size = this->GetLocalSize();

		if (mousePos.x > worldPosition.x - size.x && mousePos.x < worldPosition.x + size.x && mousePos.y > worldPosition.y - size.y && mousePos.y < worldPosition.y + size.y)
			return true;
		else
			return false;
	}
}