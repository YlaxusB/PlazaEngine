#pragma once
namespace Plaza {
	class ModelSerializer {
	public:
		static void SerializeModel(Entity* mainObject, string filePath, string modelFilePath);
		static Model* DeSerializeModel(string filePath);
		static uint64_t ReadUUID(string filePath);
	};
}