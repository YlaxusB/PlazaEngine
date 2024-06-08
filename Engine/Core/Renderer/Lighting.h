#pragma once
#include "Renderer.h"

namespace Plaza {
	class Lighting {
	public:
		struct alignas(16) LightStruct {
			glm::vec3 color;
			float radius;
			glm::vec3 position;
			float intensity;
		};

		struct alignas(16) Tile {
			int lightIndices[256];
			int lightsCount = 0;
			glm::vec3 minBounds;
			glm::vec3 maxBounds;
		};

		struct alignas(16) Plane {
			glm::vec3 Normal;
			float Distance;
		};

		struct alignas(16) Frustum {
			Plane planes[4];
		};

		virtual void Init() = 0;
		virtual void GetLights() = 0;
		virtual void UpdateTiles() = 0;
		virtual void Terminate() = 0;

		glm::vec4 directionalLightColor = glm::vec4(1.0f);
		float directionalLightIntensity = 0.8f;
		glm::vec4 ambientLightColor = glm::vec4(1.0f);
		float ambientLightIntensity = 0.3f;
		glm::vec2 mScreenSize = glm::vec2(0.0f);
		int mLightsSize;
		std::vector<Tile> mTiles;
		std::vector<LightStruct> mLights;
	};
}