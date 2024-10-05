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
		mousePos.y -= Application::Get()->appSizes->sceneImageStart.y + 35;
#endif
		glm::vec2 worldPosition = this->GetLocalPosition();
		glm::vec2 size = this->GetLocalSize() / glm::vec2(2.0f, 1.0f);

		bool insideX = mousePos.x >= worldPosition.x - size.x && mousePos.x <= worldPosition.x + size.x;
		bool insideY = mousePos.y >= worldPosition.y && mousePos.y <= worldPosition.y + size.y;

		if (insideX && insideY)
			return true;
		else
			return false;
	}
}