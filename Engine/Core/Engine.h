#pragma once
//#include "Engine/Core/ModelLoader/Model.h"

#ifdef ENGINE_EXPORTS
#define PLAZA_API __declspec(dllexport)
#else
#define PLAZA_API  __declspec(dllimport)
#endif

#include <ThirdParty/cereal/cereal/archives/json.hpp>
#include <ThirdParty/cereal/cereal/types/string.hpp>
#include <ThirdParty/cereal/cereal/types/vector.hpp>
#include <ThirdParty/cereal/cereal/cereal.hpp>
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/map.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>
#include <ThirdParty/cereal/cereal/types/utility.hpp>
#define PL_SER(T) CEREAL_NVP(T)
#define PL_SER_REGISTER_TYPE(T) CEREAL_REGISTER_TYPE(T)
#define PL_SER_REGISTER_POLYMORPHIC_RELATION(A, B) CEREAL_REGISTER_POLYMORPHIC_RELATION(A, B)

namespace cereal {
	template <class Archive, typename T, std::size_t N>
	void serialize(Archive& archive, std::array<T, N>& arr) {
		for (auto& elem : arr) {
			archive(elem);
		}
	}
}

namespace Plaza {
	class EngineClass {
	public:
		//static std::unordered_map<uint64_t, std::unique_ptr<Model>> models;
	};
}
// inline std::unordered_map<uint64_t, std::unique_ptr<Model>> EngineClass::models = std::unordered_map<uint64_t, std::unique_ptr<Model>>();