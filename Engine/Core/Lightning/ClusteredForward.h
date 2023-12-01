#pragma once
#include "Engine/Shaders/ComputeShader.h"
namespace Plaza {
	class Lightning {
	public:
		struct Light {
			glm::vec3 position;
			glm::vec3 color;
			// Add other light properties as needed
			Light(glm::vec3 pos) : position(pos) {};
			Light(glm::vec3 pos, glm::vec3 col) : position(pos), color(col) {};
		};

		struct Cluster {
			int lightIndices[256];
			int lightsCount = 0;
		};
		static GLuint mClustersBuffer;
		static unsigned int frameBuffer, textureColorbuffer, rbo;
		static Shader* mLightMergerShader;
		static Shader* mLightAccumulationShader;
		static ComputeShader* mLightSorterComputeShader;
		static std::vector<Cluster> mClusters;
		static std::vector<Light> mLights;
		static void InitializeClusters(float numberClustersX, float numberClustersY, float numberClustersZ, std::vector<Cluster>& clusters);
		static void AssignLightsToClusters(const std::vector<Light>& lights, std::vector<Cluster>& clusters);
		static void CreateClusterBuffers(const std::vector<Cluster>& clusters);
		static void LightingPass(const std::vector<Cluster>& clusters, const std::vector<Light>& lights);
	};
}
