#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <stb/stb_image.h>

#include <iostream>
#include <random>
#include <filesystem>
#include <fileSystem/fileSystem.h>
#include "guiMain.h"


void setupDockspace(GLFWwindow* window, int gameFrameBuffer, AppSizes* appSizes) {
	ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(appSizes->appWidth, appSizes->appHeight));
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
	if (ImGui::Begin("demo", new bool(true), windowFlags)) {

		// Create UI elements
		ImGui::SetNextWindowSize(ImVec2(appSizes->sceneWidth, appSizes->sceneHeight));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		if (ImGui::Begin("Scene", new bool(true), sceneWindowFlags)) {

			int textureId = gameFrameBuffer;
			ImGui::Image(ImTextureID(textureId), ImVec2(appSizes->sceneWidth, appSizes->sceneHeight));
		}

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}
	ImGui::End();
}
