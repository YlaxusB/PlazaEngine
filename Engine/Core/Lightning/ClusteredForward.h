#pragma once
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
			std::vector<int> lightIndices;
			glm::vec3 minBounds;
			glm::vec3 maxBounds;
		};
		static unsigned int frameBuffer, textureColorbuffer, rbo;
		static Shader* mLightMergerShader;
		static Shader* mLightAccumulationShader;
		static std::vector<Cluster> mClusters;
		static std::vector<Light> mLights;
		static void InitializeClusters(float numberClustersX, float numberClustersY, float numberClustersZ, std::vector<Cluster>& clusters);
		static void AssignLightsToClusters(const std::vector<Light>& lights, std::vector<Cluster>& clusters);
		static void CreateClusterBuffers(const std::vector<Cluster>& clusters);
		static void LightingPass(const std::vector<Cluster>& clusters, const std::vector<Light>& lights);
	};
}
