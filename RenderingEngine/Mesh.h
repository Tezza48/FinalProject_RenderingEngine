#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

using namespace DirectX;

class Mesh
{
public:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 color;
	};

	enum PrimativeShape
	{
		MESH_TRIANGLE,
		//MESH_SQUARE,
		MESH_CUBE,
		//MESH_NONE
	};

public:
	Mesh();
	Mesh(const Mesh&);
	~Mesh();

	bool Init(ID3D11Device*);
	bool Init(ID3D11Device*, PrimativeShape);
	bool Init(ID3D11Device*, Vertex*, unsigned long, unsigned long*, unsigned long);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void XM_CALLCONV GetWorldMatrix(XMMATRIX&);
	void XM_CALLCONV SetWorldMatrix(XMMATRIX);

private:
	bool InitBuffers(ID3D11Device*);
	bool InitBuffers(ID3D11Device*, Vertex*, unsigned long, unsigned long*, unsigned long);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	XMMATRIX mWorld;
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
	int mVertexCount, mIndexCount;
};