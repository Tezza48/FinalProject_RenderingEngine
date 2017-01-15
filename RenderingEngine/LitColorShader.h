#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
//#include <fstream>
#include "utils.h"
#include "LightHelper.h"
#include "ColorMaterial.h"

using namespace DirectX;

// A shader that draws lit objects
// with ambient, direct, point and spotlights
class LitColorShader
{
	struct PerObjectBuffer
	{
		XMMATRIX World;
		XMMATRIX WorldInvTrans;
		XMMATRIX WorldViewProj;
	};

	struct PerFrameBuffer
	{
		AmbientLight Abmient;// Ambient Light
		DirectionalLight Directional;// Directional Light
		PointLight Point;
		SpotLight Spot;
		Mat Mat;// Object Material
		XMFLOAT4 EyePos; // The eye's current world position
	};

public:
	LitColorShader();
	~LitColorShader();

	bool Init(ID3D11Device *device);
	void XM_CALLCONV Render(ID3D11DeviceContext *deviceContext, int, XMMATRIX world, XMMATRIX worldViewProj, XMMATRIX worldInvTrans);
	void XM_CALLCONV UpdateFrame(ID3D11DeviceContext *deviceContext, 
		AmbientLight *ambient, DirectionalLight *directional, 
		PointLight *point, SpotLight *spot, 
		Mat material, XMFLOAT4 eyePos);

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

