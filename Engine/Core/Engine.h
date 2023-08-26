#pragma once
#include "Engine/Core/ModelLoader/Model.h"
namespace Plaza {
	static class EngineClass {
	public:
		static std::unordered_map<uint64_t, std::unique_ptr<Model>> models;
	};
}
inline std::unordered_map<uint64_t, std::unique_ptr<Model>> EngineClass::models = std::unordered_map<uint64_t, std::unique_ptr<Model>>();