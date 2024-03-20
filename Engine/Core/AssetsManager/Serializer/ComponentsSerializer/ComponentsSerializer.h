#pragma once
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"

namespace Plaza {
	class ComponentsSerializer {
	public:
		static SerializableMeshRenderer* SerializeMeshRenderer(MeshRenderer* meshRenderer);
		static SerializableMesh SerializeMesh(Mesh* mesh);
	};
}