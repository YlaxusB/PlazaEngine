#pragma once
namespace Engine::Editor {
	class DefaultModels {
	public:
		static vector<Mesh*> meshes;

		static void Init();

		static shared_ptr<Mesh> Cube();
		static shared_ptr<Mesh> Sphere();
		static shared_ptr<Mesh> Plane();

	private:
		static void InitCube();
		static void InitSphere();
		static void InitPlane();
	};
}