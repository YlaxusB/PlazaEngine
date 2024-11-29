#include "Engine/Core/PreCompiledHeaders.h"
#include "RenderGraphEditor.h"
#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Core/Renderer/Vulkan/VulkanRenderGraph.h"

#include <ThirdParty/pfr/include/boost/pfr/core.hpp>
#include <ThirdParty/pfr/include/boost/pfr/core_name.hpp>
#include <iostream>
#include <string>

namespace Plaza::Editor {
	void RenderGraphEditor::Init() {

		this->AddNodeToCreateByType<PlTextureType>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlViewType>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlBufferType>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlTextureFormat>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlImageUsage>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlImageLayout>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlRenderStage>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlBindingType>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlMemoryProperty>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlBufferUsage>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlMemoryUsage>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlRenderPassMode>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlPrimitiveTopology>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlPolygonMode>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlCullMode>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlFrontFace>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlSampleCount>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlCompareOp>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlBlendFactor>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlBlendOp>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlColorComponentFlags>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlLogicOp>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlStencilOp>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlDynamicState>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlVertexInputRate>("Render Graph / Enums");
		this->AddNodeToCreateByType<PlSamplerAddressMode>("Render Graph / Enums");

		this->AddNodeToCreateByType<PlPipelineRasterizationStateCreateInfo>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineColorBlendAttachmentState>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineColorBlendStateCreateInfo>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlStencilOpState>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineDepthStencilStateCreateInfo>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlViewport>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlRect2D>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineViewportStateCreateInfo>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineMultisampleStateCreateInfo>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlVertexInputBindingDescription>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlVertexInputAttributeDescription>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineShaderStageCreateInfo>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPushConstantRange>("Render Graph / Structs");
		this->AddNodeToCreateByType<PlPipelineCreateInfo>("Render Graph / Structs");

		this->SetFinalNode<PlPipelineCreateInfo>();
		this->InitNodeEditor();
		this->SpawnNode(typeid(PlPipelineCreateInfo).name()).inputs[0].SetValue<std::string>("Draw to SwapChain Pass");

	};

	void RenderGraphEditor::Process() {
		ax::NodeEditor::SetCurrentEditor(mContext);
		int nodeCount = ax::NodeEditor::GetNodeCount();
		std::vector<uintptr_t> orderedNodeIds = this->GetOrderedNodes();
		std::set<uintptr_t> executedNodeIds = std::set<uintptr_t>();
		//orderedNodeIds.resize(static_cast<size_t>(nodeCount));
		//ax::NodeEditor::GetOrderedNodeIds(orderedNodeIds.data(), nodeCount);
		PlPipelineCreateInfo info{};
		mNodes[mFinalNodeId].outputs[0].SetValue<PlPipelineCreateInfo>(info);
		for (int i = orderedNodeIds.size() - 1; i >= 0; i--) {
			uintptr_t nodeId = orderedNodeIds[i];
			if (executedNodeIds.find(nodeId) != executedNodeIds.end())
				continue;
			mNodes.at(nodeId).Process();//.processFunction(mNodes.at(nodeId));
			executedNodeIds.emplace(nodeId);
			//auto it = std::find_if(mNodes.begin(), mNodes.end(), [&](const Node& node) {
			//	return node.id == nodeId.Get();
			//	});
			////if (it != mNodes.end())
			////	std::swap(*it, mNodes[i]);
		}
		ax::NodeEditor::SetCurrentEditor(nullptr);

		//for (auto& node : mNodes) {
		//	node.processFunction(node);
		//}
		Node& finalNode = mNodes[mFinalNodeId];
		info = finalNode.inputs[0].nodes[1]->outputs[0].GetValue<PlPipelineCreateInfo>();
		PL_CORE_INFO("Alright");
	}
}