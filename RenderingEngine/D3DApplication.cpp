#include "D3DApplication.h"

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// from Init Direct 3D example
	return D3DApplication::getApp()->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK D3DApplication::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		MessageBox(0, L"Hello, World", L"Hello", MB_OK);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(mhMainWindow);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool D3DApplication::Init(HINSTANCE instanceHandle, int show)
{
	if (!InitWindowsApp(instanceHandle, show))
		return false;
	if (!InitD3D())
		return false;

	mScreenViewport.TopLeftX = 0.0f;
	mScreenViewport.TopLeftY = 0.0f;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };

	return true;
}

D3DApplication::D3DApplication(HINSTANCE hInstance)
{
	assert(mApp == nullptr);
	mApp = this;
}


D3DApplication::~D3DApplication()
{
}

bool D3DApplication::InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"BasicWndClass";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		return false;
	}

	mhMainWindow = CreateWindow(
		L"BasicWndClass",
		L"Win32Basic",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		mClientWidth,
		mClientHeight,
		0,
		0,
		instanceHandle,
		0);

	if (mhMainWindow == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		return false;
	}

	ShowWindow(mhMainWindow, show);
	UpdateWindow(mhMainWindow);

	return true;
}

bool D3DApplication::InitD3D()
{
	// Create Device
	#if defined(DEBUG) || defined (_DEBUG)// Enable Debug Layer
	{
		ComPtr<ID3D12Debug> debugController;
		DX::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
	#endif

	// Create Hardware Device
	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	// Fall back to WARP (Windows Advanced Rasterisation Platform)
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		DX::ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(
			IID_PPV_ARGS(&pWarpAdapter)));

		DX::ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}

	// Crate Fence & Descriptor Sizes
	DX::ThrowIfFailed(md3dDevice->CreateFence(
		0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	DX::ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	// Create Command Queue and List
	CreateCommandObjects();

	// Describe & Create Swap Chain
	CreateSwapChain();

	CreateRtvAndDsvDesvriptorHeaps();

	// Create Render Target View
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		DX::ThrowIfFailed(mSwapChain->GetBuffer(
			i, IID_PPV_ARGS(&mSwapChainBuffer[i])));

		md3dDevice->CreateRenderTargetView(
			mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	// Create Depth/Stencil Buffer and View
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = mDepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	DX::ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(),
		nullptr,
		DepthStencilView());

	mCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

	return true;
}

void D3DApplication::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	DX::ThrowIfFailed(md3dDevice->CreateCommandQueue(
		&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	DX::ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	DX::ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(),
		nullptr,
		IID_PPV_ARGS(mCommandList.GetAddressOf())));
	mCommandList->Close();



}

void D3DApplication::CreateSwapChain()
{
	mSwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = mhMainWindow;
	sd.Windowed = true;// Make me a variable;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(), 
		&sd, 
		mSwapChain.GetAddressOf())))
	{
		MessageBox(0, L"Failed To Create Swap Chain", L"ERROR", 0);
		throw std::exception();
	}

	//DX::ThrowIfFailed(mdxgiFactory->CreateSwapChain(
	//	mCommandQueue.Get(),
	//	&sd,
	//	mSwapChain.GetAddressOf()));
}

void D3DApplication::CreateRtvAndDsvDesvriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	DX::ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	DX::ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApplication::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApplication::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();;
}

int D3DApplication::Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)// While the message isn't WM_QUIT
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else// Do Game Loopy Stuff
		{
			//DX::ThrowIfFailed(mDirectCmdListAlloc->Reset());
			//
			//DX::ThrowIfFailed(mCommandList->Reset(
			//	mDirectCmdListAlloc.Get(), nullptr));
			//
			//mCommandList->ResourceBarrier(
			//	1, &CD3DX12_RESOURCE_BARRIER::Transition(
			//		mSwapChainBuffer[mCurrBackBuffer].Get(),
			//		D3D12_RESOURCE_STATE_PRESENT,
			//		D3D12_RESOURCE_STATE_RENDER_TARGET));
			//
			//mCommandList->RSSetViewports(1, &mScreenViewport);
			//mCommandList->RSSetScissorRects(1, &mScissorRect);
			//
			//mCommandList->ClearRenderTargetView(
			//	CurrentBackBufferView(),
			//	Colors::LightSteelBlue, 0, nullptr);
			//mCommandList->ClearDepthStencilView(
			//	DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			//	1.0f, 0, 0, nullptr);
			//
			//mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(),
			//	true, &DepthStencilView());
			//
			//mCommandList->ResourceBarrier(
			//	1, &CD3DX12_RESOURCE_BARRIER::Transition(
			//		mSwapChainBuffer[mCurrBackBuffer].Get(),
			//		D3D12_RESOURCE_STATE_RENDER_TARGET,
			//		D3D12_RESOURCE_STATE_PRESENT));
			//
			//DX::ThrowIfFailed(mCommandList->Close());
			//
			//ID3D12CommandList *cmdsLists[] = { mCommandList.Get() };
			//mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
			//
			//FlushCommandQueue();
		}
	}

	return (int)msg.wParam;
}

D3DApplication *D3DApplication::mApp = nullptr;
D3DApplication *D3DApplication::getApp()
{
	return mApp;
}
