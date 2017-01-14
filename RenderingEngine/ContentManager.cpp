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

Mesh *ContentManager::LoadFBX(ID3D11Device *device, const char * pFilename, size_t &numMeshes)
{
	FbxImporter *fbxImporter = FbxImporter::Create(mFbxManager, "");

	bool importStatus = fbxImporter->Initialize(pFilename, -1, mFbxManager->GetIOSettings());
	if (!importStatus)
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", fbxImporter->GetStatus().GetErrorString());
		throw std::exception();
	}

	FbxScene *scene = FbxScene::Create(mFbxManager, "myScene");

	fbxImporter->Import(scene);

	FbxArray<FbxMesh*> meshes = GetAllMeshes(scene->GetRootNode());

	Mesh *meshArray;

	numMeshes = meshes.GetCount();
	if (numMeshes > 0)
	{
		meshArray = new Mesh[numMeshes];

		// create meshes from the data in the fbx meshes
		unsigned long numVertices = NULL;
		unsigned long numIndices = NULL;

		Mesh::Vertex *vertices = nullptr;
		unsigned long *indices = nullptr;

		FbxArray<FbxVector4> *normals;

		for (size_t i = 0; i < numMeshes; i++)
		{
			meshArray[i] = Mesh();

			/*
				control point is a vertex synonym
				polygon vertex is an index to a control point on a polygon
				
			*/

			numVertices = meshes[i]->GetControlPointsCount();// checks out
			numIndices = meshes[i]->GetPolygonVertexCount();// = number of polys * 3

			vertices = new Mesh::Vertex[numVertices];

			indices = new unsigned long[numIndices];

			for (size_t vi = 0; vi < numVertices; vi++)// vi == vertex index
			{
				vertices[vi].position = XMFLOAT3((float)meshes[i]->GetControlPointAt(vi)[0], (float)meshes[i]->GetControlPointAt(vi)[1], (float)meshes[i]->GetControlPointAt(vi)[2]);
				XMVECTOR normal = XMLoadFloat3(&vertices[vi].position);
				normal = -XMVector4Normalize(normal);
				XMStoreFloat3(&vertices[vi].normal, normal);
			}

			for (size_t ii = 0; ii < numIndices; ii++)// ii == index index? good job, me.
			{
				indices[ii] = meshes[i]->GetPolygonVertices()[ii];
			}
			meshArray[i].Init(device, vertices, numVertices, indices, numIndices);
		}

		delete[] vertices;
		vertices = nullptr;
		delete[] indices;
		indices = nullptr;
	}

	fbxImporter->Destroy();

	scene->Destroy();

	return meshArray;
}

FbxArray<FbxMesh*> ContentManager::GetAllMeshes(FbxNode *node)
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
			meshes.AddArray(GetAllMeshes(node->GetChild(i)));
		}
	}

	return meshes;
}


