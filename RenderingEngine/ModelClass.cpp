#include "ModelClass.h"

ModelClass::ModelClass()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;
}

ModelClass::ModelClass(const ModelClass &)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Init(ID3D11Device *device)
{
	bool result;

	result = InitBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext *deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return mIndexCount;
}

bool ModelClass::InitBuffers(ID3D11Device *device)
{
	HRESULT hr;
	VertexType *vertices;
	unsigned long *indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	mVertexCount = 3;
	mIndexCount = 3;

	vertices = new VertexType[mVertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[mIndexCount];
	if (!indices)
	{
		return false;
	}

	//this change this to a proper mesh loading thingy later

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);

	delete []vertices;
	vertices = nullptr;

	delete[]indices;
	indices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = nullptr;
	}
	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
