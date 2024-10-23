#pragma once
//#include "Engine/Core/ModelLoader/Model.h"

#ifdef ENGINE_EXPORTS
#define PLAZA_API __declspec(dllexport)
#else
#define PLAZA_API  __declspec(dllimport)
#endif

#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>
#define PL_SER(T) CEREAL_NVP(T)

namespace Plaza {
	class EngineClass {
	public:
		//static std::unordered_map<uint64_t, std::unique_ptr<Model>> models;
	};
}
// inline std::unordered_map<uint64_t, std::unique_ptr<Model>> EngineClass::models = std::unordered_map<uint64_t, std::unique_ptr<Model>>();