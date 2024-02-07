#pragma once 
namespace Plaza {
	class Shadows {
	public:
		virtual void Init() = 0;
		virtual void RenderToShadowMap() = 0;
		virtual void Terminate() = 0;

		std::vector<float> shadowCascadeLevels;
	};
}