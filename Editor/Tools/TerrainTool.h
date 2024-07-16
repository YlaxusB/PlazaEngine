#pragma once

namespace Plaza {
	namespace Editor {
		class TerrainTool : public Tool {
			void CreateTerrainEntity() {
				Entity* entity = new Entity();
				Scene::GetActiveScene();
			}
		};
	}
}