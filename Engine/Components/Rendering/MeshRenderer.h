#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Core/RenderGroup.h"
#include "Engine/Components/Core/Transform.h"
using namespace std;
namespace Plaza {
	class MeshRenderer : public Component {
	public:
		bool castShadows = true;
		string aiMeshName;
		//uint64_t uuid;
		std::string meshName;
		std::shared_ptr<Mesh> mesh;	
		std::shared_ptr<Material> material;
		std::shared_ptr<RenderGroup> renderGroup;
		Transform* transform;
		bool instanced = false;
		MeshRenderer(Mesh initialMesh, bool addToScene = false);
		MeshRenderer(Plaza::Mesh initialMesh, Material material, bool addToScene = false);
		MeshRenderer(const MeshRenderer& other) = default;
		~MeshRenderer();
		MeshRenderer() {
			uuid = Plaza::UUID::NewUUID();
		}
	};
}