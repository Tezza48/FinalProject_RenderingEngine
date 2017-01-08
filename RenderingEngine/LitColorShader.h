#pragma once

#include <d3d11.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <fstream>
#include "utils.h"
#include "LightStructs.h"

using namespace DirectX;

// A shader that draws lit objects
// with ambient, direct, point and spotlights
class LitColorShader
{

	struct PerObjectBuffer
	{
		XMMATRIX world;
		XMMATRIX worldInvTrans;
		XMMATRIX worldViewProj;
		AmbientLight Abmient;
		DirectionalLight Directional;
		XMFLOAT3 EyePos;
	};

	//struct PerFrameBuffer
	//{
	//};

public:
	LitColorShader();
	~LitColorShader();

	bool Init(ID3D11Device *device);
	void XM_CALLCONV Render(ID3D11DeviceContext *deviceContext, int , XMMATRIX, XMMATRIX, XMMATRIX, AmbientLight, DirectionalLight, XMMATRIX);
	//void XM_CALLCONV Frame(ID3D11DeviceContext*, AmbientLight, DirectionalLight);

private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mInputLayout;

	// constant buffers
	ID3D11Buffer *mPerObjectBuffer;
	//ID3D11Buffer *mPerFrameBuffer;

public:
	AmbientLight defaultAmbient;
};

