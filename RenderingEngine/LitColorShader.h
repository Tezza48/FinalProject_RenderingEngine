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

	struct PerObjectBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct PerFrameBuffer
	{
		AmbientLight Abmient;
		DirectionalLight Directional;
	};

public:
	LitColorShader();
	~LitColorShader();

	bool Init(ID3D11Device *device);
	void XM_CALLCONV Render(ID3D11DeviceContext *deviceContext, int , XMMATRIX, XMMATRIX, XMMATRIX);
	void XM_CALLCONV Frame(ID3D11DeviceContext*, AmbientLight, DirectionalLight);
private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mInputLayout;

	// constant buffers
	ID3D11Buffer *mPerObjectBuffer;
	ID3D11Buffer *mPerFrameBuffer;

public:
	AmbientLight defaultAmbient;
};

