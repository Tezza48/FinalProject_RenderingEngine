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
		unsigned char idLength;
		unsigned char colorMapType;
		unsigned char imageTypeCode;
		unsigned short colorMapOrigin;
		unsigned short colorMapLength;
		unsigned char colorMapEntrySize;//bit depth: 24bit / 32bit for the color map
		unsigned short xOrigin;
		unsigned short yOrigin;
		unsigned short width;
		unsigned short height;
		unsigned char imagePixelSize;//24, 32, for the actual image
		unsigned char imageDescriptorByte;//should be set to 0 says paul bourke
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

	//Helpers
	XMFLOAT2 FbxToDxVec2(const FbxVector2 &other);// Convert an FbxVector2 to an XMFloat2
	XMFLOAT3 FbxToDxVec3(const FbxVector4 &other);// Convert an FbxVector4 to an XMFloat3 using it's 0, 1 and 2 indices.
	XMFLOAT4 FbxToDxVec4(const FbxVector4 &other);// Convert an FbxVector4 to an XMFloat4
};

