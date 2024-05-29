#pragma once

namespace Plaza {
	class Lighting {
	public:
		virtual void Init() = 0;
		virtual void UpdateTiles() = 0;
		virtual void Terminate() = 0;
	};
}