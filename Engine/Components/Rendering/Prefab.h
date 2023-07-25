#pragma once
#include "Engine/Components/Core/Mesh.h"
namespace Engine {
	class Prefab : public Component {
	public:
		class Child;
		class Base;
		std::string prefabId;
		std::string prefabName;
	};

	class Prefab::Child : public Prefab {

	};
	class Prefab::Base : public Prefab {

	};
}