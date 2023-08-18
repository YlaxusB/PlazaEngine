#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Components/Rendering/Mesh.h"

using namespace std;
namespace Engine {
	class MeshRenderer : public Component {
	public:
		string aiMeshName;
		uint64_t uuid;
		std::string meshName;
		std::shared_ptr<Mesh> mesh;
		Transform* transform;
		bool instanced = false;
		MeshRenderer(Mesh initialMesh, bool addToScene = false);
		MeshRenderer(const MeshRenderer&) = default;
		~MeshRenderer();
		MeshRenderer() {
			uuid = Engine::UUID::NewUUID();
		}
	};
}