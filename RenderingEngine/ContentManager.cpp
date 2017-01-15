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

	//mFbxManager->Destroy();
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
	int polygonCount;// control point indices for each vertex
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
				int offset = currentPolygon * 3;
				FbxVector4 currentNormal;
				vertices[offset].position.x = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 0)][0];
				vertices[offset].position.y = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 0)][1];
				vertices[offset].position.z = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 0)][2];
				
				meshes[i]->GetPolygonVertexNormal(currentPolygon, 0, currentNormal);
				vertices[offset].normal.x = currentNormal[0];
				vertices[offset].normal.y = currentNormal[1];
				vertices[offset].normal.z = currentNormal[2];

				offset++;

				vertices[offset].position.x = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 1)][0];
				vertices[offset].position.y = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 1)][1];
				vertices[offset].position.z = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 1)][2];

				meshes[i]->GetPolygonVertexNormal(currentPolygon, 1, currentNormal);
				vertices[offset].normal.x = currentNormal[0];
				vertices[offset].normal.y = currentNormal[1];
				vertices[offset].normal.z = currentNormal[2];

				offset++;

				vertices[offset].position.x = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 2)][0];
				vertices[offset].position.y = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 2)][1];
				vertices[offset].position.z = controlPoints[meshes[i]->GetPolygonVertex(currentPolygon, 2)][2];

				meshes[i]->GetPolygonVertexNormal(currentPolygon, 2, currentNormal);
				vertices[offset].normal.x = currentNormal[0];
				vertices[offset].normal.y = currentNormal[1];
				vertices[offset].normal.z = currentNormal[2];
			}

			unsigned long *indices = new unsigned long[polygonCount * 3];

			for (size_t j = 0; j < polygonCount * 3; j++)
			{
				indices[j] = j;
			}

			//Initialize the new mesh
			output[i].Init(device, vertices, (unsigned long)polygonCount * 3, (unsigned long*)indices, (unsigned long)polygonCount * 3);

			output->GetWorldMatrix(xmPivot);

			xmPivot *= XMMatrixRotationRollPitchYaw(-90.0f, 0.0f, 0.0f);//rotate it so that y is up (because changing the export settings in max dosn't do anything

			output->SetWorldMatrix(xmPivot);

		}

	}
	
	//delete[] meshArray;
	//meshArray = nullptr;
	//well that was pretty stupid

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


