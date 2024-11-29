#pragma once
#include "NodeEditor.h"

namespace Plaza::Editor {
	class RenderGraphEditor : public NodeEditor {
	public:
		RenderGraphEditor(GuiLayer layer, bool startOpen = true) : NodeEditor("RenderGraphEditor", layer, startOpen) { }
		void Init() override;
		void Process() override;
		//void Update() override;
		//void OnKeyPress(int key, int scancode, int action, int mods) override;

		//ax::NodeEditor::EditorContext* mContext = nullptr;
	};
}