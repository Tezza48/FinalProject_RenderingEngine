#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
//#include <fstream>
#include "utils.h"
#include "LightHelper.h"
#include "Material.h"
#include "Texture.h"

using namespace DirectX;

// A shader that draws lit objects
// with ambient, direct, point and spotlights
class LitShader
{
	struct ShaderMat
	{
		XMFLOAT4 Emissive;
		XMFLOAT4 Specular;
	};
	struct PerObjectBuffer// Buffer 0
	{
		XMMATRIX World;
		XMMATRIX WorldInvTrans;
		XMMATRIX WorldViewProj;
	};

	struct PerFrameBuffer// Buffer 1
	{
		DirectionalLight Directional;// Directional Light
		PointLight Point;
		SpotLight Spot;
		XMFLOAT4 EyePos; // The eye's current world position
	};

	struct PerMaterialBuffer// Buffer 2
	{
		Material Mat;// Object Material
	};

public:
	LitShader();
	~LitShader();

	bool Init(ID3D11Device *device);
	void XM_CALLCONV Render(ID3D11DeviceContext *deviceContext, int, XMMATRIX world, XMMATRIX worldViewProj, XMMATRIX worldInvTrans);
	void XM_CALLCONV UpdateFrame(ID3D11DeviceContext *deviceContext, 
		DirectionalLight *directional, 
		PointLight *point, SpotLight *spot, 
		XMFLOAT4 eyePos);
	void UpdateMaterial(ID3D11DeviceContext *deviceContext, Material *material, ID3D11ShaderResourceView *texture);

private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mInputLayout;

	// For the sake of lazyness i'm not making a new shader
	ID3D11SamplerState *mSamplerState;

	// constant buffers
	ID3D11Buffer *mPerObjectBuffer;
	ID3D11Buffer *mPerFrameBuffer;
	ID3D11Buffer *mPerMaterialBuffer;

public:
};

