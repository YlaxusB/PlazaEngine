#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
namespace Plaza {
	class MaterialFileSerializer {
	public:
		static void Serialize(const std::string filePath, Material* material);
		static Material* DeSerialize(const std::string filePath);
	};
}