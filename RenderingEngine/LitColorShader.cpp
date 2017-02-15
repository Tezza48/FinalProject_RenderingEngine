#include "LitColorShader.h"

LitColorShader::LitColorShader()
{
	mVertexShader = nullptr;

	mPixelShader = nullptr;

	mInputLayout = nullptr;

	mPerObjectBuffer = nullptr;

	mPerFrameBuffer = nullptr;
}


LitColorShader::~LitColorShader()
{
	mVertexShader->Release();
	mVertexShader = nullptr;

	mPixelShader->Release();
	mPixelShader = nullptr;

	mInputLayout->Release();
	mInputLayout = nullptr;

	mPerObjectBuffer->Release();
	mPerObjectBuffer = nullptr;

//	mPerFrameBuffer->Release();
	mPerFrameBuffer = nullptr;
}

bool LitColorShader::Init(ID3D11Device *device)
{
	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;
	UINT numElements;

#if DEBUG || _DEBUG

	DX::ThrowIfFailed(D3DX11CompileFromFile(
		L"litColorVS.hlsl", NULL, NULL, "main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &vertexShaderBuffer, nullptr, NULL));

	DX::ThrowIfFailed(D3DX11CompileFromFile(
		L"litColorPS.hlsl", NULL, NULL, "main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pixelShaderBuffer, nullptr, NULL));

#else

	DX::ThrowIfFailed(D3DX11CompileFromFile(
		L"litColorVS.hlsl", NULL, NULL, "main", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &vertexShaderBuffer, nullptr, NULL));

	DX::ThrowIfFailed(D3DX11CompileFromFile(
		L"litColorPS.hlsl", NULL, NULL, "main", "ps_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pixelShaderBuffer, nullptr, NULL));
	
#endif // DEBUG || _DEBUG

	DX::ThrowIfFailed(device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		NULL, &mVertexShader));

	DX::ThrowIfFailed(device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL, &mPixelShader));

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = 12;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = 24;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the input layout
	DX::ThrowIfFailed(device->CreateInputLayout(polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&mInputLayout));

	// Release the helper buffers we no longer need
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Describe per object constant buffers
	D3D11_BUFFER_DESC perObjectBufferDesc;
	perObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	perObjectBufferDesc.ByteWidth = sizeof(PerObjectBuffer);
	perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	perObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	perObjectBufferDesc.MiscFlags = 0;
	perObjectBufferDesc.StructureByteStride = 0;

	// Possible solution, making it a multiple of 16 bytes
	D3D11_BUFFER_DESC perFrameBufferDesc;
	perFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	perFrameBufferDesc.ByteWidth = sizeof(PerFrameBuffer);
	perFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	perFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	perFrameBufferDesc.MiscFlags = 0;
	perFrameBufferDesc.StructureByteStride = 0;

	// Create the buffers
	DX::ThrowIfFailed(
		device->CreateBuffer(&perObjectBufferDesc, NULL, &mPerObjectBuffer));

	DX::ThrowIfFailed(
		device->CreateBuffer(&perFrameBufferDesc, NULL, &mPerFrameBuffer));

	D3D11_SAMPLER_DESC samplerDesc;	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	DX::ThrowIfFailed(device->CreateSamplerState(&samplerDesc, &mSamplerState));

	return true;
}

void XM_CALLCONV LitColorShader::Render(ID3D11DeviceContext *deviceContext, int indexCount,
	XMMATRIX world, XMMATRIX worldViewProj, XMMATRIX worldInvTrans)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PerObjectBuffer *pObjectBuffer;
	//PerFrameBuffer *dataPtrPerFrame;
	unsigned int bufferNumber;

	// DX requires we transpose the matrices we
	// send to our shaders (it sends it as 4 float4s and basically transposes it as it's sent.
	world = XMMatrixTranspose(world);
	worldViewProj = XMMatrixTranspose(worldViewProj);
	worldInvTrans = XMMatrixTranspose(worldInvTrans);

	// map the matrix buffer to the GPU
	hr = deviceContext->Map(mPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DX::ThrowIfFailed(hr);

	pObjectBuffer = (PerObjectBuffer*)mappedResource.pData;
	pObjectBuffer->World = world;
	pObjectBuffer->WorldViewProj = worldViewProj;
	pObjectBuffer->WorldInvTrans = worldInvTrans;
	//dataPtrPerObj->Abmient = ambient;
	//dataPtrPerObj->Directional = directional;
	//dataPtrPerObj->EyePos = eyePos;
	//dataPtrPerObj->Mat = Mat{
	//	XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
	//	XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
	//	XMFLOAT4(0.3f, 0.3f, 0.3f, 10.0f) };

	deviceContext->Unmap(mPerObjectBuffer, 0);

	//hr = deviceContext->Map(mPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//DX::ThrowIfFailed(hr);

	//dataPtrPerFrame = (PerFrameBuffer*)mappedResource.pData;
	//dataPtrPerFrame->Abmient = ambient;
	//dataPtrPerFrame->Directional = directional;
	//deviceContext->Unmap(mPerFrameBuffer, 0);

	bufferNumber = 0;

	// Actually set the shader constant buffers
	deviceContext->VSSetConstantBuffers(0, 1, &mPerObjectBuffer);
	
	deviceContext->PSSetConstantBuffers(0, 1, &mPerObjectBuffer);

	deviceContext->IASetInputLayout(mInputLayout);

	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShader(mPixelShader, NULL, 0);

	// Draw the buffers on the GPU memory
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void XM_CALLCONV LitColorShader::Render(ID3D11DeviceContext *deviceContext, int indexCount,
	XMMATRIX world, XMMATRIX worldViewProj, XMMATRIX worldInvTrans, ID3D11ShaderResourceView * texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PerObjectBuffer *pObjectBuffer;
	//PerFrameBuffer *dataPtrPerFrame;
	unsigned int bufferNumber;

	// DX requires we transpose the matrices we
	// send to our shaders (it sends it as 4 float4s and basically transposes it as it's sent.
	world = XMMatrixTranspose(world);
	worldViewProj = XMMatrixTranspose(worldViewProj);
	worldInvTrans = XMMatrixTranspose(worldInvTrans);

	// map the matrix buffer to the GPU
	hr = deviceContext->Map(mPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DX::ThrowIfFailed(hr);

	pObjectBuffer = (PerObjectBuffer*)mappedResource.pData;
	pObjectBuffer->World = world;
	pObjectBuffer->WorldViewProj = worldViewProj;
	pObjectBuffer->WorldInvTrans = worldInvTrans;

	deviceContext->Unmap(mPerObjectBuffer, 0);

	bufferNumber = 0;

	// Actually set the shader constant buffers
	deviceContext->VSSetConstantBuffers(0, 1, &mPerObjectBuffer);

	deviceContext->PSSetConstantBuffers(0, 1, &mPerObjectBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	deviceContext->IASetInputLayout(mInputLayout);

	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShader(mPixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &mSamplerState);

	// Draw the buffers on the GPU memory
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void XM_CALLCONV LitColorShader::UpdateFrame(ID3D11DeviceContext *deviceContext, 
	AmbientLight *ambient, DirectionalLight *directional, 
	PointLight *point, SpotLight *spot, 
	Mat *material, XMFLOAT4 eyePos)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PerFrameBuffer *pFrameBuffer;
	unsigned int bufferNumber;

	hr = deviceContext->Map(mPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DX::ThrowIfFailed(hr);

	pFrameBuffer = (PerFrameBuffer*)mappedResource.pData;
	pFrameBuffer->Abmient = *ambient;
	pFrameBuffer->Directional = *directional;
	pFrameBuffer->Point = *point;
	pFrameBuffer->Spot = *spot;
	pFrameBuffer->Mat = *material;
	pFrameBuffer->EyePos = eyePos;
	deviceContext->Unmap(mPerFrameBuffer, 0);

	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mPerFrameBuffer);
}
