#pragma once
namespace Plaza::Editor {
	class Cache {
	public:
		static void Serialize(const std::string filePath);
		static void DeSerialize(const std::string filePath);

		static void Load();
	};
}