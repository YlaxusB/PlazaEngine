#pragma once
#include "Editor/GUI/GuiWindow.h"
#include "ThirdParty/imgui/nodes/imgui_node_editor.h"
#include "ThirdParty/imgui/imgui_internal.h"

//namespace ed = ax::NodeEditor;
namespace Plaza::Editor {
	class NodeEditor : public GuiWindow {
	public:
		std::string mName = "";
		ax::NodeEditor::EditorContext* mContext = nullptr;

		NodeEditor(const std::string& name, GuiLayer layer, bool startOpen = true) : GuiWindow(layer, startOpen) { }

		void Init() override;
		void Update() override;
		void OnKeyPress(int key, int scancode, int action, int mods) override;

	public:
		struct Node;
		class Pin;
		class Link;
		//struct NodeIdLess;
		int	mNextId = 1;
		int mNextLinkId = 1;
		const int mPinIconSize = 24;
		std::vector<Node>    mNodes;
		std::vector<Link>    mLinks;
		std::map<std::string, Node>    mTemplateNodes;
		const float          mTouchTime = 1.0f;
		//std::map<ax::NodeEditor::NodeId, float, NodeIdLess> mNodeTouchTime;

		int GetNextId() { return mNextId++; }

		void NewNode(const std::string& nodeName);
		void RemoveNode(int index);

		void AddNodeToCreate(const Node& newNode);
	public:
		//struct NodeIdLess
		//{
		//	bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
		//	{
		//		return lhs.AsPointer() < rhs.AsPointer();
		//	}
		//};
		enum class PinType
		{
			Flow,
			Bool,
			Int,
			Float,
			String,
			Object,
			Function,
			Delegate,
			Enum
		};

		enum class PinKind
		{
			Output,
			Input
		};

		enum class NodeType
		{
			Blueprint,
			Simple,
			Tree,
			Comment,
			Houdini
		};
		struct Pin
		{
			ax::NodeEditor::PinId   id;
			Node* node;
			std::string name;
			PinType     type;
			PinKind     kind;

			Pin(int newId, const char* newName, PinType newType, PinKind newKind = PinKind::Input) :
				id(newId), node(nullptr), name(newName), type(newType), kind(newKind)
			{
			}
		};
		struct Node {
			int id;
			std::string name;
			std::vector<Pin> inputs;
			std::vector<Pin> outputs;
			ImColor color;
			NodeType type;
			ImVec2 size;
		};
		struct Link
		{
			ax::NodeEditor::LinkId id;

			ax::NodeEditor::PinId startPinID;
			ax::NodeEditor::PinId endPinID;

			ImColor color;

			Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId) :
				id(id), startPinID(startPinId), endPinID(endPinId), color(255, 255, 255)
			{
			}
		};
	private:
		ImVec2 cursorPos;
		static float CalcMaxPopupHeightFromItemCount(int items_count)
		{
			ImGuiContext& g = *GImGui;
			if (items_count <= 0)
				return FLT_MAX;
			return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
		}
		bool BeginNodeCombo(const char* label, const char* preview_value, ImGuiComboFlags flags, ImVec2& pos);

		void EndNodeCombo();
	};
}