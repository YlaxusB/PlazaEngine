#pragma once
namespace Plaza {
	class Input::Cursor {
	public:
		static double lastX;
		static double lastY;

		static double deltaX;
		static double deltaY;
		
		static void Update();
	};
}