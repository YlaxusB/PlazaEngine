#pragma once
namespace Engine {
	class Serializer {
	public:
		static void Serialize(const std::string filePath);
		static void DeSerialize();
	};
}