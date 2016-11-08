#include "Mesh.h"

Mesh::Mesh()
{
	pVBuffer = nullptr;
	pIBuffer = nullptr;
}

Mesh::Mesh(VERTEX *vertices, int *indices, ID3D11Device *device, ID3D11DeviceContext * context)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(device->CreateBuffer(&bd, NULL, &pVBuffer));

	D3D11_MAPPED_SUBRESOURCE ms;
	DX::ThrowIfFailed(context->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
	memcpy(ms.pData, &vertices, sizeof(vertices));
	context->Unmap(pVBuffer, NULL);

	//description for Indicie buffer
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(int) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(device->CreateBuffer(&bd, NULL, &pIBuffer));
	DX::ThrowIfFailed(context->Map(pIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
	memcpy(ms.pData, &indices, sizeof(indices));
	context->Unmap(pIBuffer, NULL);

}

Mesh::~Mesh()
{
	pVBuffer->Release();
	pIBuffer->Release();
}

Mesh Mesh::operator=(const Mesh & rhs) const
{
	return rhs;
}

ID3D11Buffer * Mesh::GetVBuffer() const
{
	return pVBuffer;
}

ID3D11Buffer * Mesh::GetIBuffer() const
{
	return pIBuffer;
}
