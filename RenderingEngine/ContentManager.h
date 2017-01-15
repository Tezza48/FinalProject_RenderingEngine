#pragma once

//#include <list>
#include <string>
#include <D3D11.h>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "FilePathMacros.h"
#include "Mesh.h"


#pragma comment(lib, "libfbxsdk.lib")
/*

	FBX stuff from FBX SDK documentation examples.

*/


// Class responsible for loading ecternal files into the engine
// Currently supports:
//		FBX
//			Mesh
// Will Support:
//		TGA
class ContentManager
{
private:
	FbxManager *mFbxManager;

	//FBX importer settings
	FbxIOSettings *mFbxIOS;// settings to only import the mesh

public:
	ContentManager();
	~ContentManager();

	Mesh *LoadFBX(ID3D11Device *device, const std::string filename, size_t &numMeshes);

private:
	FbxArray<FbxMesh*> GetAllMeshes(FbxNode *node);
};

