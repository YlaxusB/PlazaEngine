#pragma once
#include "Editor/GUI/GuiWindow.h"
#include "ThirdParty/imgui/nodes/imgui_node_editor.h"
#include "ThirdParty/imgui/imgui_internal.h"
#include <any>
#include "ThirdParty/magic_enum/magic_enum.hpp"
#include "Engine/Core/EnumReflection.h"
#include "Engine/Core/Any.h"

//namespace ed = ax::NodeEditor;
namespace Plaza::Editor {
	class NodeEditor : public GuiWindow {
	public:
		std::string mName = "";
		ax::NodeEditor::EditorContext* mContext = nullptr;

		NodeEditor(const std::string& name, GuiLayer layer, bool startOpen = true) : GuiWindow(layer, startOpen) { }

		void InitNodeEditor();
		//void Init() override {};
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
		std::vector<Node> mNodes;
		std::vector<Link> mLinks;
		std::map<std::string, Node> mTemplateNodes;
		const float mTouchTime = 1.0f;
		static inline Any* sEnumToShowOnPopup = nullptr;
		static inline bool sOpenNodeEditorPopup = false;
		//std::map<ax::NodeEditor::NodeId, float, NodeIdLess> mNodeTouchTime;

		int GetNextId() { return mNextId++; }

		void NewNode(const std::string& nodeName);
		void RemoveNode(int index);

		void Process();

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
			Input,
			Constant
		};

		enum class NodeType
		{
			Blueprint,
			Simple,
			Tree,
			Comment,
			Houdini
		};
		class Pin
		{
		public:
			ax::NodeEditor::PinId   id;
			Node* node;
			std::string name;
			PinType	type;
			PinKind	kind;
			//std::any value;
			Any value;
			int enumSize = 0;
			int enumIndex = 0;

			Pin(int newId, const char* newName, PinType newType, PinKind newKind = PinKind::Input) :
				id(newId), node(nullptr), name(newName), type(newType), kind(newKind) { }

			template<typename T>
			T GetValue() {
				return *value.GetValue<T>();
			}

			template<typename T>
			void SetValue(const T& newValue, bool changeType = true) {
				value.SetValue(newValue, changeType);
			}

			template<typename T>
			void SetEnumValue(int newValue) {
				static_assert(std::is_enum<T>::value, "T must be an enum type.");
				value = static_cast<T>(newValue);
				enumIndex = newValue;

				enumSize = magic_enum::enum_count<T>();

				std::vector<const char*> names = std::vector<const char*>();
				for (int i = 0; i < enumSize; ++i) {
					names.push_back(magic_enum::enum_name(T(i)).data());
				}
				if (names.size() > 0 && (names[0] == nullptr || (names.size() > 1 && names[1] == nullptr)))
					EnumReflection::RegisterBitmaskEnum<T>();
				else
					EnumReflection::RegisterEnum<T>();
			}

			void SetEnumIndex(int newValue) {
				value.SetValue(newValue, false);
				enumIndex = newValue;
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
			std::function<void()> processFunction;
			void Process() {
				processFunction();
			}
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
		bool BeginNodeCombo(const char* label, const char* preview_value, ImGuiComboFlags flags);

		void EndNodeCombo();
	};
}