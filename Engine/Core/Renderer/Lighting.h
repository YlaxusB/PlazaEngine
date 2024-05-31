#pragma once
#include "Renderer.h"

namespace Plaza {
	class Lighting {
	public:
		struct LightStruct {
			glm::vec3 color;
			float radius;
			glm::vec3 position;
			float intensity;
		};

		struct Tile {
			int lightIndices[256];
			int lightsCount = 0;
			glm::vec3 minBounds;
			glm::vec3 maxBounds;
		};

		struct Plane {
			glm::vec3 Normal;
			float Distance;
		};

		struct Frustum {
			Plane planes[4];
		};

		virtual void Init() = 0;
		virtual void GetLights() = 0;
		virtual void UpdateTiles() = 0;
		virtual void Terminate() = 0;

		glm::vec2 sceneSize = glm::vec2(0.0f);
		int mLightsSize;
		std::vector<Tile> mTiles;
		std::vector<LightStruct> mLights;
	};
}