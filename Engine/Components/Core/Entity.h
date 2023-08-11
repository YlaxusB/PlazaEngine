#pragma once
#include <variant>
namespace Engine {
	class Entity {
	public:
		uint64_t uuid;
		~Entity() = default;
		Entity();
	};
}