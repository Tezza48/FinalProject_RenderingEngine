#include "BasicShader.h"

BasicShader::BasicShader()
{
}

BasicShader::~BasicShader()
{
}

bool BasicShader::Init(ID3D11Device *device)
{
	HRESULT hr;
	ID3D10Blob *vertexShaderBuffer, *pixelShaderBuffer;
	UINT numElements;

	// Compile the PS and the VS
	hr = D3DX11CompileFromFile(L"basicVS.hlsl", NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, nullptr, NULL);
	if (FAILED(hr))
		return false;

	hr = D3DX11CompileFromFile(L"basicPS.hlsl", NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, nullptr, NULL);
	if (FAILED(hr))
		return false;

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(hr))
		return false;

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(hr))
		return false;

	// Describe the input elements our VS requires
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the input layout
	hr = device->CreateInputLayout(polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&mInputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	// Release the helper buffers we no longer need
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Describe the matrix buffer for the shader
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create that buffer
	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);

	return true;
}

void XM_CALLCONV BasicShader::Render(ID3D11DeviceContext *deviceContext, int indexCount, XMMATRIX worldViewProj)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *dataPtr;
	unsigned int bufferNumber;

	// DX requires we transpose the matrices we
	// send to our shaders
	worldViewProj = XMMatrixTranspose(worldViewProj);

	// map the matrix buffer to the GPU
	hr = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DX::ThrowIfFailed(hr);
	
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->worldViewProj = worldViewProj;

	deviceContext->Unmap(mMatrixBuffer, 0);

	bufferNumber = 0;

	// Actually set the shader constant buffers
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);

	deviceContext->IASetInputLayout(mInputLayout);

	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShader(mPixelShader, NULL, 0);

	// Draw the buffers on the GPU memory
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
