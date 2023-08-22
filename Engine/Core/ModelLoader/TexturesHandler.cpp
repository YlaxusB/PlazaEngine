#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelLoader.h"
namespace Engine {
	vector<Texture> ModelLoader::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, vector<Texture>& textures_loaded, string* directory)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// Check if textures hasnt been already loaded
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str.C_Str()) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{   // Load a new texture
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), *directory, true);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // Add it to textures loaded to prevent other textures being loaded multiple times
			}
		}
		return textures;
	}

	unsigned int ModelLoader::TextureFromFile(const char* path, const string& directory, bool gamma)
	{
		string aed = std::filesystem::path{ directory }.parent_path().string();
		string texturesPath = std::filesystem::path{ directory }.string();
		string textureName = filesystem::path{ path }.filename().string();
		string fileName;
		if (filesystem::is_directory(texturesPath)) {
			fileName = texturesPath + "\\" + textureName;
		}
		else {
			fileName = filesystem::path{ texturesPath }.parent_path().string() + "\\" + textureName;
		}
		int width, height, nrComponents;
		std::cout << fileName << std::endl;
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 4);
		unsigned int textureID = 0;
		if (data)
		{
			glGenTextures(1, &textureID);

			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 2)
				format = GL_RG;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
			format = GL_SRGB8_ALPHA8;
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				std::cerr << "OpenGL error after glTexImage2D: " << error << std::endl;
			}
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			stbi_image_free(data);


		}
		else
		{
			std::cout << "Texture failed to load at path: " << fileName << std::endl;
			stbi_image_free(data);
		}
		return textureID;
	}

	bool ModelLoader::CheckFlippedTextures(const std::string& modelPath) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return false;
		}

		const aiMesh* mesh = scene->mMeshes[0];
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			const aiVector3D& textureCoord = mesh->mTextureCoords[0][i];
			if (textureCoord.x < 0.0f || textureCoord.x > 1.0f || textureCoord.y < 0.0f || textureCoord.y > 1.0f) {
				return true; // Texture coordinates outside the expected range
			}
		}
		return false; // Texture coordinates within the expected range
	}
}