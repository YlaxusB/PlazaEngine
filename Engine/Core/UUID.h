#pragma once
#include <xhash>
#include "Engine/Core/Engine.h"

namespace Plaza {
	class PLAZA_API UUID {
	public:
		static uint64_t NewUUID();
	};
}