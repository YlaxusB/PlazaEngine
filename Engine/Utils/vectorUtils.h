#pragma once
#include <iostream>
#include <random>
#include <unordered_map>
#include <fileSystem>
#include "Engine/Vendor/filesystem/filesys.h"
#include <algorithm>

namespace Utils {
	namespace Vector {
		template<typename T>
		inline int indexOf(std::vector<T*> vector, T* item) {
			// Assuming you have a pointer (ptr) to the item you want to find the index of
			auto it = std::find(vector.begin(), vector.end(), item);

			if (it != vector.end()) {
				// Item found
				return std::distance(vector.begin(), it);
			}
			else {
				std::cout << "Item doesnt belongs to this vector" << std::endl;
				return -1;
			}
		}
	}
}