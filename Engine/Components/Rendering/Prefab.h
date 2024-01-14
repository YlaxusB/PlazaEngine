#pragma once
namespace Plaza {
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