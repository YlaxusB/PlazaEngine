#pragma once
namespace Engine {
	class ModelSerializer {
	public:
		static void SerializeModel(GameObject* mainObject, string filePath, string modelFilePath);
		static Model* DeSerializeModel(string filePath);
		static uint64_t ReadUUID(string filePath);
	};
}