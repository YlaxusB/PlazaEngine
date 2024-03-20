#include "Engine/Core/PreCompiledHeaders.h"
#include "ComponentsSerializer.h"

namespace Plaza {
	SerializableMeshRenderer* ComponentsSerializer::SerializeMeshRenderer(MeshRenderer* meshRenderer) {
		SerializableMeshRenderer* serializedMeshRenderer = new SerializableMeshRenderer();
		serializedMeshRenderer->uuid = meshRenderer->uuid;
		serializedMeshRenderer->type = SerializableComponentType::MESH_RENDERER;
		serializedMeshRenderer->serializedMesh = ComponentsSerializer::SerializeMesh(meshRenderer->mesh);
		return serializedMeshRenderer;
	}
}