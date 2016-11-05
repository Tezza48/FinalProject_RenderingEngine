#include <Windows.h>
//#include <wrl.h>
//#include <stdlib.h>
//#include <string.h>
//#include <tchar.h>
//#include <dxgi1_4.h>
//#include <d3d12.h>
//#include "d3dx12.h"//Helper functions provided by Microsoft (using book version)
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//
//#include "utils.h"

// dont forget to set Target Platform Version to '10.x' p.xxxiii (Luna)
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

//using namespace std;
//using namespace Microsoft::WRL;
//using namespace DirectX;
//using namespace DX;

//static TCHAR szWindowClass[] = _T("Rendering Engine");// the name of the application
//static TCHAR szTitle[] = _T("DirectX 12 powered Rendering Engine");// the full title of the application
//
//HINSTANCE hInst;
//HWND hWnd;
//
//ComPtr<IDXGIFactory4> dxgiFactory;
//ComPtr<IDXGISwapChain> SwapChain;
//ComPtr<ID3D12Device> d3dDevice;
//
//ComPtr<ID3D12CommandQueue> commandQueue;
//ComPtr<ID3D12CommandAllocator> directCmsListAlloc;
//ComPtr<ID3D12GraphicsCommandList> commandList;
//
//ComPtr<ID3D12DescriptorHeap> rtvHeap;
//ComPtr<ID3D12DescriptorHeap> dsvHeap;
//
//ComPtr<ID3D12Resource> DepthStencilBuffer;
//
//DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;// 8bit per channel RGBA 0 to 1 range
//DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//
//const int SwapChainBufferCount = 2;
//int currBackBuffer = 0;
//
//ComPtr<ID3D12Fence> d3dFence;
//UINT rtvDescriptorSize = 0;
//UINT dsvDescriptorSize = 0;
//UINT cbvSrvDescriptorSize = 0;
//
//bool msaa4xState = false;
//UINT msaa4xQuality = 0;
//
//int clientWidth = 800;
//int clientHeight = 600;
//
//// prototype functions
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//void InitD3D();
//void InitFence();
//void CheckMSAA();
//void CreateCommandObjects();
//void CreateSwapChain();
//void CreateRtvAndDsvDescriptorHeaps();
//
//D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView(); // const safe in example class
//D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
HWND ghMainWnd = 0;

bool InitWindowsApp(HINSTANCE instanceHandle, int show);
int Run();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	if (!InitWindowsApp(hInstance, nCmdShow))
		return 0;

	return Run();
	/*
	// window class structure
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;// redraw the window if moved/resized in horizontal/vertical
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	
	// register the window class
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(
			NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("DX12 Rendering Engine"),
			NULL
			);
	
		return 1;
	}
	
	::hInst = hInstance;
	
	// Now we can create the window
	::hWnd = CreateWindow(::szWindowClass, ::szTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, ::clientWidth, ::clientHeight, NULL, NULL, hInstance, NULL);// TODO: want to make this not resizable
	
	if (!hWnd)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("DX12 Rendering Engine"), NULL);
	
		return 1;
	}
	
	// now display the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	InitD3D();
	InitFence();
	CheckMSAA();
	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	
	// Creating RTV to both buffers, Example has this inside `OnResize` Method
	// it seems lots of this needs to be done when resizing, i'm not going to 
	// allow resizing until i really understand this.
	ComPtr<ID3D12Resource> SwapChainBuffer[SwapChainBufferCount];//didn't like it being in global scope so it's here
	//Literal copy from book
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(::rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < ::SwapChainBufferCount; i++)
	{
		ThrowIfFailed(::SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i])));
		::d3dDevice->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, ::rtvDescriptorSize);
	}
	
	//Create Depth/Stencil Buffer And View
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = ::clientWidth;
	depthStencilDesc.Height = ::clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = ::depthStencilFormat;
	depthStencilDesc.SampleDesc.Count = ::msaa4xState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = ::msaa4xState ? (::msaa4xQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = ::depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(::d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS( ::DepthStencilBuffer.GetAddressOf() )));
	
	::d3dDevice->CreateDepthStencilView(
		::DepthStencilBuffer.Get(),
		nullptr,
		DepthStencilView());

	::commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			::DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Set up Message Loop
	//modified to have a render section/logic section
	MSG msg;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

		}
	}

	return (int) msg.wParam;
	*/
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
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

	ghMainWnd = CreateWindow(
		L"BasicWndClass",
		L"Win32Basic",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instanceHandle,
		0);

	if (ghMainWnd == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		return false;
	}

	ShowWindow(ghMainWnd, show);
	UpdateWindow(ghMainWnd);

	return true;
}

int Run()
{
	MSG msg = { 0 };

	BOOL bRet = 1;
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)// While the message isn't WM_QUIT
	{
		if (bRet == -1)
		{
			MessageBox(0, L"GetMessage FAILED", 0, 0);
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

// Where we handle the messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		MessageBox(0, L"Hello, World", L"Hello", MB_OK);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(ghMainWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	PAINTSTRUCT ps;
//	HDC hdc;
//	TCHAR greeting[] = _T("Hello, World!");
//
//	switch (message)
//	{
//	case WM_PAINT:
//		hdc = BeginPaint(hWnd, &ps);
//
//		TextOut(
//			hdc,
//			5, 5,
//			greeting, _tcslen(greeting));
//
//		EndPaint(hWnd, &ps);
//		break;
//	case WM_DESTROY:
//		PostQuitMessage(0);// quits the program when the window is closed (i assume(confirmed))
//		break;
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//
//	return 0;
//}
//
//void InitD3D()
//{
//	// Create the Device
//	#if defined(DEBUG) || defined(_DEBUG)
//	// Enable D3D12 debug layer
//	ComPtr<ID3D12Debug> debugController;
//
//	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
//	debugController->EnableDebugLayer();
//
//	#endif // defined(DEBUG) || defined(_DEBUG)
//
//	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&::dxgiFactory)));
//
//	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&::d3dDevice));
//
//	// if that failed fall back to WARP (Windows advanced Rasterisation Protocol)
//
//	if (FAILED(hardwareResult))
//	{
//		ComPtr<IDXGIAdapter> pWarpAdapter;
//		ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
//
//		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&::d3dDevice)));
//	}
//}
//
//void InitFence()
//{
//	// Create Fence
//	ThrowIfFailed(::d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&::d3dFence)));
//	::rtvDescriptorSize = ::d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//	::dsvDescriptorSize = ::d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
//	::cbvSrvDescriptorSize = ::d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//}
//
//void CheckMSAA()
//{
//	// Check 4X MSAA Quality Support
//
//	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
//	msQualityLevels.Format = ::backBufferFormat;//8 bit rgba
//	msQualityLevels.SampleCount = 4;//4x msaa required on DX11 capable hardware
//	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
//	msQualityLevels.NumQualityLevels = 0;
//	ThrowIfFailed(::d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
//
//	::msaa4xQuality = msQualityLevels.NumQualityLevels;
//	assert(::msaa4xQuality > 0 && "Unexpected MSAA Quality Level.");// macro that quits with message if statement is false
//}
//
//void CreateCommandObjects()
//{
//	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	ThrowIfFailed(::d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&::commandQueue)));
//
//	ThrowIfFailed(::d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( ::directCmsListAlloc.GetAddressOf() )));
//
//	ThrowIfFailed(::d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ::directCmsListAlloc.Get(), nullptr/*pipelane state(ch 6)*/, IID_PPV_ARGS( ::commandList.GetAddressOf() )));
//
//	::commandList->Close();//need to reset it, can onlt do this if it's closed
//}
//
//void CreateSwapChain()
//{
//	::SwapChain.Reset();
//
//	DXGI_SWAP_CHAIN_DESC sd;
//	// BufferDesc
//	sd.BufferDesc.Width = ::clientWidth;
//	sd.BufferDesc.Height = ::clientHeight;
//	sd.BufferDesc.RefreshRate.Numerator = 60;// make variable
//	sd.BufferDesc.RefreshRate.Denominator = 1;
//	sd.BufferDesc.Format = ::backBufferFormat;
//	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;// research me
//	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// and me
//	// SampleDesc
//	sd.SampleDesc.Count = ::msaa4xState ? 4 : 1;
//	sd.SampleDesc.Quality = ::msaa4xState ? (::msaa4xQuality - 1) : 0;
//	// Other
//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	sd.BufferCount = ::SwapChainBufferCount;
//	sd.OutputWindow = ::hWnd;
//	sd.Windowed = true;// make variable
//	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//
//	ThrowIfFailed(::dxgiFactory->CreateSwapChain( ::commandQueue.Get(), &sd, ::SwapChain.GetAddressOf() ));
//}
//
//void CreateRtvAndDsvDescriptorHeaps()
//{
//	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
//	rtvHeapDesc.NumDescriptors = ::SwapChainBufferCount;
//	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	rtvHeapDesc.NodeMask = 0;// research me
//
//	ThrowIfFailed(::d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS( ::rtvHeap.GetAddressOf() )));
//
//	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
//	dsvHeapDesc.NumDescriptors = 1;
//	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	dsvHeapDesc.NodeMask = 0;// research me
//
//	ThrowIfFailed(::d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(::dsvHeap.GetAddressOf())));
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()
//{
//	return CD3DX12_CPU_DESCRIPTOR_HANDLE(::rtvHeap->GetCPUDescriptorHandleForHeapStart(), ::currBackBuffer, ::rtvDescriptorSize );
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()
//{
//	return ::dsvHeap->GetCPUDescriptorHandleForHeapStart();
//}