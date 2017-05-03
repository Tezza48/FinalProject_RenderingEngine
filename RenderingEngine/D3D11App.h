#pragma once

#include <WINDOWS.h>
#include <WRL.h>
#include <ASSERT.h>
#include <STRING>
#include <D3D11.h>
#include <D3DX11.h>
//#include <D3DX10.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "IRenderFramework.h"
#include "Mesh.h"
#include "Camera.h"
#include "LitShader.h"
#include "Material.h"
#include "utils.h"
#include "Scene.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

using namespace DirectX;
using namespace DX;

// Global Message procedure, redirects to the class's message proc
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

class D3D11App : IRenderFramework
{
	bool mIsRunning;

	// Rasterizer state i'm using to debug stuff
	ID3D11RasterizerState *mRS;

public:
	LitShader *mLitShader;

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

	class Scene *mScene;

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

	// What to do when the Window is resized
	void OnResize(bool isRunning = true);
	
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
