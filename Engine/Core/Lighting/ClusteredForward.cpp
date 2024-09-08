#include "Engine/Core/PreCompiledHeaders.h"
#include "ClusteredForward.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Components/Rendering/Light.h"
#include <vector>

namespace Plaza {
     int ClusteredLighting::mLightsSize = 0;
     GLuint ClusteredLighting::mClustersBuffer = 0;
     GLuint ClusteredLighting::mLightsBuffer = 0;
     GLuint ClusteredLighting::mLightsBuffer2 = 0;
     unsigned int ClusteredLighting::frameBuffer, ClusteredLighting::textureColorbuffer, ClusteredLighting::rbo = 0;
     Shader* ClusteredLighting::mLightAccumulationShader = nullptr;
     Shader* ClusteredLighting::mLightMergerShader = nullptr;
     ComputeShader* ClusteredLighting::mLightSorterComputeShader = nullptr;
     std::vector<ClusteredLighting::Cluster> ClusteredLighting::mClusters = std::vector<Cluster>();
     std::vector<ClusteredLighting::LightStruct> ClusteredLighting::mLights = std::vector<LightStruct>();
     void ClusteredLighting::InitializeClusters(float numberClustersX, float numberClustersY, float numberClustersZ, std::vector<Cluster>& clusters) {
          std::string shadersFolder = Application::Get()->enginePath;
          mLightAccumulationShader = new Shader((shadersFolder + "\\Shaders\\ClusteredForward\\accumulationVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\ClusteredForward\\accumulationFragment.glsl").c_str());
          // Clear any existing data in clusters
          clusters.clear();

          // Calculate the size of each cluster in the X, Y, and Z directions
          float clusterSizeX = Application::Get()->appSizes->sceneSize.x / numberClustersX /* calculate based on screen size and numClustersX */;
          float clusterSizeY = Application::Get()->appSizes->sceneSize.y / numberClustersY/* calculate based on screen size and numClustersY */;
          float clusterSizeZ = numberClustersZ;//* calculate based on screen size and numClustersZ */;

          // Iterate over clusters and initialize them
          for (int i = 0; i < numberClustersX; ++i) {
               for (int j = 0; j < numberClustersY; ++j) {
                    Cluster cluster;
                    clusters.push_back(cluster);
               }
          }
     }

     void ClusteredLighting::AssignLightsToClusters(const std::vector<LightStruct>& lights, std::vector<Cluster>& clusters) {
          // Clear existing light indices in clusters

          // Iterate over lights and assign them to clusters based on intersection
          for (int lightIndex = 0; lightIndex < lights.size(); ++lightIndex) {
               const LightStruct& light = lights[lightIndex];

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

     void ClusteredLighting::CreateClusterBuffers(const std::vector<Cluster>& clusters) {
          int size = 32;
          int clusterCount = glm::ceil(glm::ceil(Application::Get()->appSizes->sceneSize.x / size + 1) * glm::ceil(Application::Get()->appSizes->sceneSize.y / size + 1));
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
          glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Cluster) * clusterCount, mClusters.data(), GL_STATIC_DRAW);

          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, clusterBuffer);

          // Unbind the buffer
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

          // Create and bind buffer for light indices
          //GLuint lightsBuffer;
          glGenBuffers(1, &mLightsBuffer);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, mLightsBuffer);

          // Upload data to the buffer
          glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightStruct) * mLights.size(), mLights.data(), GL_STATIC_DRAW);

          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mLightsBuffer);

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



          ApplicationSizes& appSizes = *Application::Get()->appSizes;
          //unsigned int& textureColorbuffer = Application::Get()->textureColorbuffer;
          glGenFramebuffers(1, &ClusteredLighting::frameBuffer);
          glBindFramebuffer(GL_FRAMEBUFFER, ClusteredLighting::frameBuffer);
          // create a color attachment texture
          glGenTextures(1, &ClusteredLighting::textureColorbuffer);
          glBindTexture(GL_TEXTURE_2D, ClusteredLighting::textureColorbuffer);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ClusteredLighting::textureColorbuffer, 0);


          // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
          glGenRenderbuffers(1, &ClusteredLighting::rbo);
          glBindRenderbuffer(GL_RENDERBUFFER, ClusteredLighting::rbo);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
          glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ClusteredLighting::rbo); // now actually attach it

          // Disable reading
          glReadBuffer(GL_NONE);
          glDrawBuffer(GL_COLOR_ATTACHMENT0);

          //frameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
          //frameBuffer->Init();
          //frameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y, GL_RGBA32F, GL_FLOAT, NULL);
          //frameBuffer->InitRenderBufferObject(GL_RGBA32F, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);
          //GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
          //frameBuffer->DrawAttachments(attachments, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);

          /* Compute Shader */
          //GLuint lightsBuffer2;

          glGenBuffers(1, &mClustersBuffer);


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

          GLuint depthsBuffer;
          glGenBuffers(1, &depthsBuffer);
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, depthsBuffer);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, depthsBuffer);
          glBufferData(GL_SHADER_STORAGE_BUFFER, clusterCount * sizeof(glm::vec2), vector<glm::vec2>().data(), GL_DYNAMIC_DRAW);
     }

     void ClusteredLighting::LightingPass(const std::vector<Cluster>& clusters, const std::vector<LightStruct>& lights) {
          PLAZA_PROFILE_SECTION("Lighting Pass");
          GLuint lightsBindingLocation2 = glGetUniformLocation(mLightSorterComputeShader->ID, "LightsBuffer");
          GLuint clustersBindingLocation = glGetUniformLocation(mLightSorterComputeShader->ID, "ClusterBuffer");
          GLuint clustersBindingLocation3 = glGetUniformLocation(mLightSorterComputeShader->ID, "SizesBuffer");
          GLuint frustumsBindingLocation = glGetUniformLocation(mLightSorterComputeShader->ID, "FrustumsBuffer");
          if (ClusteredLighting::mLightsSize > 0) {
               // Set up shaders and framebuffer for the lighting pass
               glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
               glDisable(GL_DEPTH_TEST);
               mLightAccumulationShader->use();
               glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
               glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
               GLuint lightsBindingLocation = glGetUniformLocation(mLightAccumulationShader->ID, "LightsBuffer");
               glProgramUniform1i(mLightAccumulationShader->ID, lightsBindingLocation, 0);
               mLightAccumulationShader->setVec3("viewPos", Application::Get()->activeCamera->Position);
               mLightAccumulationShader->setMat4("viewMatrix", Application::Get()->activeCamera->GetViewMatrix());
               glEnable(GL_FRONT_AND_BACK);
               for (const Cluster& cluster : clusters) {


               }
               Application::Get()->mRenderer->RenderFullScreenQuad();
               glBindFramebuffer(GL_FRAMEBUFFER, 0);

               mLightSorterComputeShader->use();
               mLightSorterComputeShader->setMat4("view", Application::Get()->activeCamera->GetViewMatrix());
               mLightSorterComputeShader->setMat4("projection", Application::Get()->activeCamera->GetProjectionMatrix());
               mLightSorterComputeShader->setInt("lightCount", mLightsSize);
               mLightSorterComputeShader->setInt("depthMap", 30);
               glActiveTexture(GL_TEXTURE30);
               glBindTexture(GL_TEXTURE_2D, Application::Get()->gDepth);

               glProgramUniform1i(mLightSorterComputeShader->ID, lightsBindingLocation2, 0);

               glProgramUniform1i(mLightSorterComputeShader->ID, clustersBindingLocation, 1);

               glProgramUniform1i(mLightSorterComputeShader->ID, clustersBindingLocation3, 3);

               glProgramUniform1i(mLightSorterComputeShader->ID, frustumsBindingLocation, 7);
               //GLuint frustumsBindingLocation2 = glGetUniformLocation(mLightSorterComputeShader->ID, "DepthTileBuffer");
               //glProgramUniform1i(mLightSorterComputeShader->ID, frustumsBindingLocation2, 30);

               //glBindImageTexture(2, Application::Get()->gDepth, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

               glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mClustersBuffer);
               glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mLightsBuffer);
               //glProgramUniform1i(mLightSorterComputeShader->ID, glGetUniformLocation(mLightSorterComputeShader->ID, "LightsArray"), 1);
               //mLightSorterComputeShader->setBool("first", true);
               //glDispatchCompute(1, 1, 1);
               //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
               //mLightSorterComputeShader->setBool("first", false);
               glm::vec2 clusterSize = glm::vec2(32.0f);
               glm::vec2 clusterCount = glm::ceil(Application::Get()->appSizes->sceneSize / clusterSize);
               int extraX = int(Application::Get()->appSizes->sceneSize.x) % 32 != 0 ? 1 : 0;
               glDispatchCompute(clusterCount.x, clusterCount.y, 1);
               //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
          }

          /* Merge the scene frame buffer with the light frame buffer */
          glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->frameBuffer);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          mLightMergerShader->use();
          mLightMergerShader->setVec3("viewPos", Application::Get()->activeCamera->Position);
          mLightMergerShader->setFloat("time", glfwGetTime());
          mLightMergerShader->setMat4("view", Application::Get()->activeCamera->GetViewMatrix());
          mLightMergerShader->setMat4("projection", Application::Get()->activeCamera->GetProjectionMatrix());
          //GLuint lightsBindingLocation2 = glGetUniformLocation(mLightMergerShader->ID, "LightsBuffer");
          glProgramUniform1i(mLightMergerShader->ID, lightsBindingLocation2, 0);
          //GLuint clustersBindingLocation = glGetUniformLocation(mLightMergerShader->ID, "ClusterBuffer");
          glProgramUniform1i(mLightMergerShader->ID, clustersBindingLocation, 1);
          //GLuint clustersBindingLocation3 = glGetUniformLocation(mLightMergerShader->ID, "SizesBuffer");
          glProgramUniform1i(mLightMergerShader->ID, clustersBindingLocation3, 3);
          //GLuint frustumsBindingLocation = glGetUniformLocation(mLightMergerShader->ID, "FrustumsBuffer");
          glProgramUniform1i(mLightMergerShader->ID, frustumsBindingLocation, 7);

          mLightMergerShader->setInt("lightCount", mLightsSize);

          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mClustersBuffer);

          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, Application::Get()->gPosition);

          glActiveTexture(GL_TEXTURE1);
          glBindTexture(GL_TEXTURE_2D, Application::Get()->gNormal);

          glActiveTexture(GL_TEXTURE2);
          glBindTexture(GL_TEXTURE_2D, Application::Get()->gDiffuse);

          glActiveTexture(GL_TEXTURE3);
          glBindTexture(GL_TEXTURE_2D, Application::Get()->gOthers);

          Application::Get()->mRenderer->RenderFullScreenQuad();
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

     void ClusteredLighting::UpdateBuffers() {
          std::vector<LightStruct> mLights = std::vector<LightStruct>();
          for (auto [key, value] : Application::Get()->activeScene->lightComponents) {
               Transform& transform = Application::Get()->activeScene->transformComponents.find(key)->second;
               glm::vec3 pos = transform.GetWorldPosition();
               mLights.push_back(LightStruct(value.color, value.radius, transform.GetWorldPosition(), value.intensity));
          }
          mLightsSize = mLights.size();
          //glDeleteBuffers(1, &mLightsBuffer);
          //glGenBuffers(1, &mLightsBuffer);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, mLightsBuffer);

          // Allocate storage without initializing the data
          glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightStruct) * mLights.size(), nullptr, GL_STATIC_DRAW);

          // Upload the new data
          glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(LightStruct) * mLights.size(), mLights.data());

          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mLightsBuffer);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

          //// get pointer
          //void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
          // now copy data into memory
          //memcpy(ptr, lightsVector.data(), lightsVector.size() * sizeof(LightStruct));
          // make sure to tell OpenGL we're done with the pointer
          //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
          //mLights = lightsVector;
     }
}