#include "Engine/Core/PreCompiledHeaders.h"
#include "ComponentsConverter.h"

namespace Plaza {
	SerializableMeshRenderer* ComponentsConverter::ConvertMeshRenderer(MeshRenderer* meshRenderer) {
		SerializableMeshRenderer* serializedMeshRenderer = new SerializableMeshRenderer();
		serializedMeshRenderer->uuid = meshRenderer->mUuid;
		serializedMeshRenderer->type = SerializableComponentType::MESH_RENDERER;
		serializedMeshRenderer->materialsUuid = Material::GetMaterialsUuids(meshRenderer->mMaterials);
		serializedMeshRenderer->serializedMesh = ComponentsConverter::ConvertMesh(meshRenderer->mesh);
		return serializedMeshRenderer;
	}
}