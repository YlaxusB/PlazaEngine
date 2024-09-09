#pragma once

#include "ThirdParty/GLFW/include/GLFW/glfw3.h"
//#include "ThirdParty/imgui/imgui.h"
//#include "ThirdParty/imgui/imgui_impl_glfw.h"
//#include "ThirdParty/imgui/imgui_impl_opengl3.h"

//#include "Engine/Application/EditorCamera.h"
#include "Engine/Components/Core/Entity.h"
//#include "Engine/Application/Application.h"

#include "Editor/GUI/GuiWindow.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
#include "Editor/GUI/AssetsImporterWindow/AssetsImporterWindow.h"
#include "Editor/GUI/Console/Console.h"
#include "Editor/EditorTools/EditorTool.h"

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
			static void setupDockspace(GLFWwindow* window, Camera* camera);
			static void changeSelectedGameObject(Entity* newSelectedGameObject);
			static void Init(GLFWwindow* window);
			static void Delete();
			static void Update();
			static void NewFrame();

			static void beginScene(Camera& camera);
			static void beginEditor(Camera& camera);
			static void beginHierarchyView();
			static void beginInspector(Camera camera);
			static void beginImageInspector(Camera camera);
			static void beginAssetsViewer(Camera camera);

			static inline bool mImageInspectorShowAllImages = false;
			static inline bool mShowSelectedImageInEditorView = false;
			static inline bool mFlipY = true;
			static inline ImTextureID mSelectedImageInspector = 0;
			static inline ImVec2 imageSize = ImVec2(200, 200);

			static string scenePayloadName;

			static bool sceneViewUsingEditorCamera;
			static bool isHierarchyOpen;
			static bool isSceneOpen;
			static bool isInspectorOpen;
			static bool isFileExplorerOpen;
			static bool canUpdateContent;

			static ImTextureID playPauseButtonImageId;

			static void UpdateSizes();

			static void OpenAssetImporterContext(std::string fileToImport);

			static inline GuiLayer sFocusedLayer = GuiLayer::SCENE;
			static inline std::vector<GuiWindow> mGuiWindows = std::vector<GuiWindow>();

			HierarchyWindow mHierarchy = HierarchyWindow(GuiLayer::HIERARCHY, true);
			AssetsImporterWindow mAssetsImporter = AssetsImporterWindow(GuiLayer::ASSETS_IMPORTER, false);
			Console* mConsole = new Console(GuiLayer::CONSOLE, false);

			static inline std::map<EditorTool::ToolType, std::unique_ptr<EditorTool>> sEditorTools;
			//static inline EditorTool::ToolType sEditorToolCaptureMouseClick;
			//static inline EditorTool::ToolType sEditorToolCaptureKeyPress;
		private:
			static inline bool isAssetImporterOpen = false;
			static inline std::string mAssetToImportPath = "";
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