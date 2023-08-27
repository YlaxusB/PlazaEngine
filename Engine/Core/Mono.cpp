#include "Engine/Core/PreCompiledHeaders.h"
#include "Mono.h"
#include "Engine/Vendor/mono/jit/jit.h"
#include "Engine/Vendor/mono/metadata/assembly.h"

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

    MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
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

    MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
    {
        MonoImage* image = mono_assembly_get_image(assembly);
        MonoClass* klass = mono_class_from_name(image, namespaceName, className);

        if (klass == nullptr)
        {
            // Log error here
            return nullptr;
        }

        return klass;
    }

    MonoObject* InstantiateClass(const char* namespaceName, const char* className, MonoAssembly* assembly, MonoDomain* appDomain)
    {
        // Get a reference to the class we want to instantiate
        MonoClass* testingClass = GetClassInAssembly(assembly, "ClassLibrary1", "Class1");

        // Allocate an instance of our class
        MonoObject* classInstance = mono_object_new(appDomain, testingClass);

        if (classInstance == nullptr)
        {
            // Log error here and abort
        }

        // Call the parameterless (default) constructor
        mono_runtime_object_init(classInstance);
        return classInstance;
    }

    void CallPrintFloatVarMethod(MonoObject* objectInstance)
    {
        // Get the MonoClass pointer from the instance
        MonoClass* instanceClass = mono_object_get_class(objectInstance);

        // Get a reference to the method in the class
        MonoMethod* method = mono_class_get_method_from_name(instanceClass, "PrintFloatVar", 0);

        if (method == nullptr)
        {
            // No method called "PrintFloatVar" with 0 parameters in the class, log error or something
            return;
        }

        // Call the C# method on the objectInstance instance, and get any potential exceptions
        MonoObject* exception = nullptr;
        mono_runtime_invoke(method, objectInstance, nullptr, &exception);

        // TODO: Handle the exception
    }


    void Mono::Init() {
        mono_set_assemblies_path("lib/mono");
        //mono_set_assemblies_path((Application->editorPath + "/lib/mono").c_str());
        MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
        if (rootDomain == nullptr)
        {
            // Maybe log some error here
            return;
        }

        // Create an App Domain
        char appDomainName[] = "MyAppDomain";
        MonoDomain* s_AppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
        mono_domain_set(s_AppDomain, true);

        // Get a reference to the class we want to instantiate
        char assemblyPath[] = "C:/Users/Giovane/Desktop/Workspace/ClassLibrary1/YourLibraryName.dll";
        MonoClass* testingClass = GetClassInAssembly(LoadCSharpAssembly(assemblyPath), "ClassLibrary1", "Class1");

        // Allocate an instance of our class
        MonoObject* classInstance = mono_object_new(s_AppDomain, testingClass);

        if (classInstance == nullptr)
        {
            // Log error here and abort
        }

        MonoObject* testInstance = InstantiateClass("", "CSharpTesting", LoadCSharpAssembly(assemblyPath), s_AppDomain);
        CallPrintFloatVarMethod(testInstance);

        // Call the parameterless (default) constructor
        mono_runtime_object_init(classInstance);
    }
}