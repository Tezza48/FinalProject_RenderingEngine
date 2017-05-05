#pragma once

#include <WINDOWS.h>
#include <WRL.h>
#include <ASSERT.h>
#include <STRING>
#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "IRenderFramework.h"
#include "Mesh.h"
#include "Camera.h"
#include "LitShader.h"
#include "Material.h"
#include "utils.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

using namespace DirectX;
using namespace DX;

class D3D11Graphics/* : public IRenderFramework*/
{
	// Rasterizer state i'm using to debug stuff
	ID3D11RasterizerState *mRS;

public:
	LitShader *mLitShader;

private:
	// DirextX 11 Device
	ID3D11Device *md3dDevice;
	// DirectX 11 Context
	ID3D11DeviceContext *md3dImmediateContext;
	// Swap Chain
	IDXGISwapChain *mSwapChain;

	// Required Buffers and views
	ID3D11Texture2D *mDepthStencilBuffer;
	ID3D11RenderTargetView *mRenderTargetView;
	ID3D11DepthStencilView *mDepthStencilView;

	// Anti Aliasing settings
	bool m4xMsaaState = true;
	UINT m4xMsaaQuality = 1;

	int mClientWidth, mClientHeight;

	// Initialize Direct 3D components
	bool InitD3D(HWND hwnd);
	// Initialize Specific pipeline objects
	bool InitPipeline();
	
public:
	D3D11Graphics();
	virtual ~D3D11Graphics();

	ID3D11Device *GetDevice() const;
	ID3D11DeviceContext *GetImmediateContext() const;

	int GetScreenWidth();
	int GetScreenHeight();

	// Draw/Render the scene
	void DrawBegin();
	void DrawEnd();

	// What to do when the Window is resized
	void OnResize(HWND hwnd, int width, int height);
	
	// Initialize the app
	bool Init(HWND hwnd, int width, int height);
};
