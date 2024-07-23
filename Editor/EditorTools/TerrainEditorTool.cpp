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
					uvs.push_back(glm::vec2(0.5f, 0.5f));
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

			Mesh* mesh = &Application->mRenderer->CreateNewMesh(vertices, normals, uvs, {}, {}, indices, { 0 }, false, {}, {});
			return mesh;
		}
		void TerrainEditorTool::CreateTerrain(unsigned int x, unsigned int y, unsigned int z) {
			Scene* scene = Scene::GetActiveScene();
			Entity* entity = new Entity("Terrain", scene->mainSceneEntity);

			Mesh* mesh = TerrainEditorTool::CreateHeightMapTerrain(x, y, z);
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
				Utils::AddTableInt("X", &sSettings.x);
				Utils::AddTableInt("Y", &sSettings.y);
				Utils::AddTableInt("Z", &sSettings.z);
				ImGui::EndTable();
				if (ImGui::Button("Create Terrain")) {
					TerrainEditorTool::CreateTerrain(sSettings.x, sSettings.y, sSettings.z);
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

			ImGui::End();
		}

		void TerrainEditorTool::OnMouseClick() {

		}

		void TerrainEditorTool::OnKeyPress() {

		}
	}
}