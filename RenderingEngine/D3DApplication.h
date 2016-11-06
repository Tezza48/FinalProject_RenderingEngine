#pragma once

#include <Windows.h>
#include <wrl.h>
#include <assert.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXColors.h>
#include "d3dx12.h"
#include "GameTimer.h"
#include "utils.h"

// dont forget to set Target Platform Version to '10.x' p.xxxiii (Luna)
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DX;

// Handles all DirectX specific stuff
class D3DApplication
{

protected:
	HINSTANCE instanceHandle;
	HWND mhMainWindow = nullptr;
	static D3DApplication *mApp;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 4;

	GameTimer mTimer;

	ComPtr<IDXGIFactory4> mdxgiFactory;
	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D12Device> md3dDevice;

	ComPtr<ID3D12Fence> mFence;

	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;

	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;
	
	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvDescriptorSize = 0;

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	
	bool InitWindowsApp();
	bool InitD3D();

	virtual void Draw(const GameTimer &);

	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDesvriptorHeaps();

	ID3D12Resource *GetCurrBackBuffer();

	UINT mCurrFence = 0;

	void FlushCommandQueue();

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	int mClientWidth = 800, mClientHeight = 600;

	Vertex mTriVertBuffer[] =
	{
		MakeVert(XMFLOAT3(0, 1.0, 1.0), XMFLOAT4(1, 0, 0, 1)),
		MakeVert(XMFLOAT3(1.0, -1.0, 1.0), XMFLOAT4(0, 1, 0, 1)),
		MakeVert(XMFLOAT3(-1.0, -1.0, 1.0), XMFLOAT4(0, 0, 1, 1))
	};
	
public:

	static D3DApplication *getApp();
	
	// Ctor
	D3DApplication(HINSTANCE);
	//Dtor
	~D3DApplication();

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	virtual bool Init();
	int Run();

};

