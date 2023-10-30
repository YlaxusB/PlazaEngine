#pragma once
#include <vector>
#include <variant>
#include <unordered_map>
#include "Engine/Components/Component.h"
namespace Plaza {
	template <typename Key, typename T>
	class ComponentMultiMap : public std::unordered_multimap<Key, T> {
	public:
		std::unordered_multimap<std::string, uint64_t> asd = std::unordered_multimap<std::string, uint64_t>();
		std::map<std::string, uint64_t> ed = std::map<std::string, uint64_t>();
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