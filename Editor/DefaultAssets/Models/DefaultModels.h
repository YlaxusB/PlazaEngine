#pragma once
namespace Engine::Editor {
	class DefaultModels {
	public:
		static vector<Mesh*> meshes;

		static void Init();

		static shared_ptr<Mesh> Cube();

	private:
		static void InitCube();
	};
}