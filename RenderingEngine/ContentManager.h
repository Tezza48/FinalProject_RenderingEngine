#pragma once

//#include <list>
#include <string>
#include <fstream>
#include <D3D11.h>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "FilePathMacros.h"
#include "Mesh.h"
#include "Texture.h"


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
	struct TargaHeader
	{
		TargaHeader() { ZeroMemory(this, sizeof(this)); }
		char idLength;
		char colorMapType;
		char imageTypeCode;
		short colorMapOrigin;
		short colorMapLength;
		char colorMapEntrySize;//bit depth: 24bit / 32bit for the color map
		short xOrigin;
		short yOrigin;
		short width;
		short height;
		char imagePixelSize;//24, 32, for the actual image
		char imageDescriptorByte;//should be set to 0 says paul bourke
	};
private:
	FbxManager *mFbxManager;

	//FBX importer settings
	FbxIOSettings *mFbxIOS;// settings to only import the mesh

public:
	ContentManager();
	~ContentManager();

	Mesh *LoadFBX(ID3D11Device *device, const std::string filename, size_t &numMeshes);
	Texture *LoadTGA(ID3D11Device *device, ID3D11DeviceContext *deviceContext, const std::string filename);
private:
	FbxArray<FbxMesh*> GetAllMeshesReccursive(FbxNode *node);
};

