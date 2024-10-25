#include "Engine/Core/PreCompiledHeaders.h"
#include "Mono.h"
#include <functional>
#include "ThirdParty/mono/include/mono/metadata/object.h"
#include "ThirdParty/mono/include/mono/metadata/threads.h"
#include "ThirdParty/mono/include/mono/metadata/mono-debug.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Engine/Components/Physics/CharacterController.h"
#include "Engine/Components/Rendering/Light.h"
#include "Engine/Components/Rendering/AnimationComponent.h"
#include "Engine/Components/Drawing/UI/Gui.h"

char* ConvertConstCharToChar(const char* constCharString) {
	// Calculate the length of the input string
	size_t length = strlen(constCharString) + 1; // Include null terminator

	// Allocate memory for the mutable copy
	char* mutableCopy = new char[length];

	// Copy the contents of the const char* to the mutable copy
	strcpy_s(mutableCopy, length, constCharString);

	return mutableCopy;
}

#define PL_ADD_INTERNAL_CALL(name) mono_add_internal_call("Plaza.InternalCalls::" #name, (void*)InternalCalls::name)
namespace Plaza {
	char* ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* Mono::LoadCSharpAssembly(const std::string& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			printf(errorMessage);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			printf("%s.%s\n", nameSpace, name);
		}
	}

	MonoClass* Mono::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className, bool isCore)
	{
		MonoClass* monoClass = mono_class_from_name(mono_assembly_get_image(isCore ? mCoreAssembly : assembly), namespaceName, className);

		if (monoClass == nullptr)
		{
			// Log error here
			printf("monoClass is nullptr");
			return nullptr;
		}

		return monoClass;
	}
	MonoObject* Mono::InstantiateClass(const char* namespaceName, const char* className, MonoAssembly* assembly, MonoDomain* appDomain, uint64_t uuid)
	{
		// Get a reference to the class we want to instantiate
		MonoClass* monoClass = GetClassInAssembly(assembly, namespaceName, className);

		// Allocate an instance of our class
		MonoObject* classInstance = mono_object_new(appDomain, monoClass);
		unsigned int gcHandle = mono_gchandle_new(classInstance, true);
		if (uuid) {
			MonoClassField* uuidField = mono_class_get_field_from_name(monoClass, "Uuid");
			mono_field_set_value(classInstance, uuidField, &uuid);
		}

		// Call the parameterless (default) constructor
		mono_runtime_object_init(classInstance);
		return classInstance;
	}

	/*
		void Mono::CallMethod(MonoObject* objectInstance, std::string methodName)
	{
		// Get the MonoClass pointer from the instance
		MonoClass* instanceClass = mono_object_get_class(objectInstance);

		// Get a reference to the method in the class
		MonoMethod* method = mono_class_get_method_from_name(instanceClass, methodName.c_str(), 0);
	*/

	MonoMethod* Mono::GetMethod(MonoObject* objectInstance, const std::string& methodName, int parameterCount)
	{
		MonoClass* monoClass = mono_object_get_class(objectInstance);
		MonoMethod* method = mono_class_get_method_from_name(monoClass, methodName.c_str(), parameterCount);
		return method;
	}

	void Mono::CallMethod(MonoObject* objectInstance, MonoMethod* method)
	{
		if (method) {
			MonoObject* exception = nullptr;
			mono_runtime_invoke(method, objectInstance, nullptr, &exception);
		}
	}

	void Mono::CallMethod(MonoObject* objectInstance, MonoMethod* method, void* param)
	{
		if (method) {
			MonoObject* exception = nullptr;
			mono_runtime_invoke(method, objectInstance, &param, &exception);
		}
	}

	void Mono::CallMethod(MonoObject* objectInstance, MonoMethod* method, void* params[])
	{
		if (method) {
			MonoObject* exception = nullptr;
			mono_runtime_invoke(method, objectInstance, params, &exception);
		}
	}

	void Mono::CallMethod(MonoObject* objectInstance, std::string methodName)
	{
		// Get the MonoClass pointer from the instance
		MonoClass* instanceClass = mono_object_get_class(objectInstance);

		// Get a reference to the method in the class
		MonoMethod* method = mono_class_get_method_from_name(instanceClass, methodName.c_str(), 0);

		if (method == nullptr)
		{
			// No method called "PrintFloatVar" with 0 parameters in the class, log error or something
			return;
		}

		// Call the C# method on the objectInstance instance, and get any potential exceptions
		MonoObject* exception = nullptr;
		mono_runtime_invoke(method, objectInstance, nullptr, &exception);

		// Check if an exception occurred
		if (exception != nullptr) {
			// Get the MonoException type
			MonoClass* exceptionClass = mono_get_exception_class();
			MonoString* messageString = mono_object_to_string((MonoObject*)exception, nullptr);
			const char* message = mono_string_to_utf8(messageString);
			printf("Exception occurred: %s\n", message);

		}
	}

	static void CppFunction() {
		std::cout << "Writen in C++" << std::endl;
	}

	static void Vector3Log(glm::vec3* vec3) {
		std::cout << "X: " << vec3->x << std::endl;
		std::cout << "Y: " << vec3->y << std::endl;
		std::cout << "Z: " << vec3->z << std::endl;
	}

	void Mono::Init() {
		mono_set_assemblies_path(Application::Get()->projectPath.c_str());
		//mono_set_assemblies_path((Application::Get()->editorPath + "/lib/mono").c_str());

		if (Mono::mMonoRootDomain == nullptr)
			Mono::mMonoRootDomain = mono_jit_init("MyScriptRuntime");
		if (Mono::mMonoRootDomain == nullptr)
		{
			// Maybe log some error here
			return;
		}

		// Create an App Domain
		char appDomainName[] = "PlazaAppDomain";
		Mono::mAppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
		mono_domain_set(mAppDomain, true);

		// Load the PlazaScriptCore.dll assembly
#ifdef GAME_MODE
		Application::Get()->dllPath = Application::Get()->projectPath + "\\Binaries";
#endif // GAME_REL

#ifdef EDITOR_MODE
		std::string engineDllPath = std::filesystem::path{ Application::Get()->enginePath }.parent_path().string() + "\\PlazaScriptCore\\obj\\Release\\PlazaScriptCore.dll";
		bool foundDllInEngineFolder = std::filesystem::exists(engineDllPath);
		if (foundDllInEngineFolder) {
			std::filesystem::copy_file(engineDllPath, Application::Get()->projectPath + "\\Binaries\\PlazaScriptCore.dll", std::filesystem::copy_options::update_existing);
		}
#endif

		mCoreAssembly = mono_domain_assembly_open(mAppDomain, (Application::Get()->dllPath + "\\PlazaScriptCore.dll").c_str());
		mCoreImage = mono_image_open((Application::Get()->dllPath + "\\PlazaScriptCore.dll").c_str(), nullptr);

		Mono::RegisterComponents();
		InternalCalls::Init();


		if (!mCoreAssembly) {
			// Handle the error (assembly not found or failed to load)
			std::cout << "Didnt loaded assembly" << std::endl;
		}





		//// Load all scripts
		//for (auto& [key, value] : Application::Get()->activeProject->scripts) {
		//	Application::Get()->activeProject->scripts.emplace(key, Script());
		//}

		mEntityObject = InstantiateClass("Plaza", "Entity", LoadCSharpAssembly(Application::Get()->dllPath + "\\PlazaScriptCore.dll"), mAppDomain);
		mEntityClass = mono_object_get_class(mEntityObject);

#ifdef GAME_MODE
		Mono::mScriptAssembly = mono_domain_assembly_open(Mono::mAppDomain, (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll").c_str());
#else
		const std::string dllPath = Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll";
		const std::string copyDllPath = Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + "copy.dll";
		if (!std::filesystem::exists(copyDllPath) && std::filesystem::exists(dllPath)) {
			std::filesystem::copy(std::filesystem::path{ dllPath }, std::filesystem::path{ copyDllPath });
		}
		Mono::mScriptAssembly = mono_domain_assembly_open(Mono::mAppDomain, copyDllPath.c_str());
#endif
		if (!Mono::mScriptAssembly) {
			// Handle assembly loading error
			std::cout << "Failed to load assembly on path: " << (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll").c_str() << "\n";
		}
		else
			Mono::mScriptImage = mono_assembly_get_image(Mono::mScriptAssembly);
	}

	void Mono::OnStart(MonoObject* monoObject) {
		CallMethod(monoObject, "OnStart");
	}

	// Execute OnStart on all scripts
	void Mono::OnStartAll(bool callOnStart) {
#ifdef GAME_MODE
		Editor::ScriptManager::ReloadScriptsAssembly();
#else
		/* Make a copy of the scripts dll, so it does not break when it updates */
		std::string dllPath = (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll");
		std::string newPath = (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + "copy.dll");
		Editor::ScriptManager::ReloadScriptsAssembly(newPath);
#endif
		if (callOnStart) {
			for (auto& [key, value] : Scene::GetActiveScene()->csScriptComponents) {
				for (auto& [className, classScript] : value.scriptClasses) {
					CallMethod(classScript->monoObject, classScript->onStartMethod);
				}
			}
		}
	}

	// Execute OnUpdate on all scripts
	void Mono::Update() {
		for (auto& [key, value] : Scene::GetActiveScene()->csScriptComponents) {
			for (auto& [className, classScript] : value.scriptClasses) {
				CallMethod(classScript->monoObject, classScript->onUpdateMethod);
			}
		}
		//CallMethod(value.monoObject, "OnUpdate");
	}

	template<typename Component>
	static void RegisterComponent() {
		std::string typeName = typeid(Component).name();
		size_t pos = typeName.find_last_of(":");
		std::string className = typeName.substr(pos + 1);
		std::string  managedTypeName = "Plaza." + className;
		MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), Mono::mCoreImage);
		Mono::mEntityHasComponentFunctions[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		Mono::mEntityAddComponentFunctions[managedType] = [](Entity entity) { return entity.AddComp<Component>(); };
		Mono::mEntityGetComponentFunctions[managedType] = [](Entity entity) { return entity.GetComponent<Component>(); };
	}

	void Mono::RegisterComponents() {
		RegisterComponent<Transform>();
		RegisterComponent<MeshRenderer>();
		RegisterComponent<Camera>();
		RegisterComponent<Collider>();
		RegisterComponent<RigidBody>();
		RegisterComponent<CharacterController>();
		RegisterComponent<CsScriptComponent>();
		RegisterComponent<Plaza::Drawing::UI::TextRenderer>();
		RegisterComponent<AudioSource>();
		RegisterComponent<AudioListener>();
		RegisterComponent<Light>();
		RegisterComponent<AnimationComponent>();
		RegisterComponent<GuiComponent>();
	}

	void Mono::ReloadAppDomain() {
		/* Unload and Load again the app domain */
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(Mono::mAppDomain);
		//mono_domain_unload(Mono::mAppDomain);
		//Mono::Init();
	}
}