#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManager.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
namespace Plaza::Editor {
	void ScriptManager::NewCsScript(std::string fullPath) {
		std::ofstream file = Utils::Filesystem::GetFileStream(fullPath);
		if (file.is_open()) {
			std::string className = std::filesystem::path{ fullPath }.stem().string();
			std::string csCode = R"(
using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Plaza;
using static Plaza.InternalCalls;
using static Plaza.Input;

public class )" + className + R"( : Entity
{
    public void OnStart()
    {

    }

    public void OnUpdate()
    {

    }

    public void OnRestart()
    {

    }
}
)";
			file << csCode;
			file.close();
		}

		Script script = Script();
		script.lastModifiedDate = std::chrono::system_clock::now();
		Application->activeProject->scripts.emplace(fullPath, script);
		std::map<std::string, Script> map = Application->activeProject->scripts;
		ScriptManagerSerializer::Serialize(Application->activeProject->directory + "\\Scripts" + Standards::scriptConfigExtName, map);
	}
}

/*
using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Plaza;
using static Plaza.InternalCalls;
using static Plaza.Input;

public class Unnamed : Entity
{
	public void OnStart()
	{

	}

	public void OnUpdate()
	{

	}

	public void OnRestart()
	{

	}
}

*/