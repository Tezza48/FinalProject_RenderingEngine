#pragma once

#include <D3D11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Mesh
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
public:
	Mesh();
	Mesh(const Mesh&);
	~Mesh();

	bool Init(ID3D11Device*);
	bool Init(ID3D11Device*, VertexType*, unsigned long, unsigned long*, unsigned long);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void XM_CALLCONV GetWorldMatrix(XMMATRIX&);
	void XM_CALLCONV SetWorldMatrix(XMMATRIX);

private:
	bool InitBuffers(ID3D11Device*);
	bool InitBuffers(ID3D11Device*, VertexType*, unsigned long, unsigned long*, unsigned long);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	XMMATRIX mWorld;
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
	int mVertexCount, mIndexCount;
};