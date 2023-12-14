#pragma once
#include "Engine/Shaders/ComputeShader.h"
namespace Plaza {
	class Lighting {
	public:
		struct LightStruct {
			glm::vec3 color;
			float radius;
			glm::vec3 position;
			float intensity;
		};

		struct Cluster {
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

		static int mLightsSize;
		static GLuint mClustersBuffer;
		static GLuint mLightsBuffer;
		static GLuint mLightsBuffer2;
		static unsigned int frameBuffer, textureColorbuffer, rbo;
		static Shader* mLightMergerShader;
		static Shader* mLightAccumulationShader;
		static ComputeShader* mLightSorterComputeShader;
		static std::vector<Cluster> mClusters;
		static std::vector<LightStruct> mLights;
		static void InitializeClusters(float numberClustersX, float numberClustersY, float numberClustersZ, std::vector<Cluster>& clusters);
		static void AssignLightsToClusters(const std::vector<LightStruct>& lights, std::vector<Cluster>& clusters);
		static void CreateClusterBuffers(const std::vector<Cluster>& clusters);
		static void LightingPass(const std::vector<Cluster>& clusters, const std::vector<LightStruct>& lights);
		static void UpdateBuffers();
	};
}
