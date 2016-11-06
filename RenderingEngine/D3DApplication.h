#pragma once

#include <Windows.h>
#include <wrl.h>
#include <assert.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "utils.h"

// dont forget to set Target Platform Version to '10.x' p.xxxiii (Luna)
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;

// Handles all DirectX specific stuff
class D3DApplication
{
private:
	HWND mhMainWindow = nullptr;
	static D3DApplication *mApp;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 4;

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

	D3DApplication();
	
	bool InitWindowsApp(HINSTANCE instanceHandle, int show);
	bool InitD3D();

	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDesvriptorHeaps();

	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	int mClientWidth = 800, mClientHeight = 600;
protected:

public:
	// Ctor
	D3DApplication(HINSTANCE);
	//Dtor
	~D3DApplication();

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	bool Init(HINSTANCE instanceHandle, int show);
	int Run();
	static D3DApplication *getApp();

};

