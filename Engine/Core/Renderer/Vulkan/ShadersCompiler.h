#pragma once
namespace Plaza {
	class VulkanShadersCompiler {
	public:
		static std::string mGlslcExePath;
		static std::string mDefaultOutDirectory;
		static void Compile(std::string shadersPath, std::string outDirectory = mDefaultOutDirectory);
	};
}