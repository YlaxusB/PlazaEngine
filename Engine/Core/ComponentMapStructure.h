#pragma once
#include <vector>
#include <variant>
#include <unordered_map>
#include "Engine/Components/Component.h"
#include "Engine/Core/Engine.h"

namespace Plaza {
	template <typename Key, typename T>
	class ComponentMultiMap : public std::unordered_multimap<Key, T> {
	public:
		T& at(const Key& key) {
			auto range = this->equal_range(key);
			if (range.first != range.second) {
				return range.first->second; // Returning the first value in the range
			}
		}

		T& operator [](const Key& key) {
			auto range = this->equal_range(key);
			if (range.first != range.second) {
				return range.first->second; // Returning the first value in the range
			}
		}
	};
};