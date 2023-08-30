#pragma once
#include <string>
#include <glm/glm.hpp>
namespace Plaza::Editor::Utils {

	static float GetMinimumSpeed(float value, float divider, float min) {
		float dividedValue = glm::abs(value) / (divider * 10);
		float minimumSpeed = dividedValue > 0 ? dividedValue : min;
		return minimumSpeed > 0 ? minimumSpeed : min * divider;
	}

	static void Vec3Slider(std::string name, glm::vec3& vec3) {
		ImGui::SetNextItemWidth(225);
		ImGui::SliderFloat3(name.c_str(), &vec3.x, 0, 1.0f);
	}

	static void DragFloat(std::string name, float& value, float speed, std::function<void(float)> callback, float min = std::numeric_limits<float>().max(), float max = std::numeric_limits<float>().max(), const char* format = "%.3f", bool maxZero = false) {

		ImGui::Text(name.c_str());
		ImGui::SameLine();
		if (ImGui::DragFloat(("##" + name).c_str(), &value, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			callback(value);
	}

	static void DragFloat3(std::string name, glm::vec3& value, float speed, std::function<void(glm::vec3) > callback, float min = std::numeric_limits<float>().max(), float max = std::numeric_limits<float>().max(), const char* format = "%.3f", bool maxZero = false) {
		ImGui::PushItemWidth(75);
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::Text("X:");
		ImGui::SameLine();
		if (ImGui::DragFloat(("##x" + name).c_str(), &value.x, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			callback(value);
		//ImGui::SetWindowSize(ImVec2(10, 50));
		ImGui::SameLine();
		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragFloat(("##y" + name).c_str(), &value.y, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			callback(value);
		ImGui::SameLine();
		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragFloat(("##z" + name).c_str(), &value.z, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			callback(value);
	}

	static bool DragFloat3(std::string name, glm::vec3& value, float speed, float min = std::numeric_limits<float>().max(), float max = std::numeric_limits<float>().max(), const char* format = "%.3f", bool maxZero = false) {
		ImGui::PushItemWidth(75);
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::Text("X:");
		ImGui::SameLine();
		bool isDragging = false;
		if (ImGui::DragFloat(("##x" + name).c_str(), &value.x, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			isDragging = true;
		//ImGui::SetWindowSize(ImVec2(10, 50));
		ImGui::SameLine();
		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragFloat(("##y" + name).c_str(), &value.y, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			isDragging = true;
		ImGui::SameLine();
		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragFloat(("##z" + name).c_str(), &value.z, speed, min, max, format, ImGuiInputTextFlags_CallbackEdit))
			isDragging = true;

		return isDragging;
	}
}