#include "Engine/Core/PreCompiledHeaders.h"
#include "ClusteredForward.h"
#include "Engine/Core/Renderer.h"

namespace Plaza {
	GLuint Lightning::mClustersBuffer = 0;
	unsigned int Lightning::frameBuffer, Lightning::textureColorbuffer, Lightning::rbo = 0;
	Shader* Lightning::mLightAccumulationShader = nullptr;
	Shader* Lightning::mLightMergerShader = nullptr;
	ComputeShader* Lightning::mLightSorterComputeShader = nullptr;
	std::vector<Lightning::Cluster> Lightning::mClusters = std::vector<Cluster>();
	std::vector<Lightning::Light> Lightning::mLights = std::vector<Light>();
	void Lightning::InitializeClusters(float numberClustersX, float numberClustersY, float numberClustersZ, std::vector<Cluster>& clusters) {
		std::string shadersFolder = Application->enginePath;
		mLightAccumulationShader = new Shader((shadersFolder + "\\Shaders\\ClusteredForward\\accumulationVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\ClusteredForward\\accumulationFragment.glsl").c_str());
		// Clear any existing data in clusters
		clusters.clear();

		// Calculate the size of each cluster in the X, Y, and Z directions
		float clusterSizeX = Application->appSizes->sceneSize.x / numberClustersX /* calculate based on screen size and numClustersX */;
		float clusterSizeY = Application->appSizes->sceneSize.y / numberClustersY/* calculate based on screen size and numClustersY */;
		float clusterSizeZ = numberClustersZ;//* calculate based on screen size and numClustersZ */;

		// Iterate over clusters and initialize them
		for (int i = 0; i < numberClustersX; ++i) {
			for (int j = 0; j < numberClustersY; ++j) {
				Cluster cluster;
				clusters.push_back(cluster);
			}
		}
	}

	void Lightning::AssignLightsToClusters(const std::vector<Light>& lights, std::vector<Cluster>& clusters) {
		// Clear existing light indices in clusters

		// Iterate over lights and assign them to clusters based on intersection
		for (int lightIndex = 0; lightIndex < lights.size(); ++lightIndex) {
			const Light& light = lights[lightIndex];

			// Find the clusters that the light intersects
			for (int clusterIndex = 0; clusterIndex < clusters.size(); ++clusterIndex) {
				const Cluster& cluster = clusters[clusterIndex];

				// Check if the light intersects with the cluster
				//if (light.position.x >= cluster.minBounds.x && light.position.x < cluster.maxBounds.x &&
				//	light.position.y >= cluster.minBounds.y && light.position.y < cluster.maxBounds.y &&
				//	light.position.z >= cluster.minBounds.z && light.position.z < cluster.maxBounds.z) {
				//	// Assign the light to the cluster
				//	clusters[clusterIndex].lightIndices.push_back(lightIndex);
				//}
			}
		}
	}

	void Lightning::CreateClusterBuffers(const std::vector<Cluster>& clusters) {
		int size = 32;
		int clusterCount = glm::round(Application->appSizes->sceneSize.y / size) * glm::round(Application->appSizes->sceneSize.x / size);
		for (int i = 0; i < clusterCount; i++) {
			mClusters.push_back(*new Cluster());
		}
		// Create OpenGL buffers to store information about lights in each cluster

		// Example buffer for light indices in each cluster
		std::vector<int> lightIndexData;
		std::vector<glm::vec3> lightColors;

		// Create and bind buffer for light indices
		GLuint clusterBuffer;
		glGenBuffers(1, &clusterBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterBuffer);

		// Upload data to the buffer
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Cluster) * mClusters.size(), mClusters.data(), GL_STATIC_DRAW);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, clusterBuffer);

		// Unbind the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Create and bind buffer for light indices
		GLuint lightsBuffer;
		glGenBuffers(1, &lightsBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsBuffer);

		// Upload data to the buffer
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * mLights.size(), mLights.data(), GL_STATIC_DRAW);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightsBuffer);

		// Unbind the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		vector<int> mSizes = vector<int>();
		// Create and bind buffer for light indices
		GLuint sizesBuffer;
		glGenBuffers(1, &sizesBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, sizesBuffer);

		// Upload data to the buffer
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * mSizes.size(), mSizes.data(), GL_STATIC_DRAW);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sizesBuffer);

		// Unbind the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Optionally, you can store the buffer ID or any other information in the Cluster structure
		// For example: cluster.clusterBuffer = clusterBuffer;



		ApplicationSizes& appSizes = *Application->appSizes;
		//unsigned int& textureColorbuffer = Application->textureColorbuffer;
		glGenFramebuffers(1, &Lightning::frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Lightning::frameBuffer);
		// create a color attachment texture
		glGenTextures(1, &Lightning::textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, Lightning::textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Lightning::textureColorbuffer, 0);


		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &Lightning::rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, Lightning::rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
		glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Lightning::rbo); // now actually attach it

		// Disable reading
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		//frameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
		//frameBuffer->Init();
		//frameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_RGBA32F, GL_FLOAT, NULL);
		//frameBuffer->InitRenderBufferObject(GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
		//GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		//frameBuffer->DrawAttachments(attachments, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);

		/* Compute Shader */
		GLuint lightsBuffer2;
		glGenBuffers(1, &lightsBuffer2);
		glGenBuffers(1, &mClustersBuffer);

		// Bind the input buffer to the SSBO binding point 0
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsBuffer2);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsBuffer2);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mLights.size() * sizeof(Light), mLights.data(), GL_DYNAMIC_DRAW);

		// Bind the output buffer to the SSBO binding point 1
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mClustersBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mClustersBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, clusterCount * sizeof(Cluster), mClusters.data(), GL_DYNAMIC_DRAW);

		GLuint resultBuffer;
		glGenBuffers(1, &resultBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, resultBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, resultBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, clusterCount * sizeof(glm::vec3), vector<glm::vec3>().data(), GL_DYNAMIC_DRAW);

		GLuint frustumsBuffer;
		glGenBuffers(1, &frustumsBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, frustumsBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, frustumsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, clusterCount * sizeof(Frustum), vector<Frustum>().data(), GL_DYNAMIC_DRAW);
	}

	void Lightning::LightingPass(const std::vector<Cluster>& clusters, const std::vector<Light>& lights) {
		// Set up shaders and framebuffer for the lighting pass

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glDisable(GL_DEPTH_TEST);
		mLightAccumulationShader->use();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		GLuint lightsBindingLocation = glGetUniformLocation(mLightAccumulationShader->ID, "LightsBuffer");
		glProgramUniform1i(mLightAccumulationShader->ID, lightsBindingLocation, 0);
		mLightAccumulationShader->setVec3("viewPos", Application->activeCamera->Position);
		mLightAccumulationShader->setMat4("viewMatrix", Application->activeCamera->GetViewMatrix());
		glEnable(GL_FRONT_AND_BACK);
		for (const Cluster& cluster : clusters) {


		}
		Renderer::RenderQuadOnScreen();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		mLightSorterComputeShader->use();
		mLightSorterComputeShader->setMat4("view", Application->activeCamera->GetViewMatrix());
		mLightSorterComputeShader->setMat4("projection", Application->activeCamera->GetProjectionMatrix());
		//glProgramUniform1i(mLightSorterComputeShader->ID, glGetUniformLocation(mLightSorterComputeShader->ID, "LightsArray"), 1);
		//mLightSorterComputeShader->setBool("first", true);
		//glDispatchCompute(1, 1, 1);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		//mLightSorterComputeShader->setBool("first", false);
		glm::vec2 clusterSize = glm::vec2(32.0f);
		glm::vec2 clusterCount = glm::ceil(Application->appSizes->sceneSize / clusterSize);
		glDispatchCompute(clusterCount.x, clusterCount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


		/* Merge the scene frame buffer with the light frame buffer */
		glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mLightMergerShader->use();
		mLightMergerShader->setVec3("viewPos", Application->activeCamera->Position);
		mLightMergerShader->setFloat("time", glfwGetTime());
		mLightMergerShader->setMat4("view", Application->activeCamera->GetViewMatrix());
		GLuint lightsBindingLocation2 = glGetUniformLocation(mLightMergerShader->ID, "LightsBuffer");
		glProgramUniform1i(mLightMergerShader->ID, lightsBindingLocation2, 0);
		GLuint clustersBindingLocation = glGetUniformLocation(mLightMergerShader->ID, "ClusterBuffer");
		glProgramUniform1i(mLightMergerShader->ID, clustersBindingLocation, 1);
		GLuint clustersBindingLocation3 = glGetUniformLocation(mLightMergerShader->ID, "SizesBuffer");
		glProgramUniform1i(mLightMergerShader->ID, clustersBindingLocation3, 3);
		GLuint frustumsBindingLocation = glGetUniformLocation(mLightMergerShader->ID, "FrustumsBuffer");
		glProgramUniform1i(mLightMergerShader->ID, frustumsBindingLocation, 7);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mClustersBuffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Application->gPosition);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Application->gNormal);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Application->gDiffuse);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Application->gOthers);

		Renderer::RenderQuadOnScreen();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
	}
}