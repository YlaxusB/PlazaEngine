#pragma once
#include <imgui/imgui.h>
class EditorStyle {
public:
#pragma region Hierarchy
	ImVec4 hierarchyBackgroundColor = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);

#pragma region TreeNode
	ImVec4 treeNodeBackgroundColor = ImVec4(0.23f, 0.23f, 0.23f, 1.0f);
	ImVec4 selectedTreeNodeBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
#pragma endregion TreeNode


#pragma endregion Hierarchy




};

extern EditorStyle editorStyle;