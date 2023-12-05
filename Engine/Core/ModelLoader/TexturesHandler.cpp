#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelLoader.h"
#include <ddraw.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
namespace Plaza {
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
				texture.path = ".\\" + std::filesystem::path(str.C_Str()).filename().string();
				textures.push_back(texture);
				texture.path = str.C_Str();
				textures_loaded.push_back(texture); // Add it to textures loaded to prevent other textures being loaded multiple times
			}
		}
		if (textures.size() == 0)
			textures.push_back(Texture(typeName));
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

	GLuint texture_loadDDS(const char* path);
	unsigned int ModelLoader::TextureFromFile(std::string filePath, bool gamma) {
		int width, height, nrComponents;
		unsigned int textureID = 0;
		if (std::filesystem::path{ filePath }.extension().string() == ".dds") {
			textureID = texture_loadDDS(filePath.c_str());
		}
		else {
			unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 4);
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
				std::cout << "Texture failed to load at path: " << filePath << std::endl;
				stbi_image_free(data);
			}
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

	typedef unsigned int DWORD;
	GLuint texture_loadDDS(const char* path) {
		// lay out variables to be used
		unsigned char* header;

		unsigned int width;
		unsigned int height;
		unsigned int mipMapCount;

		unsigned int blockSize;
		unsigned int format;

		unsigned int w;
		unsigned int h;

		unsigned char* buffer = nullptr;

		GLuint tid = 0;

		// open the DDS file for binary reading and get file size
		FILE* f;
		if (fopen_s(&f, path, "rb") != 0 || f == nullptr)
			return 0;

		fseek(f, 0, SEEK_END);
		long file_size = ftell(f);
		fseek(f, 0, SEEK_SET);

		// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
		// read in 128 bytes from the file
		header = static_cast<unsigned char*>(malloc(128));
		fread(header, 1, 128, f);

		// compare the `DDS ` signature
		if (memcmp(header, "DDS ", 4) != 0)
			return 0;

		// extract height, width, and amount of mipmaps - yes it is stored height then width
		height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
		width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
		mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

		// figure out what format to use for what fourCC file type it is
		// block size is about physical chunk storage of compressed data in file (important)
		if (header[84] == 'D') {
			switch (header[87]) {
			case '1': // DXT1
				format = GL_COMPRESSED_RGBA;
				blockSize = 8;
				break;
			case '3': // DXT3
				format = GL_COMPRESSED_RGBA;
				blockSize = 16;
				break;
			case '5': // DXT5
				format = GL_COMPRESSED_RGBA;
				blockSize = 16;
				break;
			case '0': // DX10
				// unsupported, else will error
				// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
				// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			default:
				return 0;
			}
		}
		else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
			return 0;

		// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
		// read rest of file
		buffer = static_cast<unsigned char*>(malloc(file_size - 128));
		if (buffer == nullptr)
			return 0;
		fread(buffer, 1, file_size, f);

		// prepare new incomplete texture
		glGenTextures(1, &tid);
		if (tid == 0)
			return 0;

		// bind the texture
		// make it complete by specifying all needed parameters and ensuring all mipmaps are filled
		glBindTexture(GL_TEXTURE_2D, tid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// prepare some variables
		unsigned int offset = 0;
		unsigned int size = 0;
		w = width;
		h = height;

		// loop through sending block at a time with the magic formula
		// upload to opengl properly, note the offset transverses the pointer
		// assumes each mipmap is 1/2 the size of the previous mipmap
		for (unsigned int i = 0; i < mipMapCount; i++) {
			if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
				mipMapCount--;
				continue;
			}
			size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
			offset += size;
			w /= 2;
			h /= 2;
		}
		// discard any odd mipmaps, ensure a complete texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);

		// easy macro to get out quick and uniform (minus like 15 lines of bulk)
	exit:
		free(buffer);
		free(header);
		fclose(f);
		return tid;
	}
}