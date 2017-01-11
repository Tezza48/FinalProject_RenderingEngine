#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
//#include "Material.h"
#include "ColorMaterial.h"
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
	Mesh(const Mesh&);
	~Mesh();

	//bool Init(ID3D11Device*);
	bool Init(ID3D11Device*, PrimativeShape);
	bool Init(ID3D11Device*, Vertex*, unsigned long, unsigned long*, unsigned long);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void XM_CALLCONV GetWorldMatrix(XMMATRIX&);
	void XM_CALLCONV SetWorldMatrix(XMMATRIX);
	void SetMaterial(ColorMaterial *material);
	//ColorMaterial *GetMaterial();

private:
	//bool InitBuffers(ID3D11Device*);
	bool InitBuffers(ID3D11Device*, Vertex*, unsigned long, unsigned long*, unsigned long);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	XMMATRIX mWorld;
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
	int mVertexCount, mIndexCount;
	//ColorMaterial *mMaterial;
};