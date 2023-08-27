#pragma once
#include "../Component.h"
#include "Engine/Components/Component.h"
namespace Plaza {
	class CppScriptComponent : public  Component {
	public:
		virtual void OnStart() {};
		virtual void OnReload() {};
		virtual void OnUpdate() {};
		virtual void OnDestroy() {};
	};
}