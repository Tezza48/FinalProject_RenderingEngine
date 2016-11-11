#pragma once

#include <D3D11.h>
#include <DirectXMath.h>

using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Init(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
	int mVertexCount, mIndexCount;
};