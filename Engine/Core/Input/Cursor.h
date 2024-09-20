#pragma once
namespace Plaza {
	class Input::Cursor {
	public:
		static bool show;

		static double lastX;
		static double lastY;

		static double deltaX;
		static double deltaY;
		
		static void Update();

		static void SetX(float value);
		static void SetY(float value);
	};
}