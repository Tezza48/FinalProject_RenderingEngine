#pragma once

#include <d3d11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <fstream>

//using namespace DirectX;

class BasicShader
{
public:
	BasicShader();
	~BasicShader();

	bool Init(ID3D11Device*);
	void Render(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mInputLayout;

};

