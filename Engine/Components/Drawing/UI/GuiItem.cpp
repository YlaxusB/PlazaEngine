#include "GuiItem.h"
#include "Engine/Core/Scene.h"

namespace Plaza {
	GuiComponent* GuiItem::GetOwnerComponent() {
		return Scene::GetActiveScene()->GetComponent<GuiComponent>(this->mComponentUuid);
	}

	void GuiItem::SetPosition(glm::vec2 newPosition) {
		mPosition = newPosition;
		glm::mat3 parentMatrix = this->GetOwnerComponent()->HasGuiItem(mGuiParentUuid) ? GetOwnerComponent()->GetGuiItem<GuiItem>(mGuiParentUuid)->mTransform : glm::mat3(1.0f);
		GuiItem::UpdateSelfAndChildrenTransform(this, parentMatrix);
	}

	void GuiItem::SetScale(glm::vec2 newScale) {
		mScale = newScale;
		glm::mat3 parentMatrix = this->GetOwnerComponent()->HasGuiItem(mGuiParentUuid) ? GetOwnerComponent()->GetGuiItem<GuiItem>(mGuiParentUuid)->mTransform : glm::mat3(1.0f);
		GuiItem::UpdateSelfAndChildrenTransform(this, parentMatrix);
	}

	void GuiItem::SetSize(glm::vec2 newSize) {
		mSize = newSize;
		mRecalculateVertices = true;
	}

	void GuiItem::UpdateSelfAndChildrenTransform(GuiItem* item, glm::mat3& parentTransform) {
		item->mTransform = parentTransform * item->mTransform;
		for (uint64_t& uuid : item->mGuiChildren) {
			GuiItem::UpdateSelfAndChildrenTransform(Scene::GetActiveScene()->GetComponent<GuiComponent>(item->mComponentUuid)->GetGuiItem<GuiItem>(uuid).get(), item->mTransform);
		}
	}
}