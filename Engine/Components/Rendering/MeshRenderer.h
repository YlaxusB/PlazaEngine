#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Components/Rendering/Mesh.h"

using namespace std;
namespace Plaza {
	class MeshRenderer : public Component {
	public:
		string aiMeshName;
		uint64_t uuid;
		std::string meshName;
		std::shared_ptr<Mesh> mesh;
		Transform* transform;
		bool instanced = false;
		MeshRenderer(Mesh initialMesh, bool addToScene = false);
		MeshRenderer(const MeshRenderer& other) = default;
		~MeshRenderer();
		MeshRenderer() {
			uuid = Plaza::UUID::NewUUID();
		}
	};
}