#pragma once
#include "Engine/Components/Component.h"
#include "GuiItem.h"

namespace Plaza {
	struct GuiMesh {
		std::vector<glm::vec2> mVertices = std::vector<glm::vec2>();
		std::vector<unsigned int> mIndices = std::vector<unsigned int>();
	};

	class GuiComponent : public Component {
	public:
		std::unordered_map<uint64_t, std::shared_ptr<GuiItem>> mGuiItems = std::unordered_map<uint64_t, std::shared_ptr<GuiItem>>();

		template<typename T, typename = std::enable_if_t<std::is_base_of<GuiItem, T>::value>>
		std::shared_ptr<T> NewGuiItem(std::string newGuiName, uint64_t parentUuid) {
			auto item = std::make_shared<T>(newGuiName);
			item->mGuiParentUuid = parentUuid;
			mGuiItems.emplace(item->mGuiUuid, item);
			return item;
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of<GuiItem, T>::value>>
		std::shared_ptr<T> NewGuiItem(std::shared_ptr<T> item) {
			mGuiItems.emplace(item->mGuiUuid, item);
			if (item->mGuiParentUuid != 0)
				mGuiItems[item->mGuiParentUuid]->mGuiChildren.push_back(item->mGuiUuid);
			return item;
		}

		template<typename T>
		std::shared_ptr<T> GetGuiItem(uint64_t uuid) {
			if (this->HasGuiItem(uuid))
				return mGuiItems.at(uuid);
			else
				return nullptr;
		}

		bool HasGuiItem(uint64_t uuid) {
			return mGuiItems.find(uuid) != mGuiItems.end();
		}

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Component>(this), PL_SER(mGuiItems));
		}
	};
}