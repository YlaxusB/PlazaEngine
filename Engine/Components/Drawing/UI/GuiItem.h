#pragma once
#include "Gui.h"
#include <vector>

namespace Plaza {
	class GuiComponent;
	class GuiItem {
	public:
		uint64_t mGuiUuid = 0;
		std::string mGuiName = "";
		uint64_t mComponentUuid = 0;
		//GuiMesh mGuiMesh;

		bool mRecalculateVertices = true;

		uint64_t mGuiParentUuid = 0;
		std::vector<uint64_t> mGuiChildren = std::vector<uint64_t>();

		GuiItem(std::string guiName) : mGuiName(guiName) {
			mGuiUuid = Plaza::UUID::NewUUID();
		}

		GuiItem() { mGuiUuid = Plaza::UUID::NewUUID(); }

		GuiComponent* GetOwnerComponent();

		glm::vec2 GetPosition() { return mPosition; }
		glm::vec2 GetScale() { return mPosition; }

		void SetPosition(glm::vec2 newPosition);
		void SetScale(glm::vec2 newScale);
		void SetSize(glm::vec2 newSize);

		static void UpdateSelfAndChildrenTransform(GuiItem* item, glm::mat3& parentTransform);

	private:
		glm::vec2 mPosition = glm::vec2(0.0f);
		glm::vec2 mScale = glm::vec2(1.0f);
		glm::mat3 mTransform = glm::mat3(1.0f);
		glm::vec2 mSize = glm::vec2(1.0f);
	public:
		template <class Archive>
		void serialize(Archive& archive) {
			archive(PL_SER(mGuiUuid), PL_SER(mGuiName));
		}
	};
}