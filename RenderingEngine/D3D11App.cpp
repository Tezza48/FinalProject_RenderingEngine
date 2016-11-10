#include "D3D11App.h"

D3D11App::D3D11App()
{
	assert(mApp == nullptr);
	mApp = this;
}

D3D11App::~D3D11App()
{

}

bool D3D11App::Init(HINSTANCE hInstance, int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd))
		return false;
	if (!InitD3D())
		return false;

	InitPipeline();

	Start();
	return true;
}

bool D3D11App::InitWindowsApp(HINSTANCE hInstance, int nShowCmd)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"DX11RE";

	if (!RegisterClass(&wc))
	{
		return false;//add message box
	}

	mMainWindow = CreateWindow(
		L"DX11RE",
		L"DX11 Rendering Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		mClientWidth,
		mClientHeight,
		0,
		0,
		hInstance,
		0);

	if (mMainWindow == 0)
	{
		return false;//add message box
	}

	ShowWindow(mMainWindow, nShowCmd);
	UpdateWindow(mMainWindow);

	return true;
}

bool D3D11App::InitD3D()
{

	UINT createDeviceFlags = 0;

	#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createDeviceFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	ThrowIfFailed(md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));

	assert(m4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;// swap chain description
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;//book uses if statement, DX12 uses ?:
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;//?: is a little neather here
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mMainWindow;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	{
		IDXGIDevice *dxgiDevice = 0;
		ThrowIfFailed(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

		IDXGIAdapter *dxgiAdapter = 0;
		ThrowIfFailed(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

		IDXGIFactory *dxgiFactory = 0;
		ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

		ThrowIfFailed(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

		dxgiDevice->Release();
		dxgiAdapter->Release();
		dxgiFactory->Release();
	}
	{
		ID3D11Texture2D *backBuffer;
		mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			reinterpret_cast<void**>(&backBuffer));
		md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);
		backBuffer->Release();
	}

	D3D11_TEXTURE2D_DESC dsd;// depth stencil description
	dsd.Width = mClientWidth;
	dsd.Height = mClientHeight;
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	dsd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;

	ThrowIfFailed(md3dDevice->CreateTexture2D(
		&dsd,
		0,
		&mDepthStencilBuffer));

	ThrowIfFailed(md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer,
		0,
		&mDepthStencilView));

	md3dImmediateContext->OMGetRenderTargets(1, &mRenderTargetView, &mDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(mClientWidth);
	vp.Height = static_cast<float>(mClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &vp);

	OnResize();

	return true;
}

bool D3D11App::InitPipeline()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return true;
}

void D3D11App::OnResize()
{
	//mSwapChain->ResizeBuffers()
}

void D3D11App::Start()
{
	mTimer = GameTimer();
}

int D3D11App::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//Update();
			//Draw();
		}
	}
	return (int)msg.wParam;
}

void D3D11App::Update(const GameTimer & gt)
{
}

void D3D11App::Draw(const GameTimer &gt)
{

}

float D3D11App::AspectRatio()
{
	return static_cast<float>(mClientWidth/mClientHeight);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return D3D11App::GetApp()->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK D3D11App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(mMainWindow);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

D3D11App *D3D11App::mApp = nullptr;
D3D11App *D3D11App::GetApp()
{
	return mApp;
}