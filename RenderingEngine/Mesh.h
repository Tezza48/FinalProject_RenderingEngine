#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <fbxsdk.h>
#include "FilePathMacros.h"
#include "Material.h"
#include "utils.h"

using namespace DirectX;

class Mesh
{
public:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 tex;// Texture UV co-ord
	};

	enum PrimativeShape
	{
		MESH_TRIANGLE,
		//MESH_SQUARE,
		MESH_CUBE,
		MESH_SOFTCUBE,
		//MESH_NONE
	};

public:
	Mesh();
	~Mesh();

	//bool Init(ID3D11Device*);
	bool Init(ID3D11Device *device, PrimativeShape shape);
	bool Init(ID3D11Device *device, Vertex *vertices, unsigned long numVertices, unsigned long *indices, unsigned long numIndices);
	void Render(ID3D11DeviceContext *deviceContext);

	int GetIndexCount();
	void XM_CALLCONV GetWorldMatrix(XMMATRIX &other);
	void XM_CALLCONV SetWorldMatrix(XMMATRIX world);
	//void SetMaterial(ColorMaterial *material);
	//ColorMaterial *GetMaterial();

private:
	//bool InitBuffers(ID3D11Device*);
	bool InitBuffers(ID3D11Device *device, Vertex *vertices, unsigned long numVertices, unsigned long *indices, unsigned long numIndices);
	void RenderBuffers(ID3D11DeviceContext *deviceContext);

private:
	XMMATRIX mWorld;
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
	int mVertexCount, mIndexCount;
	//ColorMaterial *mMaterial;
};