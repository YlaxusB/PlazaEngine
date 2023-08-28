#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"

namespace Plaza::Editor {
	void ScriptManager::ReloadScriptsAssembly() {
		const auto& scriptsMap = ScriptManagerSerializer::DeSerialize(Application->activeProject->scriptsConfigFilePath);
		for (auto& [key, value] : scriptsMap) {
            const char* csFilePath = key.c_str();
            std::filesystem::path dllPath = std::filesystem::path{ csFilePath };
            std::string compileCommand = "mcs -target:library -out:" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll " + "\"" + std::string(csFilePath) + "\"";
            compileCommand += " -reference:" + Application->enginePath + "\\..\\x64\\Debug\\" + "PlazaScriptCore.dll";
            std::cout << compileCommand << std::endl;
            int result = system(compileCommand.c_str());
            if (result == 0) {
                // Compilation success
                // You might want to handle loading the new DLL here
            }
            else {
                // Compilation failed
                // Handle error
            }
		}
	}
}