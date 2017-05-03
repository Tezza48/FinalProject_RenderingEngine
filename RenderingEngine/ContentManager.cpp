#include "ContentManager.h"

ContentManager::ContentManager()
{
	mFbxManager = FbxManager::Create();

	mFbxIOS = FbxIOSettings::Create(mFbxManager, IOSROOT);

	mFbxManager->SetIOSettings(mFbxIOS);
}


ContentManager::~ContentManager()
{
	mFbxIOS->Destroy();

	mFbxManager->Destroy();
}

Mesh *ContentManager::LoadFBX(ID3D11Device *device, std::string pFilename, size_t &numMeshes)
{
	FbxImporter *fbxImporter = FbxImporter::Create(mFbxManager, "");

	bool importStatus = fbxImporter->Initialize(pFilename.c_str(), -1, mFbxManager->GetIOSettings());
	if (!importStatus)
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", fbxImporter->GetStatus().GetErrorString());
		throw std::exception();
	}

	FbxScene *scene = FbxScene::Create(mFbxManager, "myScene");

	fbxImporter->Import(scene);

	FbxAxisSystem::DirectX.ConvertScene(scene);

	FbxArray<FbxMesh*> meshes = GetAllMeshesReccursive(scene->GetRootNode());

	numMeshes = meshes.GetCount();
	Mesh *output;

	Mesh::Vertex *vertices;

	FbxVector4 *controlPoints;
	size_t numControlPoints;
	size_t polygonCount;

	//Control points are all the vertex positions (they are most likely used multiple times)
	//Polygon vertices are the indices for each vertex's position (control point)
	//Polygon Vertex Normals are the the float4 normals for each polygon vertex

	if (numMeshes > 0)
	{
		output = new Mesh[numMeshes];

		for (size_t i = 0; i < numMeshes; i++)
		{
			output[i] = Mesh();

			controlPoints = meshes[i]->GetControlPoints();
			numControlPoints = meshes[i]->GetControlPointsCount();
			polygonCount = meshes[i]->GetPolygonCount();

			vertices = new Mesh::Vertex[polygonCount * 3];

			for (size_t currentPolygon = 0; currentPolygon < polygonCount; currentPolygon++)
			{
				FbxVector4 currentNormal;

				FbxStringList uvNames;
				meshes[i]->GetUVSetNames(uvNames);
				const char *uvName;
				uvName = uvNames[0];
				FbxVector2 currentUV;
				bool isMapped;

				for (size_t currentVertex = 0; currentVertex < 3; currentVertex++)
				{
					vertices[currentPolygon * 3 + currentVertex].position = FbxToDxVec3(controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, currentVertex)]);
				
					meshes[i]->GetPolygonVertexNormal(currentPolygon, currentVertex, currentNormal);
					vertices[currentPolygon * 3 + currentVertex].normal = FbxToDxVec3(currentNormal);

					meshes[i]->GetPolygonVertexUV(currentPolygon, currentVertex, uvName, currentUV, isMapped);
					vertices[currentPolygon * 3 + currentVertex].tex = FbxToDxVec2(currentUV);
				}
			}

			unsigned long *indices = new unsigned long[polygonCount * 3];

			for (size_t j = 0; j < polygonCount * 3; j++)
			{
				indices[j] = j;
			}

			//Initialize the new mesh
			output[i].Init(device, vertices, (unsigned long)polygonCount * 3, (unsigned long*)indices, (unsigned long)polygonCount * 3);
		}
	}
	fbxImporter->Destroy(true);
	fbxImporter = nullptr;

	return output;
}

void ContentManager::LoadTGA(ID3D11Device *device, ID3D11DeviceContext *deviceContext, const std::string filename, Texture &output)
{
	// http://www.cplusplus.com/reference/istream/istream/
	std::ifstream is;
	is.open(filename, std::fstream::binary);

	if (is)
	{
		is.seekg(0, is.end);
		int length = (int)is.tellg();
		is.seekg(0, is.beg);

		char *buffer = new char[length];

		is.read(buffer, length);

		is.close();

		TargaHeader header;

		header.idLength = buffer[0];
		header.colorMapType = buffer[1];
		header.imageTypeCode = buffer[2];
		header.colorMapOrigin = buffer[4] << 8 | buffer[3];
		header.colorMapLength = buffer[6] << 8 | buffer[5];
		header.colorMapEntrySize = buffer[7];
		header.xOrigin = buffer[9] << 8 | buffer[8];
		header.yOrigin = buffer[11] << 8 | buffer[10];

		unsigned char widthLo = buffer[12];
		unsigned char widthHi = buffer[13];

		unsigned char heightLo = buffer[14];
		unsigned char heightHi = buffer[15];

		header.width = widthHi << 8 | widthLo;
		header.height = heightHi << 8 | heightLo;
		header.imagePixelSize = buffer[16];
		header.imageDescriptorByte = buffer[17]; // 00000000

		char *rawImageData = &buffer[18 + header.idLength + header.colorMapLength];
		char *finalImageData = new char[header.width * header.height * 4];

		switch (header.imagePixelSize)
		{
		case 24:
			for (size_t texel24 = 0, texel32 = 0; texel24 < header.width * header.height * (unsigned)3; texel24+=3, texel32+=4)
			{
				finalImageData[texel32 + 0] = rawImageData[texel24 + 2];
				finalImageData[texel32 + 1] = rawImageData[texel24 + 1];
				finalImageData[texel32 + 2] = rawImageData[texel24 + 0];
				finalImageData[texel32 + 3] = (char)255;
			}
			break;
		case 32:
			// convert from BGR to RGB
			for (size_t texel = 0; texel < (size_t)header.width * (size_t)header.height * 4; texel+=4)
			{
				finalImageData[texel + 0] = rawImageData[texel + 2];
				finalImageData[texel + 1] = rawImageData[texel + 1];
				finalImageData[texel + 2] = rawImageData[texel + 0];
				finalImageData[texel + 3] = rawImageData[texel + 3];
			}
			break;
		default:
			throw std::exception("TARGA has invalid bit depth: " + header.imagePixelSize);
			break;
		}


		output.Init(device, deviceContext, finalImageData, header.width, header.height);

		delete[] buffer;
		buffer = nullptr;
		rawImageData = nullptr; // imageData is part of buffer so it's already deleted.
		delete[] finalImageData;
		finalImageData = nullptr;
	}
	else
	{
		throw std::exception();
	}
}

FbxArray<FbxMesh*> ContentManager::GetAllMeshesReccursive(FbxNode *node)
{
	size_t i;
	FbxArray<FbxMesh*> meshes = FbxArray<FbxMesh*>();

	// Call GetMesh; if tyhere's no mesh it'll return null
	FbxMesh *currentMesh = node->GetMesh();

	if (currentMesh)
	{
		meshes.Add(currentMesh);
	}

	//has this node got any children
	size_t count = node->GetChildCount();
	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			meshes.AddArray(GetAllMeshesReccursive(node->GetChild(i)));
		}
	}

	return meshes;
}

XMFLOAT2 ContentManager::FbxToDxVec2(const FbxVector2 & other)
{
	return XMFLOAT2((float)other[0], (float)other[1]);
}

XMFLOAT3 ContentManager::FbxToDxVec3(const FbxVector4 & other)
{
	return XMFLOAT3((float)other[0], (float)other[1], (float)other[2]);
}

XMFLOAT4 ContentManager::FbxToDxVec4(const FbxVector4 & other)
{
	return XMFLOAT4((float)other[0], (float)other[1], (float)other[2], (float)other[3]);
}



