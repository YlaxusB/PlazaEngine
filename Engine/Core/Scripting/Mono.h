#pragma once
#include "Engine/Core/Engine.h"

namespace Plaza {
	class PLAZA_API Mono {
	public:
		static inline MonoDomain* mAppDomain = nullptr;
		static inline MonoAssembly* mCoreAssembly = nullptr;
		static inline MonoDomain* mMonoRootDomain = nullptr;
		static inline MonoImage* mCoreImage = nullptr;
		static inline MonoObject* mEntityObject = nullptr;
		static inline MonoClass* mEntityClass = nullptr;
		static inline MonoAssembly* mScriptAssembly = nullptr;
		static inline MonoImage* mScriptImage = nullptr;

		static inline std::unordered_map<MonoType*, std::function<bool(Entity)>> mEntityHasComponentFunctions = std::unordered_map<MonoType*, std::function<bool(Entity)>>();
		static inline std::unordered_map<MonoType*, std::function<Component* (Entity)>> mEntityAddComponentFunctions = std::unordered_map<MonoType*, std::function<Component* (Entity)>>();
		static inline std::unordered_map<MonoType*, std::function<Component* (Entity)>> mEntityGetComponentFunctions = std::unordered_map<MonoType*, std::function<Component* (Entity)>>();
		static void Init();
		static void OnStartAll(bool callOnStart = true);
		static void OnStart(MonoObject* monoObject);
		static void Update();
		static void RegisterComponents();
		static MonoObject* InstantiateClass(const char* namespaceName, const char* className, MonoAssembly* assembly, MonoDomain* appDomain, uint64_t uuid = 0);

		static void ReloadAppDomain();
		static MonoMethod* GetMethod(MonoObject* objectInstance, const std::string& methodName, int parameterCount = 0);
		static void CallMethod(MonoObject* objectInstance, std::string methodName);
		static void CallMethod(MonoObject* objectInstance, MonoMethod* method);
		static void CallMethod(MonoObject* objectInstance, MonoMethod* method, void* param);
		static void CallMethod(MonoObject* objectInstance, MonoMethod* method, void* params[]);
		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className, bool isCore = false);
		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
	};

	class InternalCalls {
	public:
		static void Init();
	};
}