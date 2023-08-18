#pragma once
namespace Engine::Editor {
	class DefaultModels {
	public:
		static vector<Mesh*> meshes;

		static void Init();

		static shared_ptr<Mesh> Cube();
		static shared_ptr<Mesh> Sphere();
		static shared_ptr<Mesh> Plane();
		static shared_ptr<Mesh> Cylinder();

	private:
		static uint64_t cubeUuid;
		static uint64_t sphereUuid;
		static uint64_t planeUuid;
		static uint64_t cylinderUuid;

		static void InitCube();
		static void InitSphere();
		static void InitPlane();
		static void InitCylinder();
	};
}