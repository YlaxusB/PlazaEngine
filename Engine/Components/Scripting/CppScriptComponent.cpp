#include "Engine/Core/PreCompiledHeaders.h"
#include "CppScriptComponent.h"
namespace Plaza {
	CsScriptComponent::CsScriptComponent(uint64_t uuid) {
		this->uuid = uuid;
	}
	void CsScriptComponent::Init(std::string csScriptPath) {
		this->scriptPath = csScriptPath;
		std::string dllPath = std::filesystem::path{ csScriptPath }.replace_extension(".dll").string();
		
		this->assembly = mono_domain_assembly_open(Mono::mAppDomain, dllPath.c_str());
		this->image = mono_assembly_get_image(assembly);
		const MonoTableInfo* tableInfo = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int rows = mono_table_info_get_rows(tableInfo);
		/* For each row, get some of its values */
		for (int i = 0; i < rows; i++)
		{
			MonoClass* klass = nullptr;
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			klass = mono_class_from_name(image, nameSpace, name);
			if (std::string(name) != "<Module>") {
				PlazaScriptClass* script = new PlazaScriptClass(klass, nameSpace, name, dllPath, this->uuid, Mono::mAppDomain);
				script->klass = mono_class_from_name(image, nameSpace, name);
				this->scriptClasses.emplace(name, script);
			}

		}
		//this->UpdateMethods();
		for (auto& [key, value] : scriptClasses) {
			value->onStartMethod = mono_class_get_method_from_name(mono_class_from_name(image, "", key.c_str()), "OnStart", 0);
			value->onUpdateMethod = mono_class_get_method_from_name(mono_class_from_name(image, "", key.c_str()), "OnUpdate", 0);
		}

		Application->activeProject->scripts[csScriptPath].entitiesUsingThisScript.emplace(this->uuid);
	}
	CsScriptComponent::~CsScriptComponent() {
		//Application->activeProject->scripts.at(scriptPath).entitiesUsingThisScript.erase(this->uuid);
	}
}