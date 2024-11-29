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

		Node renderPassNode = Node();
		renderPassNode.name = "Render Pass";
		this->AddInputPin(renderPassNode, Pin(0, "Pass Name", PinType::String, PinKind::Constant)).SetValue<std::string>("Render Pass");
		this->AddInputPin(renderPassNode, Pin(0, "Stages", PinType::Enum, PinKind::Constant)).SetEnumValue<PlRenderStage>(0);
		this->AddInputPin(renderPassNode, Pin(0, "Render Mode", PinType::Enum, PinKind::Constant)).SetEnumValue<PlRenderPassMode>(0);
		this->AddInputPin(renderPassNode, Pin(0, "Multi view count", PinType::Int, PinKind::Constant)).SetValue<int>(0);
		this->AddInputPin(renderPassNode, Pin(0, "Render Size", PinType::Vector2, PinKind::Constant)).SetValue<glm::vec2>(glm::vec2(1920.0f, 1080.0f));
		this->AddInputPin(renderPassNode, Pin(0, "Dispatch Size", PinType::Vector3, PinKind::Constant)).SetValue<glm::vec3>(glm::vec3(0.0f));
		this->AddInputPin(renderPassNode, Pin(0, "Flip Viewport", PinType::Bool, PinKind::Constant)).SetValue(true);
		this->AddOutputPin(renderPassNode, Pin(0, "Out", PinType::Object, PinKind::Output)).value.SetType<PlazaRenderPass>();
		renderPassNode.processFunction = [](Node& node) {

			/*
					std::string mName = "";
		int16_t mExecutionIndex = 0;
		int mStage = 0;
		PlRenderPassMode mRenderMethod = PL_RENDER_PASS_FULL_SCREEN_QUAD;
		uint16_t mMultiViewCount = 0;
		glm::vec2 mRenderSize = glm::vec2(0, 0);
		glm::vec3 mDispatchSize = glm::vec3(0, 0, 0);
		bool mFlipViewPort = true;
			*/

			PlazaRenderPass pass{};
			pass.mName = node.inputs[0].GetValue<std::string>();
			pass.mStage = node.inputs[1].GetValue<PlRenderStage>();
			pass.mRenderMethod = node.inputs[2].GetValue<PlRenderPassMode>();
			pass.mMultiViewCount = node.inputs[3].GetValue<int>();
			pass.mRenderSize = node.inputs[4].GetValue<glm::vec2>();
			pass.mDispatchSize = node.inputs[5].GetValue<glm::vec3>();
			pass.mFlipViewPort = node.inputs[6].GetValue<bool>();
			node.outputs[0].SetValue(pass);
			};
		this->AddNodeToCreate(renderPassNode);

		this->SetFinalNode<PlPipelineCreateInfo>();
		this->InitNodeEditor();
		this->SpawnNode(typeid(PlPipelineCreateInfo).name()).inputs[0].SetValue<std::string>("Draw to SwapChain Pass");

	};

	void RenderGraphEditor::Process() {
		ax::NodeEditor::SetCurrentEditor(mContext);
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
		if (finalNode.inputs[0].nodes.size() > 1)
			info = finalNode.inputs[0].nodes[1]->outputs[0].GetValue<PlPipelineCreateInfo>();

		VulkanRenderGraph* renderGraph = new VulkanRenderGraph();
		for (auto& [key, node] : mNodes) {
			Any& value = node.outputs[0].value;
			if (value.type() == typeid(PlPipelineCreateInfo)) {
				PlPipelineCreateInfo info = node.outputs[0].GetValue<PlPipelineCreateInfo>();
				renderGraph->CreatePipeline(info);
			}
			else if (value.type() == typeid(PlazaRenderPass)) {
				PlazaRenderPass pass = node.outputs[0].GetValue<PlazaRenderPass>();
				renderGraph->AddRenderPass(std::make_shared<PlazaRenderPass>(pass));
			}

		}

		renderGraph->OrderPasses();
		renderGraph->UpdateUsedTexturesInfo();
		renderGraph->Compile();
		renderGraph->CompileNotBoundBuffers();

		Application::Get()->mRenderer->mRenderGraph = renderGraph;
		Application::Get()->mRenderer->UpdateImGuiDisplayTexture(Application::Get()->mRenderer->mRenderGraph->GetTexture<VulkanTexture>("FinalTexture"));

		PL_CORE_INFO("Alright");
	}
}