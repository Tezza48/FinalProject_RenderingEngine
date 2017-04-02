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
	MeshData() { mName = ""; mNumVerts = 0; mNumTextures = 0; mTextureNames = nullptr; mTextureLocations = nullptr; }
	~MeshData() { delete[] mTextureNames; mTextureNames = nullptr; delete[] mTextureLocations; mTextureLocations = nullptr; }
	std::string mName;
	int mNumVerts;
	size_t mNumTextures;
	std::string *mTextureNames;
	const char **mTextureLocations;
};

void NavigateNodes(FbxNode *node);
void PrintTable(std::list<MeshData*> _meshData);

std::list<MeshData*> gMeshData;

int main()
{
	FbxManager *fbxManager = nullptr;
	FbxIOSettings *fbxIOS = nullptr;
	FbxImporter *fbxImporter = nullptr;

	gMeshData = std::list<MeshData*>();

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

		printf("Importing...\n\n");

		fbxImporter->Import(scene);

		FbxArray<FbxTexture*> sceneTextures;

		scene->FillTextureArray(sceneTextures);

		for (size_t i = 0; i < sceneTextures.GetCount(); i++)
		{
			printf("%s\n", sceneTextures[i]->GetName());
			printf("\n");
		}

		printf("File imported, Searching...\n\n");

		NavigateNodes(scene->GetRootNode());

		printf("Navigating finished.\n\n");

		PrintTable(gMeshData);

	}
	catch(std::exception e)
	{
		printf(e.what());
	}

	gMeshData.clear();

	fbxImporter->Destroy();
	fbxIOS->Destroy();
	fbxManager->Destroy();

	return 0;
}

void NavigateNodes(FbxNode *node)
{
	MeshData *data = new MeshData();

	size_t numChildren = node->GetChildCount();
	for (size_t i = 0; i < numChildren; i++)
	{
		NavigateNodes(node->GetChild(i));
	}

	size_t numMaterials = node->GetSrcObjectCount<FbxSurfaceMaterial>();
	
	for (size_t i = 0; i < numMaterials; i++)
	{
		FbxSurfaceMaterial *mat = node->GetSrcObject<FbxSurfaceMaterial>(i);
		if (!mat)
			continue;
	
		// get diffuse texture
		FbxProperty prop = mat->FindProperty(FbxSurfaceMaterial::sDiffuse);
		
		size_t layerCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
		if (layerCount > 0)
		{
	
		}
		else
		{
			data->mNumTextures = prop.GetSrcObjectCount<FbxTexture>();
			data->mTextureNames = new std::string[data->mNumTextures];
			data->mTextureLocations = new const char*[data->mNumTextures];
			for (size_t j = 0; j < data->mNumTextures; j++)
			{
				const FbxTexture *tex = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(j));
				data->mTextureNames[j] = tex->GetName();
				data->mTextureLocations[j] = tex->GetUrl().Buffer();
			}
		}

		gMeshData.push_back(data);
	
		// normal texture
	}

	FbxMesh *mesh = node->GetMesh();
	if (mesh)
	{
		data->mName = mesh->GetName();
		data->mNumVerts = mesh->GetControlPointsCount();
	}
	else
	{
		data->mName = "No Mesh";
		data->mNumVerts = NULL;
	}

}

void PrintTable(std::list<MeshData*> _meshData)
{
	printf("Printing\n\n");
	for each (MeshData *current in _meshData)
	{
		printf("Name: %s\n", current->mName.c_str());
		printf("Num Verts: %i\n", current->mNumVerts);
		printf("Textures\n");
		for (size_t i = 0; i < current->mNumTextures; i++)
		{
			printf("Texture %i name: %s\n", i, current->mTextureNames[i]);
			printf("Texture %i location: %s\n", i, current->mTextureLocations[i]);
		}
		printf("\n");
	}
}

