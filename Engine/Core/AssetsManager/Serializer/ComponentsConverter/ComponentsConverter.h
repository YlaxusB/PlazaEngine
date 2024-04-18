#pragma once
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"

namespace Plaza {
	class ComponentsConverter {
	public:
		static SerializableMeshRenderer* ConvertMeshRenderer(MeshRenderer* meshRenderer);
		static SerializableMaterial ConvertMaterial(Material* material);
		static SerializableMesh ConvertMesh(Mesh* mesh);
		static SerializableCollider* ConvertCollider(Collider* collider);
	};
}