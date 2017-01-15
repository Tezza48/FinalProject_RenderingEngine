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

	FbxArray<FbxMesh*> meshes = GetAllMeshes(scene->GetRootNode());

	numMeshes = meshes.GetCount();
	Mesh *output;

	Mesh::Vertex *vertices;

	FbxVector4 *controlPoints;// vertex positions. dont need to know how many because polygon vertices does (kinda)
	size_t numPolyVerts;//number of polygon vertices (the indices that point to control points)
	int *polygonVertices;// control point indices for each vertex
	FbxArray<FbxVector4> polyVertNrms;//polygon vertex normals

	//Control points are all the vertex positions (they are most likely used multiple times)
	//Polygon vertices are the indices for each vertex's position (control point)
	//Polygon Vertex Normals are the the float4 normals for each polygon vertex

	if (numMeshes > 0)
	{
		output = new Mesh[numMeshes];

		for (size_t i = 0; i < numMeshes; i++)
		{
			output[i] = Mesh();

			//fill controlPoints
			controlPoints = meshes[i]->GetControlPoints();
			
			//fill polygonVertices
			numPolyVerts = meshes[i]->GetPolygonVertexCount();
			polygonVertices = meshes[i]->GetPolygonVertices();

			//fill polyVertNrm
			meshes[i]->GetPolygonVertexNormals(polyVertNrms);

			//TODO: fill polygon vertex UVs

			vertices = new Mesh::Vertex[numPolyVerts];
			FbxVector4 currentVec;//temporary variable for any FbxVector4s we need
			//Create the Vertex*, dont need to do indices as polygonVertices is already here.
			for (size_t j = 0; j < numPolyVerts; j++)
			{
				currentVec = controlPoints[polygonVertices[j]];
				vertices[j].position = XMFLOAT3((float)currentVec[0], (float)currentVec[1], (float)currentVec[2]);
				currentVec = polyVertNrms[j];
				vertices[j].normal = XMFLOAT3((float)currentVec[0], (float)currentVec[1], (float)currentVec[2]);
				//TODO: UVs
			}

			//Initialize the new mesh
			output[i].Init(device, vertices, (unsigned long)numPolyVerts, (unsigned long*)polygonVertices, (unsigned long)numPolyVerts);
		}

	}
	
	//delete[] meshArray;
	//meshArray = nullptr;
	//well that was pretty stupid

	delete[] polygonVertices;
	polygonVertices = nullptr;

	delete controlPoints;
	controlPoints = nullptr;

	delete[] vertices;
	vertices = nullptr;

	//scene->Destroy(true);
	//scene = nullptr;
	
	fbxImporter->Destroy(true);
	fbxImporter = nullptr;

	return output;
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


