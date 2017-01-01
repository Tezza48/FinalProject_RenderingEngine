#pragma once

#include <Windows.h>
#include <WRL.h>
#include <assert.h>
#include <string>
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "IRenderFramework.h"
#include "utils.h"
#include "Mesh.h"
#include "Camera.h"
#include "LitColorShader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
//#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;

// Global Message procedure, redirects to the class's message proc
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

class D3D11App : IRenderFramework
{

	bool mIsRunning;

	// Main Camera (only camera)
	Camera *mMainCamera;
	// Cube Mesh we're rendering
	Mesh *mCube;
	// Class with vertex color VS and PS
	LitColorShader *mLitColorShader;

	// we're rotating mCube for the demonstration
	float triRotY = 0.0f;

	// Matrices we use when rendering
	XMMATRIX mWorld, mView, mProjection, mWorldViewProj;

	// Rasterizer state i'm using to debug stuff
	ID3D11RasterizerState *mRS;

private:
	// Singleton of this app
	static D3D11App *mApp;

	// Main Window handle
	HWND mMainWindow;

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

	// Set up the Window
	bool InitWindowsApp(HINSTANCE hInstance, int nShowCmd);
	// Initialize Direct 3D components
	bool InitD3D();
	// Initialize Specific pipeline objects
	bool InitPipeline();
	// Set up misc things
	void Start() override;
	// Update the scene (objects, variables, other stuff)
	void Update(const GameTimer &gt) override;
	// Draw/Render the scene
	void Draw(const GameTimer &gt) override;

	// Helper Method to get the Aspect Ratio
	float AspectRatio();
	// What to do when the Window is resized
	void OnResize(bool isRunning = true);

	// width, height and title of the window
	int mClientWidth = 1280, mClientHeight = 720;
	std::wstring mClientTitle = L"DX11 Rendering Engine";
	
public:
	D3D11App();
	virtual ~D3D11App();

	// get the singleton of this app
	static D3D11App *GetApp();

	// Window Message Procedure
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	
	// Initialize the app
	bool Init(HINSTANCE hInstance, int nShowCmd);

	// Start the main loop
	int Run();
};
