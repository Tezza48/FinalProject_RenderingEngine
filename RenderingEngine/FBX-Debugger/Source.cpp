#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <exception>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

#pragma comment(lib, "libfbxsdk.lib")

struct MeshData
{
	std::string mName;
	int mNumVerts;

};

void NavigateNodes(FbxNode *node);
void PrintTable(std::list<MeshData> _meshData);

static std::list<MeshData> gMeshData;

int main()
{
	FbxManager *fbxManager = nullptr;
	FbxIOSettings *fbxIOS = nullptr;
	FbxImporter *fbxImporter = nullptr;

	std::string filename = "../res/fbx/sponza.fbx";

	try
	{
		fbxManager = FbxManager::Create();
		fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(fbxIOS);
		fbxImporter = FbxImporter::Create(fbxManager, "");

		bool importStatus = fbxImporter->Initialize(filename.c_str(), -1, fbxManager->GetIOSettings());
		if (!importStatus)
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", fbxImporter->GetStatus().GetErrorString());
			throw std::exception();
		}

		FbxScene *scene = FbxScene::Create(fbxManager, "myScene");

		fbxImporter->Import(scene);

		NavigateNodes(scene->GetRootNode());

		PrintTable(gMeshData);

	}
	catch(std::exception e)
	{
		printf(e.what());
	}

	std::string n;
	std::cin >> n;

	fbxImporter->Destroy();
	fbxIOS->Destroy();
	fbxManager->Destroy();

	return 0;
}

void NavigateNodes(FbxNode *node)
{
	size_t numChildren = node->GetChildCount();
	for (size_t i = 0; i < numChildren; i++)
	{
		NavigateNodes(node->GetChild(i));
	}

	// Get the textures too

	FbxMesh *mesh = node->GetMesh();
	if (mesh)
	{
		::gMeshData.push_back(MeshData({ 
			mesh->GetName(), 
			mesh->GetControlPointsCount(),
		}));
	}

}

void PrintTable(std::list<MeshData> _meshData)
{
	for each (MeshData current in _meshData)
	{
		printf("Name: %s\n", current.mName.c_str());
		printf("Num Verts: %i\n", current.mNumVerts);
		printf("\n");
	}
}

