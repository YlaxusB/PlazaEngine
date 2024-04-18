#pragma once
#ifdef EDITOR_MODE
#include <ThirdParty/Tracy/public/tracy/Tracy.hpp>
//#include "ThirdParty/Tracy/tracy/Tracy.hpp"
#define PLAZA_PROFILE ZoneScoped
#define PLAZA_PROFILE_FRAME(x) FrameMark
#define PLAZA_PROFILE_SECTION(x) static constexpr tracy::SourceLocationData __tracy_source_location385{ x, __FUNCTION__, "", (uint32_t)385, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location385, true)
#else
#define PLAZA_PROFILE 
#define PLAZA_PROFILE_FRAME(x) 
#define PLAZA_PROFILE_SECTION(x)
#endif

/*

	static constexpr tracy::SourceLocationData __tracy_source_location385{ "a", __FUNCTION__, "C:\\Users\\Giovane\\Desktop\\Workspace\\Plaza\\Engine\\Application\\Application.cpp", (uint32_t)385, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location385, true)

*/