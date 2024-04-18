#pragma once
namespace Plaza {
	class Prefab : public Component {
	public:
		uint64_t mPrefabUuid;
		uint64_t mAssetUuid;
		class Child;
		class Base;
		std::string mPrefabName;
	};

	class Prefab::Child : public Prefab {

	};
	class Prefab::Base : public Prefab {

	};
}