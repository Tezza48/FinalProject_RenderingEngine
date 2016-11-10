#pragma once

#include <d3d11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class ColorShaderClass
{
private:

	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass &);
	~ColorShaderClass();

	bool Init(ID3D11Device *, HWND );
	void Shutdown();
	bool XM_CALLCONV Render(ID3D11DeviceContext *, int , XMMATRIX , XMMATRIX , XMMATRIX );

private:
	bool InitShader(ID3D11Device *, HWND , WCHAR *, WCHAR *);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob *, HWND , WCHAR *);

	bool XM_CALLCONV SetShaderParameters (ID3D11DeviceContext*, XMMATRIX , XMMATRIX , XMMATRIX );
	void RenderShader(ID3D11DeviceContext *, int );

private:
	ID3D11VertexShader *mVertexShader;
	ID3D11PixelShader *mPixelShader;
	ID3D11InputLayout *mLayout;
	ID3D11Buffer* mMatrixBuffer;

};

