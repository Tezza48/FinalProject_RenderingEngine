#pragma once

#include <Windows.h>
#include <assert.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <DirectXColors.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

struct VERTEX
{
	float X, Y, Z;
	D3DXCOLOR Color;
};

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

class D3D11App
{
	ID3D11Buffer *mpVBuffer;
	ID3D11InputLayout *mpLayout;

protected:
	HWND mMainWindow = 0;
	static D3D11App *mApp;

	IDXGISwapChain *mSwapChain;
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;

	ID3D11RenderTargetView *mBackBuffer;

	ID3D11VertexShader *mpVS;
	ID3D11PixelShader *mpPS;

	bool InitWindowsApp(HINSTANCE hInstance, int nShowCmd);
	bool InitD3D();
	void InitPipeline();
	void CleanD3D();
	void Draw();

	const int mClientWidth = 800, mClientHeight = 600;
public:
	D3D11App();
	virtual ~D3D11App();

	static D3D11App *GetApp();
	
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	
	bool Init(HINSTANCE hInstance, int nShowCmd);

	int Run();


};

