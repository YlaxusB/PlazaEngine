#pragma once

namespace Plaza {
	class Model : public Asset {
	public:
		Model() : Asset() {
			mAssetUuid = Plaza::UUID::NewUUID();
		}

		Mesh* GetMesh(uint64_t uuid) {
			if (mMeshes.find(uuid) != mMeshes.end()) {
				return mMeshes.at(uuid).get();
			}
			return nullptr;
		}

		void AddMeshes(const std::vector<std::shared_ptr<Mesh>>& newMeshes) {
			for (const std::shared_ptr<Mesh>& mesh : newMeshes) {
				mMeshes.emplace(mesh->uuid, mesh);
			}
		}

		Entity* GetEntity(uint64_t uuid) {
			if (mEntities.find(uuid) != mEntities.end()) {
				return mEntities.at(uuid).get();
			}
			return nullptr;
		}

		void AddEntities(std::vector<std::unique_ptr<Entity>> newEntities) {
			for (std::unique_ptr<Entity>& entity : newEntities) {
				//mEntities.emplace(entity->uuid, entity);
			}
		}

		std::unordered_map<uint64_t, std::unique_ptr<Entity>> mEntities = std::unordered_map<uint64_t, std::unique_ptr<Entity>>();
		std::unordered_map<uint64_t, std::shared_ptr<Mesh>> mMeshes = std::unordered_map<uint64_t, std::shared_ptr<Mesh>>();

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Asset>(this), PL_SER(mMeshes));
		}
	private:
	};
}