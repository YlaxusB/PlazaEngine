#pragma once
namespace Engine {
	class ModelSerializer {
	public:
		static void SerializeModel(GameObject* mainObject, string filePath);
		static void DeSerializeModel(string filePath);
	};
}