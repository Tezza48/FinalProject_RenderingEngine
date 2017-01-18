#include "ContentManager.h"

ContentManager::ContentManager()
{
	mFbxManager = FbxManager::Create();

	mFbxIOS = FbxIOSettings::Create(mFbxManager, IOSROOT);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_MATERIAL, false);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_TEXTURE, false);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_LINK, false);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_SHAPE, true);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_GOBO, false);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_ANIMATION, false);
	//mFBXIOSMeshOnly->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, false);

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

	FbxArray<FbxMesh*> meshes = GetAllMeshesReccursive(scene->GetRootNode());

	numMeshes = meshes.GetCount();
	Mesh *output;

	Mesh::Vertex *vertices;

	FbxVector4 *controlPoints;// vertex positions. dont need to know how many because polygon vertices does (kinda)
	size_t numControlPoints;//number of polygon vertices (the indices that point to control points)
	size_t polygonCount;// control point indices for each vertex
	//FbxArray<FbxVector4> polyVertNrms;//polygon vertex normals

	//Control points are all the vertex positions (they are most likely used multiple times)
	//Polygon vertices are the indices for each vertex's position (control point)
	//Polygon Vertex Normals are the the float4 normals for each polygon vertex
	
	XMMATRIX xmPivot;
	FbxAMatrix fbxPivot;// current mesh's pivot matrix

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
					vertices[currentPolygon * 3 + currentVertex].position.x = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, currentVertex)][0];
					vertices[currentPolygon * 3 + currentVertex].position.y = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, currentVertex)][1];
					vertices[currentPolygon * 3 + currentVertex].position.z = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, currentVertex)][2];
				
					meshes[i]->GetPolygonVertexNormal(currentPolygon, currentVertex, currentNormal);
					vertices[currentPolygon * 3 + currentVertex].normal.x = (float)currentNormal[0];
					vertices[currentPolygon * 3 + currentVertex].normal.y = (float)currentNormal[1];
					vertices[currentPolygon * 3 + currentVertex].normal.z = (float)currentNormal[2];

					meshes[i]->GetPolygonVertexUV(currentPolygon, currentVertex, uvName, currentUV, isMapped);
					vertices[currentPolygon * 3 + currentVertex].tex.x = (float)currentUV[0];
					vertices[currentPolygon * 3 + currentVertex].tex.y = (float)currentUV[1];
				}


				//offset++;

				//vertices[offset].position.x = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 1)][0];
				//vertices[offset].position.y = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 1)][1];
				//vertices[offset].position.z = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 1)][2];

				//meshes[i]->GetPolygonVertexNormal(currentPolygon, 1, currentNormal);
				//vertices[offset].normal.x = (float)currentNormal[0];
				//vertices[offset].normal.y = (float)currentNormal[1];
				//vertices[offset].normal.z = (float)currentNormal[2];

				//meshes[i]->GetPolygonVertexUV(offset, 0, uvName, currentUV, isMapped);
				//vertices[offset].tex.x = (float)currentUV[0];
				//vertices[offset].tex.y = (float)currentUV[1];

				//offset++;

				//vertices[offset].position.x = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 2)][0];
				//vertices[offset].position.y = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 2)][1];
				//vertices[offset].position.z = (float)controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 2)][2];

				//meshes[i]->GetPolygonVertexNormal(currentPolygon, 2, currentNormal);
				//vertices[offset].normal.x = (float)currentNormal[0];
				//vertices[offset].normal.y = (float)currentNormal[1];
				//vertices[offset].normal.z = (float)currentNormal[2];

				//meshes[i]->GetPolygonVertexUV(offset, 0, uvName, currentUV, isMapped);
				//vertices[offset].tex.x = (float)currentUV[0];
				//vertices[offset].tex.y = (float)currentUV[1];
				continue;
			}

			unsigned long *indices = new unsigned long[polygonCount * 3];

			for (size_t j = 0; j < polygonCount * 3; j++)
			{
				indices[j] = j;
			}

			//Initialize the new mesh
			output[i].Init(device, vertices, (unsigned long)polygonCount * 3, (unsigned long*)indices, (unsigned long)polygonCount * 3);

			//output->GetWorldMatrix(xmPivot);

			//xmPivot *= XMMatrixRotationRollPitchYaw(-90.0f, 0.0f, 0.0f);//rotate it so that y is up (because changing the export settings in max dosn't do anything

			//output->SetWorldMatrix(xmPivot);

		}

	}

	//delete[] polygonVertices;
	//polygonVertices = nullptr;

	//delete controlPoints;
	//controlPoints = nullptr;

	//delete[] vertices;
	//vertices = nullptr;

	//scene->Destroy(true);
	//scene = nullptr;
	
	fbxImporter->Destroy(true);
	fbxImporter = nullptr;

	return output;
}

Texture * ContentManager::LoadTGA(ID3D11Device *device, ID3D11DeviceContext *deviceContext, const std::string filename)
{
	// http://www.cplusplus.com/reference/istream/istream/
	Texture *output;
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
		header.width = buffer[13] << 8 | buffer[12];
		header.height = buffer[15] << 8 | buffer[14];
		header.imagePixelSize = buffer[16];
		header.imageDescriptorByte = buffer[17];

		if (header.imagePixelSize != 32)
		{
			throw std::exception("Targa was not 32bit");
		}

		// init new texture

		output = new Texture();

		char *imageData = &buffer[18 + header.idLength + header.colorMapLength];

		output->Init(device, deviceContext, imageData, header.width, header.height);

		delete[] buffer;
		buffer = nullptr;
		imageData = nullptr;
	}

	return output;
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


