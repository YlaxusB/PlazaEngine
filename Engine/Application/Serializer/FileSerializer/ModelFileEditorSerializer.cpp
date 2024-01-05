#include "Engine/Core/PreCompiledHeaders.h"
#include "FileSerializer.h"
#include "Engine/Application/Serializer/Components/TextureSerializer.h"
namespace Plaza {
	void ModelFileSerializer::Serialize(const std::string filePath, Model* model) {
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());


		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Model" << YAML::Value << model->modelName;
		out << YAML::Key << "Uuid" << YAML::Value << model->uuid;

		std::string objectPath = model->modelObjectPath;
		size_t found = objectPath.find(filePath);
		if (found != std::string::npos) {
			objectPath.erase(found, filePath.length());
			objectPath = ".\\" + objectPath;
		}
		out << YAML::Key << "ObjectRelativePath" << YAML::Value << objectPath;
		//ComponentSerializer::TextureSerializer::Serialize(out, material->diffuse);
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	Model* ModelFileSerializer::DeSerialize(const std::string filePath) {
		return nullptr;
	}
}