#pragma once

#include <d3d11.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <fstream>
#include "utils.h"
#include "LightStructs.h"

using namespace DirectX;

class LitColorShader
{

	struct MatrixBufferType
	{
		XMMATRIX worldViewProj;
	};

	struct LightBufferType
	{
		AmbientLight Abmient;
	};

public:
	LitColorShader();
	~LitColorShader();

	bool Init(ID3D11Device*);
	void XM_CALLCONV Render(ID3D11DeviceContext*, int, XMMATRIX, AmbientLight);

private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mInputLayout;
	ID3D11Buffer *mMatrixBuffer;
	ID3D11Buffer *mLightBuffer;
};

