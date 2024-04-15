#pragma once
namespace Plaza {
	class Input {
	public:
		static inline bool isAnyKeyPressed = false;
		class Cursor;
		static void Update();
	};
}