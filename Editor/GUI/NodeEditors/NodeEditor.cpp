#include "Engine/Core/PreCompiledHeaders.h"
#include "NodeEditor.h"
#include "Engine/Application/FileDialog/FileDialog.h"

//namespace ed = ax::NodeEditor;
namespace Plaza::Editor {
	void NodeEditor::Init() {
		ax::NodeEditor::Config config;
		config.SettingsFile = "Simple.json";
		mContext = ax::NodeEditor::CreateEditor(&config);

		Node node = Node();
		node.name = "Multiply";
		node.inputs.push_back(Pin(0, "X", PinType::Float));
		node.outputs.push_back(Pin(0, "Y", PinType::Float, PinKind::Output));
		AddNodeToCreate(node);
		node.name = "Enum";
		node.inputs[0].type = PinType::Enum;
		node.outputs[0].type = PinType::Enum;
		AddNodeToCreate(node);
	};

	enum class VkFormat {
		VK_FORMAT_UNDEFINED,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		// Add other formats as needed
	};

	// Convert VkFormat to string for display in combo box
	const char* VkFormatToString(VkFormat format) {
		switch (format) {
		case VkFormat::VK_FORMAT_UNDEFINED: return "VK_FORMAT_UNDEFINED";
		case VkFormat::VK_FORMAT_D32_SFLOAT: return "VK_FORMAT_D32_SFLOAT";
		case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT: return "VK_FORMAT_D24_UNORM_S8_UINT";
		default: return "Unknown";
		}
	}

	void NodeEditor::Update() {
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
		static VkFormat selectedFormat = VkFormat::VK_FORMAT_UNDEFINED;  // Default value

		// Create a combo box for VkFormat enum
		const char* formats[] = {
			VkFormatToString(VkFormat::VK_FORMAT_UNDEFINED),
			VkFormatToString(VkFormat::VK_FORMAT_D32_SFLOAT),
			VkFormatToString(VkFormat::VK_FORMAT_D24_UNORM_S8_UINT),
			// Add more formats as needed
		};
		const char* currentFormat = VkFormatToString(selectedFormat);
		if (ImGui::Begin("Render Graph Editor", &mExpanded, windowFlags)) {
			ImGui::BeginChild("Selection", ImVec2(300.0f, 0));
			for (const auto& [key, value] : mTemplateNodes) {
				if (ImGui::Button(("Add: " + value.name).c_str())) {
					this->NewNode(value.name);
				}
			}
			for (const auto& node : mNodes) {
				if (ImGui::Button(node.name.c_str()))
					ax::NodeEditor::SelectNode(node.id);
				if (ImGui::Button("Navigate to Content"))
					ax::NodeEditor::NavigateToContent();
				if (ImGui::Button("Navigate to Selected"))
					ax::NodeEditor::NavigateToSelection(true);
			}

			ImGui::EndChild();
			ImGui::SameLine();
			ax::NodeEditor::SetCurrentEditor(mContext);
			ax::NodeEditor::Begin("Node Editor", ImVec2(0.0, 0.0f));
			int uniqueId = 1;
			// Start drawing nodes.
			ax::NodeEditor::BeginNode(uniqueId++);
			ImGui::Text("Node A");
			ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
			ImGui::Text("-> In");
			ax::NodeEditor::EndPin();
			ImGui::SameLine();
			ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
			ImGui::Text("Out ->");
			ax::NodeEditor::EndPin();
			ax::NodeEditor::EndNode();

			for (const auto& node : mNodes) {
				ax::NodeEditor::BeginNode(uniqueId++);
				ImGui::Text(node.name.c_str());
				for (const auto& input : node.inputs) {
					cursorPos = ImGui::GetWindowPos();
					if (input.type == PinType::Enum) {
						//if (ImGui::Button("Select"))
						//	ImGui::OpenPopup("EnumPopup");
						ImVec2 pos;
						if (NodeEditor::BeginNodeCombo("Format", currentFormat, ImGuiComboFlags_PopupAlignLeft, pos)) {
							ImGui::SetNextWindowPos(pos);
							ax::NodeEditor::Suspend();
							if (ImGui::Begin("New Window")) {
								for (int i = 0; i < IM_ARRAYSIZE(formats); ++i) {
									bool isSelected = (currentFormat == formats[i]);
									if (ImGui::Selectable(formats[i], isSelected)) {
										selectedFormat = static_cast<VkFormat>(i);  // Set selected enum value
									}
								}
								ImGui::End();
								ax::NodeEditor::Resume();
							}
							NodeEditor::EndNodeCombo();
						}
					}
					else {
						ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
						ImGui::Text(input.name.c_str());
						ax::NodeEditor::EndPin();
					}
				}
				for (const auto& output : node.outputs) {
					ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
					ImGui::Text(output.name.c_str());
					ax::NodeEditor::EndPin();
				}
				ax::NodeEditor::EndNode();
			}

			if (ax::NodeEditor::BeginCreate())
			{
				ax::NodeEditor::PinId inputPinId, outputPinId;
				if (ax::NodeEditor::QueryNewLink(&inputPinId, &outputPinId))
				{
					// QueryNewLink returns true if editor want to create new link between pins.
					//
					// Link can be created only for two valid pins, it is up to you to
					// validate if connection make sense. Editor is happy to make any.
					//
					// Link always goes from input to output. User may choose to drag
					// link from output pin or input pin. This determine which pin ids
					// are valid and which are not:
					//   * input valid, output invalid - user started to drag new ling from input pin
					//   * input invalid, output valid - user started to drag new ling from output pin
					//   * input valid, output valid   - user dragged link over other pin, can be validated

					if (inputPinId && outputPinId) // both are valid, let's accept link
					{
						// ed::AcceptNewItem() return true when user release mouse button.
						if (ax::NodeEditor::AcceptNewItem())
						{
							// Since we accepted new link, lets add one to our list of links.
							mLinks.push_back({ ax::NodeEditor::LinkId(mNextLinkId++), inputPinId, outputPinId });

							// Draw new link.
							ax::NodeEditor::Link(mLinks.back().id, mLinks.back().startPinID, mLinks.back().endPinID);
						}

						// You may choose to reject connection between these nodes
						// by calling ed::RejectNewItem(). This will allow editor to give
						// visual feedback by changing link thickness and color.
					}
				}
			}
			ax::NodeEditor::EndCreate();

			for (const auto& link : mLinks) {
				ax::NodeEditor::Link(link.id, link.startPinID, link.endPinID);
			}

			ax::NodeEditor::End();
			ax::NodeEditor::SetCurrentEditor(nullptr);
			//if (ImGui::BeginPopup("EnumPopup"))
			//{
			//	for (int i = 0; i < IM_ARRAYSIZE(formats); ++i) {
			//		bool isSelected = (currentFormat == formats[i]);
			//		if (ImGui::Selectable(formats[i], isSelected)) {
			//			selectedFormat = static_cast<VkFormat>(i);  // Set selected enum value
			//		}
			//	}
			//
			//	ImGui::EndPopup();
			//}
		}


		ImGui::End();
	};
	void NodeEditor::OnKeyPress(int key, int scancode, int action, int mods) {

	};

	void NodeEditor::NewNode(const std::string& nodeName) {
		Node& newNode = mTemplateNodes.at(nodeName);
		newNode.id = GetNextId();
		for (const auto& input : newNode.inputs) {
			//input.id; //= ed::PinId(GetNextId());
		}
		mNodes.emplace_back(newNode);
	}

	void NodeEditor::AddNodeToCreate(const Node& newNode) {
		mTemplateNodes.emplace(newNode.name, newNode);
		//mNodes.emplace_back(GetNextId(), "Branch");
		//mNodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
		//mNodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
		//mNodes.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
		//mNodes.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);
	}

	bool NodeEditor::BeginNodeCombo(const char* label, const char* preview_value, ImGuiComboFlags flags, ImVec2& pos)
	{
		using namespace ImGui;

		ImGuiContext& g = *GImGui;
		bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
		g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview));

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const float expected_w = CalcItemWidth();
		const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
		bool popup_open = IsPopupOpen(id, ImGuiPopupFlags_None);

		const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
		RenderNavHighlight(frame_bb, id);
		if (!(flags & ImGuiComboFlags_NoPreview))
			window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(value_x2, frame_bb.Max.y), frame_col, style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Left);
		if (!(flags & ImGuiComboFlags_NoArrowButton))
		{
			ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
			ImU32 text_col = GetColorU32(ImGuiCol_Text);
			window->DrawList->AddRectFilled(ImVec2(value_x2, frame_bb.Min.y), frame_bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Right);
			if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
				RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, frame_bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
		}
		RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);
		if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
			RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
		if (label_size.x > 0)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		if ((pressed || g.NavActivateId == id) && !popup_open)
		{
			if (window->DC.NavLayerCurrent == 0)
				window->NavLastIds[0] = id;
			OpenPopupEx(id, ImGuiPopupFlags_None);
			popup_open = true;
		}

		if (!popup_open)
			return false;

		if (has_window_size_constraint)
		{
			g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
			g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
		}
		else
		{
			if ((flags & ImGuiComboFlags_HeightMask_) == 0)
				flags |= ImGuiComboFlags_HeightRegular;
			IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
			int popup_max_height_in_items = -1;
			if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
			else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
			else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
			SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
		}

		char name[16];
		ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

		// Set default position without using CalcWindowExpectedSize
		ImVec2 size_expected(w, CalcMaxPopupHeightFromItemCount(8)); // Estimate expected size
		pos = frame_bb.GetBL();
		if (flags & ImGuiComboFlags_PopupAlignLeft)
			pos.x -= size_expected.x - w;

		SetNextWindowPos(ax::NodeEditor::CanvasToScreen(pos));
		pos = ax::NodeEditor::CanvasToScreen(pos);

		// Move ed::Suspend and ed::Resume here
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

		//ax::NodeEditor::Suspend();
		PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));

		bool ret = Begin(name, NULL, window_flags);

		PopStyleVar();

		if (!ret)
		{
			EndPopup();
			IM_ASSERT(0);
			return false;
		}
		return true;
	}

	void NodeEditor::EndNodeCombo() {
		ImGui::EndPopup();

		//ax::NodeEditor::Resume();
		//ImGui::SetWindowPos(ImVec2(500.0f, 200.0f));
	}
}