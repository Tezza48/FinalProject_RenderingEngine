#include "Mesh.h"

Mesh::Mesh()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;
	//mMaterial = nullptr;

	mWorld = XMMatrixIdentity();
}

Mesh::~Mesh()
{
	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}

	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = nullptr;
	}

	//delete mMaterial;
	//mMaterial = nullptr;
}

bool Mesh::Init(ID3D11Device *device, PrimativeShape shape)
{
	Vertex *vertices = nullptr;
	unsigned long *indices = nullptr;

	unsigned long numVertices = 0;
	unsigned long numIndices = 0;

	switch (shape)
	{
	case Mesh::MESH_TRIANGLE:
	{
		numVertices = 3;
		numIndices = 3;

		vertices = new Vertex[numVertices];
		indices = new unsigned long[numIndices];

		vertices[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
		vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[0].tex = XMFLOAT2(0.0f, 0.0f);
		//vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		vertices[1].position = XMFLOAT3(0.0f, 0.5f, 0.0f);
		vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[1].tex = XMFLOAT2(0.5f, 1.0f);
		//vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[2].position = XMFLOAT3(0.5f, -0.5f, 0.0f);
		vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[2].tex = XMFLOAT2(1.0f, 0.0f);
		//vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);


		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
	}
		break;
	//case Mesh::MESH_SQUARE:

	//	break;
	case Mesh::MESH_CUBE:
	{
		numVertices = 24;
		numIndices = 36;

		vertices = new Mesh::Vertex[numVertices];
		indices = new unsigned long[numIndices];

		//front
		vertices[0].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		vertices[1].position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
		vertices[1].normal = vertices[0].normal;

		vertices[2].position = XMFLOAT3(0.5f, 0.5f, -0.5f);
		vertices[2].normal = vertices[0].normal;

		vertices[3].position = XMFLOAT3(0.5f, -0.5f, -0.5f);
		vertices[3].normal = vertices[0].normal;

		vertices[0].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[1].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[2].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[3].tex = XMFLOAT2(1.0f, 0.0f);

		//right
		vertices[4].position = XMFLOAT3(0.5f, -0.5f, -0.5f);
		vertices[4].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
		
		vertices[5].position = XMFLOAT3(0.5f, 0.5f, -0.5f);
		vertices[5].normal = vertices[4].normal;

		vertices[6].position = XMFLOAT3(0.5f, 0.5f, 0.5f);
		vertices[6].normal = vertices[4].normal;

		vertices[7].position = XMFLOAT3(0.5f, -0.5f, 0.5f);
		vertices[7].normal = vertices[4].normal;

		vertices[4].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[5].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[6].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[7].tex = XMFLOAT2(1.0f, 0.0f);

		//back
		vertices[8].position = XMFLOAT3(0.5f, -0.5f, 0.5f);
		vertices[8].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

		vertices[9].position = XMFLOAT3(0.5f, 0.5f, 0.5f);
		vertices[9].normal = vertices[8].normal;

		vertices[10].position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
		vertices[10].normal = vertices[8].normal;

		vertices[11].position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
		vertices[11].normal = vertices[8].normal;

		vertices[8].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[9].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[10].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[11].tex = XMFLOAT2(1.0f, 0.0f);

		//left
		vertices[12].position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
		vertices[12].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

		vertices[13].position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
		vertices[13].normal = vertices[12].normal;

		vertices[14].position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
		vertices[14].normal = vertices[12].normal;

		vertices[15].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[15].normal = vertices[12].normal;

		vertices[12].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[13].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[14].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[15].tex = XMFLOAT2(1.0f, 0.0f);

		//top
		vertices[16].position = XMFLOAT3(0.5f, 0.5f, 0.5f);
		vertices[16].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		vertices[17].position = XMFLOAT3(0.5f, 0.5f, -0.5f);
		vertices[17].normal = vertices[16].normal;

		vertices[18].position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
		vertices[18].normal = vertices[16].normal;

		vertices[19].position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
		vertices[19].normal = vertices[16].normal;

		vertices[16].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[17].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[18].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[19].tex = XMFLOAT2(1.0f, 0.0f);

		//bottom
		vertices[20].position = XMFLOAT3(0.5f, -0.5f, -0.5f);
		vertices[20].normal = XMFLOAT3(0.0f, -0.1f, 0.0f);

		vertices[21].position = XMFLOAT3(0.5f, -0.5f, 0.5f);
		vertices[21].normal = vertices[20].normal;

		vertices[22].position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
		vertices[22].normal = vertices[20].normal;

		vertices[23].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[23].normal = vertices[20].normal;

		vertices[20].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[21].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[22].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[23].tex = XMFLOAT2(1.0f, 0.0f);

		//front
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

		//right
		indices[6] = 4;
		indices[7] = 5;
		indices[8] = 6;

		indices[9] = 4;
		indices[10] = 6;
		indices[11] = 7;

		//back
		indices[12] = 8;
		indices[13] = 9;
		indices[14] = 10;

		indices[15] = 8;
		indices[16] = 10;
		indices[17] = 11;

		//left
		indices[18] = 12;
		indices[19] = 13;
		indices[20] = 14;

		indices[21] = 12;
		indices[22] = 14;
		indices[23] = 15;


		//top
		indices[24] = 16;
		indices[25] = 17;
		indices[26] = 18;

		indices[27] = 16;
		indices[28] = 18;
		indices[29] = 19;

		//bottom
		indices[30] = 20;
		indices[31] = 21;
		indices[32] = 22;

		indices[33] = 20;
		indices[34] = 22;
		indices[35] = 23;

	}
		break;
	case Mesh::MESH_SOFTCUBE:
	{
		numVertices = 8;
		numIndices = 36;

		vertices = new Vertex[numVertices];
		indices = new unsigned long[numIndices];

		// Front
		vertices[0].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[0].normal = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);
		vertices[0].tex = XMFLOAT2(0.0f, 0.0f);

		vertices[1].position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
		vertices[1].normal = XMFLOAT3(-0.57735f, 0.57735f, -0.57735f);
		vertices[1].tex = XMFLOAT2(0.0f, 1.0f);

		vertices[2].position = XMFLOAT3(0.5f, 0.5f, -0.5f);
		vertices[2].normal = XMFLOAT3(0.57735f, 0.57735f, -0.57735f);
		vertices[2].tex = XMFLOAT2(1.0f, 1.0f);

		vertices[3].position = XMFLOAT3(0.5f, -0.5f, -0.5f);
		vertices[3].normal = XMFLOAT3(0.57735f, -0.57735f, -0.57735f);
		vertices[3].tex = XMFLOAT2(1.0f, 0.0f);

		// Back
		vertices[4].position = XMFLOAT3(0.5f, -0.5f, 0.5f);
		vertices[4].normal = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
		vertices[4].tex = XMFLOAT2(0.0f, 0.0f);

		vertices[5].position = XMFLOAT3(0.5f, 0.5f, 0.5f);
		vertices[5].normal = XMFLOAT3(0.57735f, 0.57735f, 0.57735f);
		vertices[5].tex = XMFLOAT2(0.0f, 1.0f);

		vertices[6].position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
		vertices[6].normal = XMFLOAT3(-0.57735f, 0.57735f, 0.57735f);
		vertices[6].tex = XMFLOAT2(1.0f, 1.0f);

		vertices[7].position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
		vertices[7].normal = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
		vertices[7].tex = XMFLOAT2(1.0f, 0.0f);


		//F
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

		//R
		indices[6] = 3;
		indices[7] = 2;
		indices[8] = 5;

		indices[9] = 3;
		indices[10] = 5;
		indices[11] = 4;

		//Ba
		indices[12] = 4;
		indices[13] = 5;
		indices[14] = 6;

		indices[15] = 4;
		indices[16] = 6;
		indices[17] = 7;

		//L
		indices[18] = 7;
		indices[19] = 6;
		indices[20] = 1;

		indices[21] = 7;
		indices[22] = 1;
		indices[23] = 0;

		//T
		indices[24] = 1;
		indices[25] = 6;
		indices[26] = 5;

		indices[27] = 1;
		indices[28] = 5;
		indices[29] = 2;

		//Bo
		indices[30] = 7;
		indices[31] = 0;
		indices[32] = 3;

		indices[33] = 7;
		indices[34] = 3;
		indices[35] = 4;
	}
		break;
	case Mesh::MESH_PLANE:
	{
		numVertices = 4;
		numIndices = 6;

		vertices = new Vertex[numVertices];
		indices = new unsigned long[numIndices];

		vertices[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
		vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		vertices[1].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
		vertices[1].normal = vertices[0].normal;

		vertices[2].position = XMFLOAT3(0.5f, 0.5f, 0.0f);
		vertices[2].normal = vertices[0].normal;

		vertices[3].position = XMFLOAT3(0.5f, -0.5f, 0.0f);
		vertices[3].normal = vertices[0].normal;

		vertices[0].tex = XMFLOAT2(0.0f, 0.0f);
		vertices[1].tex = XMFLOAT2(0.0f, 1.0f);
		vertices[2].tex = XMFLOAT2(1.0f, 1.0f);
		vertices[3].tex = XMFLOAT2(1.0f, 0.0f);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

	}
		break;
	//case Mesh::MESH_NONE:
	//	break;
	//default:
	//	numVertices = 0;
	//	numIndices = 0;
	//	vertices = new Vertex{ Vertex {XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)} };
	//	indices = new unsigned long{0};
	//	break;
	}

	if (!InitBuffers(device, vertices, numVertices, indices, numIndices))
		return false;

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

bool Mesh::Init(ID3D11Device *device,
	Vertex *vertices, unsigned long numVertices,
	unsigned long *indices, unsigned long numIndices)
{
	return InitBuffers(device, vertices, numVertices, indices, numIndices);
}

void Mesh::Render(ID3D11DeviceContext *deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int Mesh::GetIndexCount()
{
	return mIndexCount;
}

void XM_CALLCONV Mesh::GetWorldMatrix(XMMATRIX &other)
{
	other = mWorld;
}

void XM_CALLCONV Mesh::SetWorldMatrix(XMMATRIX world)
{
	mWorld = world;
}

//void Mesh::SetMaterial(ColorMaterial * material)
//{
//	//mMaterial = material;
//}

//ColorMaterial * Mesh::GetMaterial()
//{
//	//return mMaterial;
//}

bool Mesh::InitBuffers(ID3D11Device *device, 
	Vertex *vertices, unsigned long numVertices, 
	unsigned long *indices, unsigned long numIndices)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	mVertexCount = numVertices;
	mIndexCount = numIndices;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer));

	return true;
}

void Mesh::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
