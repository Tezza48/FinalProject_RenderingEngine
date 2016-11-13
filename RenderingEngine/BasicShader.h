#pragma once

#include <d3d11.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <fstream>
#include "utils.h"

using namespace DirectX;

class BasicShader
{

	struct MatrixBufferType
	{
		XMMATRIX worldViewProj;
	};

public:
	BasicShader();
	~BasicShader();

	bool Init(ID3D11Device*);
	void XM_CALLCONV Render(ID3D11DeviceContext*, int, XMMATRIX);

private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mInputLayout;
	ID3D11Buffer *mMatrixBuffer;
};

