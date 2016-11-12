#pragma once

#include <Windows.h>
#include <WRL.h>
#include <assert.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "IRenderFramework.h"
#include "utils.h"
#include "BasicShader.h"
#include "ModelClass.h"
#include "CameraClass.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
//#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

class D3D11App : IRenderFramework
{
	CameraClass *mCamera;
	ModelClass *mTriangle;
	BasicShader *mBasicShader;

	XMMATRIX mWorld, mView, mProjection;

	ID3D11RasterizerState *mWireframeRS;

protected:
	HWND mMainWindow = 0;
	static D3D11App *mApp;

	ID3D11Device *md3dDevice;
	ID3D11DeviceContext *md3dImmediateContext;
	IDXGISwapChain *mSwapChain;

	ID3D11Texture2D *mBackBuffer;
	ID3D11RenderTargetView *mRenderTargetView;

	ID3D11Texture2D *mDepthStencilBuffer;
	ID3D11DepthStencilView *mDepthStencilView;

	ID3D11InputLayout *mVertexLayout;

	ID3D11VertexShader *mpVS;
	ID3D11PixelShader *mpPS;

	bool m4xMsaaState = true;
	UINT m4xMsaaQuality = 1;

	bool InitWindowsApp(HINSTANCE hInstance, int nShowCmd);
	bool InitD3D();
	bool InitPipeline();
	void Start() override;
	void Update(const GameTimer &gt) override;
	void Draw(const GameTimer &gt) override;
	float AspectRatio();
	void OnResize();

	int mClientWidth = 1280, mClientHeight = 720;
public:
	D3D11App();
	virtual ~D3D11App();

	static D3D11App *GetApp();

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	
	bool Init(HINSTANCE hInstance, int nShowCmd);

	int Run();
};
