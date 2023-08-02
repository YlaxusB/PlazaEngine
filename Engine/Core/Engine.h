#pragma once
#include "Engine/Core/ModelLoader/Model.h"
namespace Engine {
	static class EngineClass {
	public:
		static std::vector<std::unique_ptr<Model>> models;
	};
}
inline std::vector<std::unique_ptr<Model>> EngineClass::models = std::vector<std::unique_ptr<Model>>();