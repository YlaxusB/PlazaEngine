#pragma once
namespace Plaza {
	class Mono {
	public:
		static MonoDomain* mAppDomain;
		static MonoAssembly* mCoreAssembly;
		static void Init();
		static void OnStart();
		static void Update();
		static MonoObject* InstantiateClass(const char* namespaceName, const char* className, MonoAssembly* assembly, MonoDomain* appDomain);
		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
	};
}