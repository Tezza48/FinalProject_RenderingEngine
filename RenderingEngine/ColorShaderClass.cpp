#include "ColorShaderClass.h"

// http://www.rastertek.com/dx11s2tut04.html

ColorShaderClass::ColorShaderClass()
{
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mLayout = nullptr;
	mMatrixBuffer = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass &other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Init(ID3D11Device *device, HWND hWnd)
{
	bool result;

	result = InitShader(device, hWnd, L"colorVS.hlsl", L"colorPS.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool XM_CALLCONV ColorShaderClass::Render(ID3D11DeviceContext *deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return false;
}

bool ColorShaderClass::InitShader(ID3D11Device *device, HWND hWnd, WCHAR *vsFilename, WCHAR *psFilename)
{
	HRESULT hr;
	ID3D10Blob *errorMessage;
	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	hr = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hWnd, vsFilename);
		}
		else
		{
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	hr = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hWnd, psFilename);
		}
		else
		{
			MessageBox(hWnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(hr))
	{
		return false;
	}

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

	hr = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout);
	if (FAILED(hr))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

void ColorShaderClass::ShutdownShader()
{
	if (mMatrixBuffer)
	{
		mMatrixBuffer->Release();
		mMatrixBuffer = nullptr;
	}

	if (mLayout)
	{
		mLayout->Release();
		mLayout = nullptr;
	}

	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = nullptr;
	}

	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = nullptr;
	}

	return;

}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hWnd, WCHAR *shaderFilename)
{
	char *compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	
	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message", shaderFilename, MB_OK);

	return;
}

bool XM_CALLCONV ColorShaderClass::SetShaderParameters(ID3D11DeviceContext *deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *dataPtr;
	unsigned int bufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	hr = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(mMatrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);

	return true;

}

void ColorShaderClass::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{

	deviceContext->IASetInputLayout(mLayout);

	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShader(mPixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;

}
