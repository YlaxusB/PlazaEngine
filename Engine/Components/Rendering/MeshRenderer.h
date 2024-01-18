#pragma once
#include "Engine/Components/Component.h"
//#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Core/RenderGroup.h"
#include "Engine/Components/Core/Transform.h"
using namespace std;
namespace Plaza {
	template<typename Type>
	class Array2D {
	public:
		Array2D() {}
		int m_cols;
		int m_rows;
		float* m_p;
		void InitArray2D(int Cols, int Rows, void* pData)
		{
			m_cols = Cols;
			m_rows = Rows;

			m_p = (float*)pData;
		}
		float* GetAddr(int Col, int Row) const
		{
			if (Col > m_cols || Row > m_rows)
				return new float(0.0f);
			size_t Index = CalcIndex(Col, Row);

			return &m_p[Index];
		}
		float Get(int col, int row) const {
			return *GetAddr(col, row);
		}
		size_t CalcIndex(int Col, int Row) const
		{
			size_t Index = Row * m_cols + Col;

			return Index;
		}
	};
	class MeshRenderer : public Component {
	public:
		bool castShadows = true;
		string aiMeshName;
		//uint64_t uuid;
		std::string meshName;
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		std::shared_ptr<RenderGroup> renderGroup = nullptr;
		GLuint heightMap;

		Array2D<float> m_heightMap;

		float GetHeight(float x, float y) const {
			return m_heightMap.Get((int)x, (int)y);
		}

		char* ReadBinaryFile(const char* pFilename, int& size)
		{
			FILE* f = NULL;

			errno_t err = fopen_s(&f, pFilename, "rb");

			if (!f) {
				char buf[256] = { 0 };
				strerror_s(buf, sizeof(buf), err);
				exit(0);
			}

			struct stat stat_buf;
			int error = stat(pFilename, &stat_buf);

			if (error) {
				char buf[256] = { 0 };
				strerror_s(buf, sizeof(buf), err);
				return NULL;
			}

			size = stat_buf.st_size;

			char* p = (char*)malloc(size);
			assert(p);

			size_t bytes_read = fread(p, 1, size, f);

			if (bytes_read != size) {
				char buf[256] = { 0 };
				strerror_s(buf, sizeof(buf), err);
				exit(0);
			}

			fclose(f);

			return p;
		}

		void LoadHeightMap(std::string filePath) {
			int FileSize = 0;
			unsigned char* p = (unsigned char*)ReadBinaryFile(filePath.c_str(), FileSize);

			if (FileSize % sizeof(float) != 0) {
				printf("%s:%d - '%s' does not contain an whole number of floats (size %d)\n", __FILE__, __LINE__, filePath.c_str(), FileSize);
				//exit(0);
			}

			int m_terrainSize = (int)sqrtf((float)FileSize / (float)sizeof(float));

			printf("Terrain size %d\n", m_terrainSize);

			if ((m_terrainSize * m_terrainSize) != (FileSize / sizeof(float))) {
				printf("%s:%d - '%s' does not contain a square height map - size %d\n", __FILE__, __LINE__, filePath.c_str(), FileSize);
				//exit(0);
			}

			m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, (float*)p);
		}
		Transform* transform = nullptr;
		bool instanced = false;
		MeshRenderer(Mesh& initialMesh, bool addToScene = false);
		MeshRenderer(Plaza::Mesh& initialMesh, Material material, bool addToScene = false);
		MeshRenderer(const MeshRenderer& other) = default;
		~MeshRenderer();
		MeshRenderer() {
			uuid = Plaza::UUID::NewUUID();
		}
	};
}