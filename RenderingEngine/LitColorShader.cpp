#include "LitColorShader.h"

LitColorShader::LitColorShader()
{
	//defaultAmbient.Ambient = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
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

	//mPerFrameBuffer->Release();
	//mPerFrameBuffer = nullptr;
}

bool LitColorShader::Init(ID3D11Device *device)
{
	HRESULT hr;

	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;
	UINT numElements;

#if DEBUG || _DEBUG

	hr = D3DX11CompileFromFile(L"litColorVS.hlsl", NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, nullptr, NULL);
	if (FAILED(hr))
		return false;
	hr = D3DX11CompileFromFile(L"litColorPS.hlsl", NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, nullptr, NULL);
	if (FAILED(hr))
		return false;

#else

	hr = D3DX11CompileFromFile(L"litColorVS.hlsl", NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, nullptr, NULL);
	if (FAILED(hr))
		return false;
	hr = D3DX11CompileFromFile(L"litColorPS.hlsl", NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, nullptr, NULL);
	if (FAILED(hr))
		return false;

#endif // DEBUG || _DEBUG

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(hr))
		return false;
	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(hr))
		return false;

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
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = 24;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the input layout
	hr = device->CreateInputLayout(polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&mInputLayout);

	if (FAILED(hr))
		return false;

	// Release the helper buffers we no longer need
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Describe per object constant buffer buffer
	D3D11_BUFFER_DESC perObjectBufferDesc;
	perObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	perObjectBufferDesc.ByteWidth = sizeof(PerObjectBuffer);
	perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	perObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	perObjectBufferDesc.MiscFlags = 0;
	perObjectBufferDesc.StructureByteStride = 0;

	//D3D11_BUFFER_DESC perFrameBufferDesc;
	//perFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//perFrameBufferDesc.ByteWidth = sizeof(PerFrameBuffer);
	//perFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//perFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//perFrameBufferDesc.MiscFlags = 0;
	//perFrameBufferDesc.StructureByteStride = 0;

	// Create that buffer
	hr = device->CreateBuffer(&perObjectBufferDesc, NULL, &mPerObjectBuffer);
	if (FAILED(hr))
		return false;

	//hr = device->CreateBuffer(&perFrameBufferDesc, NULL, &mPerFrameBuffer);
	//if (FAILED(hr))
	//	return false;

	return true;
}

void XM_CALLCONV LitColorShader::Render(ID3D11DeviceContext *deviceContext, int indexCount,
	XMMATRIX world, XMMATRIX worldViewProj, XMMATRIX worldInvTrans,
	AmbientLight ambient, DirectionalLight directional, XMMATRIX view, ColorMaterial mat)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PerObjectBuffer *dataPtrPerObj;
	//PerFrameBuffer *dataPtrPerFrame;
	unsigned int bufferNumber;

	//{
	//	world = world * view * projection;
	//}

	// DX requires we transpose the matrices we
	// send to our shaders
	world = XMMatrixTranspose(world);
	worldViewProj = XMMatrixTranspose(worldViewProj);
	worldInvTrans = XMMatrixTranspose(worldInvTrans);

	// map the matrix buffer to the GPU
	hr = deviceContext->Map(mPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DX::ThrowIfFailed(hr);

	XMVECTOR nullVector;
	XMVECTOR eyePosVec;
	XMFLOAT3 eyePos;
	XMMatrixDecompose(&nullVector, &nullVector, &eyePosVec, view);
	XMStoreFloat3(&eyePos, eyePosVec);

	dataPtrPerObj = (PerObjectBuffer*)mappedResource.pData;
	dataPtrPerObj->World = world;
	dataPtrPerObj->WorldViewProj = worldViewProj;
	dataPtrPerObj->WorldInvTrans = worldInvTrans;
	dataPtrPerObj->Abmient = ambient;
	dataPtrPerObj->Directional = directional;
	dataPtrPerObj->EyePos = eyePos;
	dataPtrPerObj->Mat = Mat{
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 10.0f) };

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
	//deviceContext->PSSetConstantBuffers(1, 1, &mPerFrameBuffer);

	deviceContext->IASetInputLayout(mInputLayout);

	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShader(mPixelShader, NULL, 0);

	// Draw the buffers on the GPU memory
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

//void XM_CALLCONV LitColorShader::Frame(ID3D11DeviceContext *deviceContext, AmbientLight ambient, DirectionalLight directional)
//{
//	HRESULT hr;
//	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
//	PerFrameBuffer *dataPtr;
//	unsigned int bufferNumber;
//
//	hr = deviceContext->Map(mPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
//	DX::ThrowIfFailed(hr);
//
//	dataPtr = (PerFrameBuffer*)mappedSubresource.pData;
//	dataPtr->Abmient = ambient;
//	dataPtr->Directional = directional;
//	deviceContext->Unmap(mPerFrameBuffer, 0);
//
//	bufferNumber = 1;
//
//	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mPerFrameBuffer);
//
//}
