#pragma once

#include "Engine/Vendor/GLFW/glfw3.h"
//#include "Engine/Vendor/imgui/imgui.h"
//#include "Engine/Vendor/imgui/imgui_impl_glfw.h"
//#include "Engine/Vendor/imgui/imgui_impl_opengl3.h"

//#include "Engine/Application/EditorCamera.h"
#include "Engine/Components/Core/Entity.h"
//#include "Engine/Application/Application.h"

namespace ImGuizmoHelper {
	static bool IsDrawing;
}

namespace Plaza {
	class Camera;
}
using namespace std;
namespace Plaza {
	namespace Editor {
		class Gui {
		public:
			class FileExplorer;
			class MainMenuBar;
			class Hierarchy;
			//class MaterialInspector;
			class TransformInspector;
			static void setupDockspace(GLFWwindow* window, int gameFrameBuffer, Camera* camera);
			static void changeSelectedGameObject(Entity* newSelectedGameObject);
			static void Init(GLFWwindow* window);
			static void Delete();
			static void Update();
			static void NewFrame();

			static void beginScene(int gameFrameBuffer, Camera& camera);
			static void beginEditor(int gameFrameBuffer, Camera& camera);
			static void beginHierarchyView(int gameFrameBuffer);
			static void beginInspector(int gameFrameBuffer, Camera camera);

			static string scenePayloadName;

			static bool isHierarchyOpen;
			static bool isSceneOpen;
			static bool isInspectorOpen;
			static bool isFileExplorerOpen;
			static bool canUpdateContent;

			static unsigned int playPauseButtonImageId;

			static void UpdateSizes();
		};
	}
}


/// <summary>
/// ImGui Helper
/// </summary>
namespace ImGui {
	inline bool Compare(ImVec2 firstVec, ImVec2 secondVec) {
		return firstVec.x == secondVec.x && firstVec.y == secondVec.y;
	}
	// Transforms glm::vec2 to ImVec2
	inline ImVec2 imVec2(glm::vec2 vec) {
		return ImVec2(vec.x, vec.y);
	}
	// Transforms ImVec2 to glm::vec2
	inline glm::vec2 glmVec2(ImVec2 imguiVec) {
		return glm::vec2(imguiVec.x, imguiVec.y);
	}

}