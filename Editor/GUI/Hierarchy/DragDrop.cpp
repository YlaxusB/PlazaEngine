#include "Engine/Core/PreCompiledHeaders.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
#include "Engine/Utils/vectorUtils.h"
using namespace Plaza;
void payloadDrop(Plaza::Entity* entity, Plaza::Entity* currentObj, ImVec2 treeNodeMin, ImVec2 treeNodeMax, const ImGuiPayload* payload);
void InsertBefore(Plaza::Entity* payloadObj, Plaza::Entity* currentObj);
void InsertAsChild(Plaza::Entity* payloadObj, Plaza::Entity* currentObj);
void InsertAfter(Plaza::Entity* payloadObj, Plaza::Entity* currentObj);


namespace Plaza::Editor {
	void Gui::Hierarchy::Item::HierarchyDragDrop(Entity& entity, Entity* currentObj, ImVec2 treeNodeMin, ImVec2 treeNodeMax) {
		if (entity.parentUuid && ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName.c_str())) {
				payloadDrop(&entity, currentObj, treeNodeMin, treeNodeMax, payload);
			}
			ImGui::EndDragDropTarget();
		}
	}
}
void payloadDrop(Plaza::Entity* entity, Plaza::Entity* currentObj, ImVec2 treeNodeMin, ImVec2 treeNodeMax, const ImGuiPayload* payload) {
	if (payload->DataSize == sizeof(Plaza::Entity*)) {
		/* The current "object" is the one that the mouse was over when dropped*/
		/* PayloadObj is the dragged object */
		Plaza::Entity* payloadObj = static_cast<Plaza::Entity*>(payload->Data);  // Dereference the pointer			
		std::vector<uint64_t>& currentChildren = currentObj->childrenUuid;
		std::vector<uint64_t>& payloadChildren = payloadObj->childrenUuid;

		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 treeNodePos = treeNodeMin;
		ImVec2 treeNodeSize = ImVec2(treeNodeMax.x - treeNodeMin.x, treeNodeMax.y - treeNodeMin.y);
		ImVec2 treeNodeCenter = ImVec2(treeNodePos.x, treeNodePos.y + treeNodeSize.y / 2);

		if (mousePos.y < treeNodeCenter.y - treeNodeSize.y * 0.15f) { // Top
			InsertBefore(payloadObj, currentObj);
		}
		else if (mousePos.y > treeNodeCenter.y + treeNodeSize.y * 0.15f) { // Bottom

			if (currentObj->childrenUuid.size() > 0) {
				InsertAsChild(payloadObj, currentObj);
			}
			else {
				InsertAfter(payloadObj, currentObj);
			}
		}
		else { // Center
			InsertAsChild(payloadObj, currentObj);
		}
	}
} 

void InsertBefore(Plaza::Entity* payloadObj, Plaza::Entity* currentObj) {
	// Becomes child of the target's parent
	Entity& payloadParent = payloadObj->GetParent();
	Entity& currentObject = currentObj->GetParent();
	payloadObj->GetParent().childrenUuid.erase(std::remove(payloadObj->GetParent().childrenUuid.begin(), payloadObj->GetParent().childrenUuid.end(), payloadObj->uuid), payloadObj->GetParent().childrenUuid.end()); // Erase this object from its old parent
	Application->activeScene->entities.at(payloadObj->uuid).parentUuid = currentObj->parentUuid; // changes his parent to the target's parent
	payloadObj->parentUuid = currentObj->parentUuid;
	int currentObjIt = std::min(Utils::Vector::indexOf(currentObj->GetParent().childrenUuid, currentObj->uuid), 1);
	Application->activeScene->entities.at(payloadObj->GetParent().uuid).childrenUuid.insert(currentObj->GetParent().childrenUuid.begin() + currentObjIt, payloadObj->uuid); // Changes the target position in the vector of the parent's children
	//  PayloadObj index now must be currentObj index - 1, so it comes before

	// Update positions
	payloadObj->GetParent().GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
}

void InsertAfter(Plaza::Entity* payloadObj, Plaza::Entity* currentObj) {
	payloadObj->GetParent().childrenUuid.erase(std::remove(payloadObj->GetParent().childrenUuid.begin(), payloadObj->GetParent().childrenUuid.end(), payloadObj->uuid), payloadObj->GetParent().childrenUuid.end());
	Application->activeScene->entities.at(payloadObj->uuid).parentUuid = currentObj->parentUuid;
	Entity& currentObjParent = currentObj->GetParent();
	currentObj->GetParent().childrenUuid.insert(currentObj->GetParent().childrenUuid.begin() + Utils::Vector::indexOf(currentObj->GetParent().childrenUuid, currentObj->uuid) + 1, payloadObj->uuid);
	payloadObj->GetParent().GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
}

void InsertAsChild(Plaza::Entity* payloadObj, Plaza::Entity* currentObj) {
	payloadObj->GetParent().childrenUuid.erase(std::remove(payloadObj->GetParent().childrenUuid.begin(), payloadObj->GetParent().childrenUuid.end(), payloadObj->uuid), payloadObj->GetParent().childrenUuid.end());
	Application->activeScene->entities.at(payloadObj->uuid).parentUuid = currentObj->uuid;
	currentObj->childrenUuid.insert(currentObj->childrenUuid.begin(), payloadObj->uuid);
	payloadObj->GetParent().GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
}