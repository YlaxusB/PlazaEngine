#include "Engine/Core/PreCompiledHeaders.h"
#include "TerrainEditorTool.h"
#include <unordered_map>
#include "Editor/GUI/Utils/DataVisualizer.h"

namespace Plaza {
	namespace Editor {
		Mesh* TerrainEditorTool::CreateHeightMapTerrain(unsigned int x, unsigned int y, unsigned int z) {
			std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
			std::vector<unsigned int> indices = std::vector<unsigned int>();//x * z * 3
			std::vector<glm::vec3> normals = std::vector<glm::vec3>();
			std::vector<glm::vec2> uvs = std::vector<glm::vec2>();

			for (unsigned int i = 0; i < x; ++i) {
				for (unsigned int j = 0; j < z; ++j) {
					vertices.push_back(glm::vec3(i, 1.0f, j));
					normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
					uvs.push_back(glm::vec2(float(i % mSettings.repeatInterval) / float(mSettings.repeatInterval), float(j % mSettings.repeatInterval) / float(mSettings.repeatInterval)));
				}
			}

			int depth = x;
			int width = z;

			for (int z = 0; z < depth - 1; z++) {
				for (int x = 0; x < width - 1; x++) {
					int bottomLeft = z * width + x;
					int topleft = (z + 1) * width + x;
					int topRight = (z + 1) * width + x + 1;
					int bottomRight = z * width + x + 1;

					indices.push_back(bottomLeft);
					indices.push_back(topRight);
					indices.push_back(topleft);

					indices.push_back(bottomLeft);
					indices.push_back(bottomRight);
					indices.push_back(topRight);
				}
			}

			// Recalculate normals
			for (int i = 0; i < indices.size(); i += 3) {
				normals[indices[i]] = glm::vec3(0.0f);
				normals[indices[i + 1]] = glm::vec3(0.0f);
				normals[indices[i + 2]] = glm::vec3(0.0f);
			}

			for (int i = 0; i < indices.size(); i += 3) {
				int index0 = indices[i];
				int index1 = indices[i + 1];
				int index2 = indices[i + 2];
				glm::vec3 v1 = vertices[index1] - vertices[index0];
				glm::vec3 v2 = vertices[index2] - vertices[index0];
				glm::vec3 normal = glm::cross(v1, v2);
				normal = glm::normalize(normal);
				normals[index0] += normal;
				normals[index1] += normal;
				normals[index2] += normal;
			}

			std::vector<glm::vec3> tangents;
			std::vector<glm::vec3> bitangents;
			std::vector<unsigned int> materials{ 0 };
			Mesh* mesh = &Application->mRenderer->CreateNewMesh(vertices, normals, uvs, tangents, bitangents, indices, materials, false, {}, {});
			return mesh;
		}
		void TerrainEditorTool::CreateTerrain(unsigned int x, unsigned int y, unsigned int z) {
			Scene* scene = Scene::GetActiveScene();
			Entity* entity = new Entity("Terrain", scene->mainSceneEntity);
			mLastTerrainUuid = entity->uuid;

			Mesh* mesh = this->CreateHeightMapTerrain(x, y, z);
			AssetsManager::AddMesh(mesh);

			MeshRenderer* meshRenderer = new MeshRenderer(mesh, { scene->DefaultMaterial() }, false);
			entity->AddComponent<MeshRenderer>(meshRenderer);

		}

		void TerrainEditorTool::UpdateGui() {
			PLAZA_PROFILE_SECTION("Begin Terrain Editor Tool");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;

			ImGui::SetNextWindowSize(ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y));

			ImGuiStyle& style = ImGui::GetStyle();

			// Adjust padding and margin sizes
			style.WindowPadding = ImVec2(0.0f, 0.0f);  // Change window padding
			if (ImGui::Begin("Terrain Tool", &Gui::isSceneOpen, windowFlags)) {
				ImGui::BeginTable("Terrain Editor Tool Settings", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
				Utils::AddTableInt("X", &mSettings.x);
				Utils::AddTableInt("Y", &mSettings.y);
				Utils::AddTableInt("Z", &mSettings.z);
				Utils::AddTableInt("Radius", &mSettings.radius);
				Utils::AddTableFloat("Intensity", &mSettings.intensity);
				Utils::AddTableInt("Repeat Interval", &mSettings.repeatInterval);
				ImGui::EndTable();
				if (ImGui::Button("Create Terrain")) {
					TerrainEditorTool::CreateTerrain(mSettings.x, mSettings.y, mSettings.z);
				}
			};
			if (ImGui::IsWindowFocused())
			{
				if (Application->focusedMenu != "TerrainEditorTool") {
					glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				Application->focusedMenu = "TerrainEditorTool";
			}
			if (ImGui::IsWindowHovered())
				Application->hoveredMenu = "TerrainEditorTool";

			if (ImGui::Checkbox("Edit Terrain", &mEditTerrain)) {
				this->CaptureMouseClick(mEditTerrain);
			}

			ImGui::Checkbox("Raise Tool", &mSettings.raiseTool);

			ImGui::Checkbox("Smooth Tool", &mSettings.smoothTool);


			if (mEditTerrain && glfwGetMouseButton(Application->Window->glfwWindow, 0) == GLFW_PRESS)
				OnMouseClick(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);

			ImGui::End();
		}

		void TerrainEditorTool::RaiseTool(Mesh* mesh, uint32_t nearestVertexIndex) {
			glm::vec2 centerPos = glm::vec2(mesh->vertices[nearestVertexIndex].x, mesh->vertices[nearestVertexIndex].z);

			for (int i = -mSettings.radius; i < mSettings.radius; ++i) {
				for (int j = -mSettings.radius; j < mSettings.radius; ++j) {
					uint32_t vertexIndex = nearestVertexIndex + (j * mSettings.x) + i;
					if (mesh->vertices.size() > vertexIndex) {
						if (i == 0 && j == 0) {
							float asd = 50;
						}
						glm::vec2 vertexPos = glm::vec2(mesh->vertices[vertexIndex].x, mesh->vertices[vertexIndex].z);
						float distanceFromCenter = glm::length(vertexPos - centerPos);
						if (distanceFromCenter == 0.0f) {
							mesh->vertices[vertexIndex].y += mSettings.intensity * 2.0f;
							continue;
						}
						if (distanceFromCenter <= mSettings.radius) {
							float distanceMultiplier = 1.0f - distanceFromCenter / float(mSettings.radius);

							float newY = glm::lerp(1.0f, 2.0f, distanceMultiplier);
							mesh->vertices[vertexIndex].y += mSettings.intensity * newY;
						}
					}
				}
			}
			if (mesh->vertices.size() > nearestVertexIndex) {
				for (int i = 0; i < mesh->indices.size(); i += 3) {
					mesh->normals[mesh->indices[i]] = glm::vec3(0.0f);
					mesh->normals[mesh->indices[i + 1]] = glm::vec3(0.0f);
					mesh->normals[mesh->indices[i + 2]] = glm::vec3(0.0f);
				}

				for (int i = 0; i < mesh->indices.size(); i += 3) {
					int index0 = mesh->indices[i];
					int index1 = mesh->indices[i + 1];
					int index2 = mesh->indices[i + 2];
					glm::vec3 v1 = mesh->vertices[index1] - mesh->vertices[index0];
					glm::vec3 v2 = mesh->vertices[index2] - mesh->vertices[index0];
					glm::vec3 normal = glm::cross(v1, v2);
					normal = glm::normalize(normal);
					mesh->normals[index0] += normal;
					mesh->normals[index1] += normal;
					mesh->normals[index2] += normal;
				}

				//mesh->vertices[nearestVertexIndex].y += 10.0f;
				VulkanRenderer::GetRenderer()->UpdateMeshVertices(*mesh);
			}
		}

		void TerrainEditorTool::SmoothTool(Mesh* mesh, uint32_t nearestVertexIndex) {
			// Apply smoothing
			glm::vec2 centerPos = glm::vec2(mesh->vertices[nearestVertexIndex].x, mesh->vertices[nearestVertexIndex].z);
			for (int i = -mSettings.radius; i < mSettings.radius; ++i) {
				for (int j = -mSettings.radius; j < mSettings.radius; ++j) {
					uint32_t vertexIndex = nearestVertexIndex + (j * mSettings.x) + i;
					if (mesh->vertices.size() <= vertexIndex)
						continue;

					glm::vec2 vertexPos = glm::vec2(mesh->vertices[vertexIndex].x, mesh->vertices[vertexIndex].z);
					float distanceFromCenter = glm::length(vertexPos - centerPos);

					if (distanceFromCenter <= mSettings.radius) {
						float sumHeight = 0.0f;
						int count = 0;

						// Calculate average height of surrounding vertices
						for (int ni = -1; ni <= 1; ++ni) {
							for (int nj = -1; nj <= 1; ++nj) {
								uint32_t neighborIndex = vertexIndex + (nj * mSettings.x) + ni;
								if (mesh->vertices.size() > neighborIndex) {
									sumHeight += mesh->vertices[neighborIndex].y;
									count++;
								}
							}
						}

						if (count > 0) {
							float averageHeight = sumHeight / count;
							float smoothingFactor = mSettings.intensity; // Adjust smoothing effect
							mesh->vertices[vertexIndex].y = glm::mix(mesh->vertices[vertexIndex].y, averageHeight, smoothingFactor);
						}
					}
				}
			}

			// Recalculate normals
			for (int i = 0; i < mesh->indices.size(); i += 3) {
				mesh->normals[mesh->indices[i]] = glm::vec3(0.0f);
				mesh->normals[mesh->indices[i + 1]] = glm::vec3(0.0f);
				mesh->normals[mesh->indices[i + 2]] = glm::vec3(0.0f);
			}

			for (int i = 0; i < mesh->indices.size(); i += 3) {
				int index0 = mesh->indices[i];
				int index1 = mesh->indices[i + 1];
				int index2 = mesh->indices[i + 2];
				glm::vec3 v1 = mesh->vertices[index1] - mesh->vertices[index0];
				glm::vec3 v2 = mesh->vertices[index2] - mesh->vertices[index0];
				glm::vec3 normal = glm::cross(v1, v2);
				normal = glm::normalize(normal);
				mesh->normals[index0] += normal;
				mesh->normals[index1] += normal;
				mesh->normals[index2] += normal;
			}

			VulkanRenderer::GetRenderer()->UpdateMeshVertices(*mesh);
		}

		void TerrainEditorTool::OnMouseClick(int button, int action, int mods) {
			if (Application->focusedMenu != "Editor") //!Gui::sFocusedLayer == GuiLayer::SCENE || 
				return;

			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mEditTerrain) {
				Application->mThreadsManager->mFrameRendererAfterGeometry->AddToQueue([&]() {
					float xposGame = Callbacks::lastX - Application->appSizes->hierarchySize.x;
					float yposGame = Callbacks::lastY - Application->appSizes->sceneImageStart.y;
					yposGame = Application->appSizes->sceneSize.y - (Callbacks::lastY - Application->appSizes->sceneImageStart.y - 35);
					glm::vec4 clickPosition = VulkanRenderer::GetRenderer()->mDeferredPositionTexture.ReadTexture(glm::vec2(xposGame, yposGame), 4);
					clickPosition.w = 1.0f;

					Entity* entity = Scene::GetActiveScene()->GetEntity(mLastTerrainUuid);
					if (!entity)
						return;

					glm::vec4 localPosition = glm::inverse(entity->GetComponent<Transform>()->GetTransform()) * clickPosition;

					Mesh* mesh = entity->GetComponent<MeshRenderer>()->mesh;

					uint32_t nearestVertexIndex = (glm::round<int>(localPosition.x) * mSettings.x) + (glm::round<int>(localPosition.z));//(mSettings.x / localPosition.x) * (mSettings.z / localPosition.z);

					if (mSettings.raiseTool)
						RaiseTool(mesh, nearestVertexIndex);
					if (mSettings.smoothTool)
						SmoothTool(mesh, nearestVertexIndex);

					//std::cout << "Clicked at: " << "X: " << localPosition.x << " Y: " << localPosition.y << " Z: " << localPosition.z << "\n";
					//std::cout << "Nearest Index: " << nearestVertexIndex << "\n";
					});
			}

		}

		void TerrainEditorTool::OnKeyPress(int key, int scancode, int action, int mods) {

		}
	}
}