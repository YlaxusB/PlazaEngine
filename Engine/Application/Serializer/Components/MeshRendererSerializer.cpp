#include "Engine/Core/PreCompiledHeaders.h"
#include "MeshRendererSerializer.h"
#include "MeshSerializer.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Plaza {
	void ComponentSerializer::MeshRendererSerializer::Serialize(YAML::Emitter& out, MeshRenderer& meshRenderer) {
		out << YAML::Key << "MeshRendererComponent" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Uuid" << YAML::Value << meshRenderer.mUuid;
		out << YAML::Key << "CastShadows" << YAML::Value << meshRenderer.castShadows;
		out << YAML::Key << "AiMeshName" << YAML::Value << meshRenderer.aiMeshName;
		out << YAML::Key << "MeshName" << YAML::Value << meshRenderer.meshName;
		out << YAML::Key << "Instanced" << YAML::Value << meshRenderer.instanced;
		uint64_t meshId = meshRenderer.mesh->meshId;
		out << YAML::Key << "MeshId" << YAML::Value << meshId;
		// TODO: FIX MATERIAL SERIALIZATION
		//out << YAML::Key << "Material" << YAML::Value << meshRenderer.material->uuid;
		//ComponentSerializer::MeshSerializer::Serialize(out, *meshRenderer.mesh);
		out << YAML::EndMap;
	}
	MeshRenderer* ComponentSerializer::MeshRendererSerializer::DeSerialize(YAML::Node data)
	{
		auto meshRenderDeserialized = data;
		MeshRenderer* meshRenderer = new MeshRenderer();//new MeshRenderer(*shared_ptr<Mesh>(Application->activeScene->meshes.at(meshRenderDeserialized["MeshId"].as<uint64_t>())).get(), Application->activeScene->meshes.at(meshRenderDeserialized["MeshId"].as<uint64_t>())->material);
		meshRenderer->instanced = data["Instanced"].as<bool>();
		if (data["CastShadows"])
			meshRenderer->castShadows = data["CastShadows"].as<bool>();
		uint64_t meshId = meshRenderDeserialized["MeshId"].as<uint64_t>();
		if (AssetsManager::HasMesh(meshRenderDeserialized["MeshId"].as<uint64_t>()))
			meshRenderer->mesh = AssetsManager::GetMesh(meshRenderDeserialized["MeshId"].as<uint64_t>());//Application->activeScene->meshes.at(meshRenderDeserialized["MeshId"].as<uint64_t>()).get();
		else
			meshRenderer->mesh = nullptr;//new OpenGLMesh();
		uint64_t materialUuid;
		if (data["Material"])
			materialUuid = data["Material"].as<uint64_t>();;
		Material* material;
		if (Application->activeScene->materials.find(materialUuid) != Application->activeScene->materials.end() && materialUuid != 0) {
			material = Application->activeScene->materials.at(materialUuid).get();
		}
		else
			material = Scene::DefaultMaterial();
		meshRenderer->mMaterials.push_back(material);
		if (meshRenderer->mesh && meshRenderer->mMaterials.size() > 0) {
			meshRenderer->renderGroup = Application->activeScene->AddRenderGroup(meshRenderer->mesh, meshRenderer->mMaterials);//std::make_shared<RenderGroup>(meshRenderer->mesh, meshRenderer->material);
			//Application->activeScene->AddRenderGroup(meshRenderer->renderGroup);
		}
		return meshRenderer;
	}
}