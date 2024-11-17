#include "Engine/Core/PreCompiledHeaders.h"
#include "RenderGraphEditor.h"
#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Core/Renderer/Vulkan/VulkanRenderGraph.h"

namespace Plaza::Editor {
	void RenderGraphEditor::Init() {
		this->InitNodeEditor();
		
		Node node = Node();
		node.name = "Multiply";
		node.inputs.push_back(Pin(0, "X", PinType::Float));
		node.outputs.push_back(Pin(0, "Y", PinType::Float, PinKind::Output));
		node.processFunction = [&]() {
			node.outputs[0].SetValue<float>(node.inputs[0].GetValue<float>() * 10.0f);
			};
		AddNodeToCreate(node);

		node = Node();
		node.name = "Texture Info";
		node.inputs.push_back(Pin(0, "Descriptor Count", PinType::Int));
		node.inputs.back().SetValue<int>(0);

		node.inputs.push_back(Pin(1, "Texture Format", PinType::Enum));
		node.inputs.back().SetEnumValue<PlTextureFormat>(0);

		node.inputs.push_back(Pin(2, "Image Usage", PinType::Enum));
		node.inputs.back().SetEnumValue<PlImageUsage>(0);

		node.inputs.push_back(Pin(3, "Initial Layout", PinType::Enum));
		node.inputs.back().SetEnumValue<PlImageLayout>(0);

		node.inputs.push_back(Pin(4, "Descriptor Count 2", PinType::Int));
		node.inputs.back().SetValue<int>(0);

		node.outputs.push_back(Pin(0, "Out", PinType::Object, PinKind::Output));
		node.processFunction = [&]() {
			TextureInfo info{};
			info.mDescriptorCount = node.inputs[0].GetValue<uint32_t>();
			info.mFormat = node.inputs[1].GetValue<PlTextureFormat>();
			info.mImageUsage = node.inputs[2].GetValue<PlImageUsage>();
			info.mInitialLayout = node.inputs[3].GetValue<PlImageLayout>();
			node.outputs[0].SetValue<TextureInfo>(info);
			};
		AddNodeToCreate(node);
	};
}