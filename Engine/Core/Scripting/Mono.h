#pragma once
namespace Plaza {
	class Mono {
	public:
		static MonoDomain* mAppDomain;
		static MonoAssembly* mCoreAssembly;
		static MonoDomain* mMonoRootDomain;
		static void Init();
		static void OnStart();
		static void Update();
		static MonoObject* InstantiateClass(const char* namespaceName, const char* className, MonoAssembly* assembly, MonoDomain* appDomain);

		static void CallMethod(MonoObject* objectInstance, std::string methodName);
		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
	};

	class InternalCalls {
	public:
		static void Init();
	};
}