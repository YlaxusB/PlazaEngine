#pragma once
namespace Plaza {
	class Prefab : public Component {
	public:
		uint64_t mAssetUuid;
		bool mEditorOnly = true; // Prefabs marked as editor only wont be shipped with the game and thus can't be instantiaded in game
		std::string mPrefabName;
		std::vector<Entity> mEntities = std::vector<Entity>();
		std::multimap<uint64_t, std::shared_ptr<Component>> mComponents = std::multimap<uint64_t, std::shared_ptr<Component>>();

		Prefab(Entity& rootEntity);

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mAssetUuid, mPrefabName, mEntities, mComponents);
		}

	private:

	};
}