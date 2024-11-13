#include "Engine/Core/PreCompiledHeaders.h"
#include "RenderGraphEditor.h"
#include "Engine/Application/FileDialog/FileDialog.h"

namespace Plaza::Editor {
	//void RenderGraphEditor::Init() {
	//	NodeEditor::Config config;
	//	config.SettingsFile = "Simple.json";
	//	mContext = NodeEditor::CreateEditor(&config);
	//};
	//void RenderGraphEditor::Update() {
	//	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
	//	if (ImGui::Begin("Render Graph Editor", &mExpanded, windowFlags)) {
	//		if (ImGui::Button("Serialize Graph"))
	//			AssetsSerializer::SerializeFile<VulkanRenderGraph>(*VulkanRenderer::GetRenderer()->mRenderGraph, Plaza::FileDialog::SaveFileDialog(Standards::plazaRenderGraph.c_str()), Application::Get()->mSettings.mRenderGraphSerializationMode);
	//
	//		if (ImGui::Button("DeSerialize Graph")) {
	//			std::shared_ptr<VulkanRenderGraph> graph = AssetsSerializer::DeSerializeFile<VulkanRenderGraph>(Plaza::FileDialog::OpenFileDialog(Standards::plazaRenderGraph.c_str()), Application::Get()->mSettings.mRenderGraphSerializationMode);
	//			std::cout << graph->mCompiledBindings.size() << "\n";
	//			graph->OrderPasses();
	//			graph->Compile();
	//		}
	//
	//
	//		NodeEditor::SetCurrentEditor(mContext);
	//		NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));
	//		int uniqueId = 1;
	//		NodeEditor::BeginCreate();
	//		NodeEditor::EndCreate();
	//		// Start drawing nodes.
	//		NodeEditor::BeginNode(uniqueId++);
	//		ImGui::Text("Node A");
	//		NodeEditor::BeginPin(uniqueId++, NodeEditor::PinKind::Input);
	//		ImGui::Text("-> In");
	//		NodeEditor::EndPin();
	//		ImGui::SameLine();
	//		NodeEditor::BeginPin(uniqueId++, NodeEditor::PinKind::Output);
	//		ImGui::Text("Out ->");
	//		NodeEditor::EndPin();
	//		NodeEditor::EndNode();
	//		NodeEditor::End();
	//		NodeEditor::SetCurrentEditor(nullptr);
	//	}
	//
	//	ImGui::End();
	//};
	//void RenderGraphEditor::OnKeyPress(int key, int scancode, int action, int mods) {
	//
	//};
}