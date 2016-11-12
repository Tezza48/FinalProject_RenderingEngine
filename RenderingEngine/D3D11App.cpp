#include "D3D11App.h"

D3D11App::D3D11App()
{
	assert(mApp == nullptr);
	mApp = this;

	mWorld = XMMatrixIdentity();
	mView = XMMatrixIdentity();
	mProjection = XMMatrixIdentity();
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

	if (!InitPipeline())
		return false;

	Start();
	isRunning = true;
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
		mClientTitle.c_str(),
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

	OnResize();

	return true;
}

void D3D11App::OnResize()
{
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


	IDXGIDevice *dxgiDevice = 0;
	ThrowIfFailed(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter *dxgiAdapter = 0;
	ThrowIfFailed(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory *dxgiFactory = 0;
	ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	ThrowIfFailed(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
	dxgiFactory->MakeWindowAssociation(mMainWindow, DXGI_MWA_NO_ALT_ENTER);

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	ID3D11Texture2D *backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer));
	md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);
	backBuffer->Release();


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

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)(mClientWidth);
	vp.Height = (float)(mClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &vp);

	XMMATRIX P = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, AspectRatio(), 1.0f, 100.0f);
	mProjection = P;
}

bool D3D11App::InitPipeline()
{

	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	
	md3dDevice->CreateRasterizerState(&rd, &mRS);

	mBasicShader = new BasicShader();

	if (!mBasicShader->Init(md3dDevice))
	{
		MessageBox(mMainWindow, L"Could not initialize the Basic Shader object", L"ERROR", MB_OK);
		return false;
	}

	//mColorShader = new ColorShaderClass();

	//if (!mColorShader->Init(md3dDevice, mMainWindow))
	//	return false;

	return true;
}

void D3D11App::Start()
{
	mTimer = GameTimer();
	mTimer.Reset();

	//mCamera = new CameraClass();
	//mCamera->SetPosition(0.0f, 0.0f, -5.0f);

	XMVECTOR pos = XMVectorSet(0.0f, 1.0f, -2.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX  V = XMMatrixLookAtLH(pos, target, up);
	mView = V;

	{
		mTriangle = new ModelClass();

		ModelClass::VertexType *vertices = new ModelClass::VertexType[8];

		unsigned long *indices = new unsigned long[36];

		vertices[0].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		vertices[1].position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
		vertices[1].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f);

		vertices[2].position = XMFLOAT3(0.5f, 0.5f, -0.5f);
		vertices[2].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);

		vertices[3].position = XMFLOAT3(0.5f, -0.5f, -0.5f);
		vertices[3].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);



		vertices[4].position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
		vertices[4].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

		vertices[5].position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
		vertices[5].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);

		vertices[6].position = XMFLOAT3(0.5f, 0.5f, 0.5f);
		vertices[6].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		vertices[7].position = XMFLOAT3(0.5f, -0.5f, 0.5f);
		vertices[7].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);

		//front
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

		//right
		indices[6] = 3;
		indices[7] = 2;
		indices[8] = 6;

		indices[9] = 3;
		indices[10] = 6;
		indices[11] = 7;

		//back
		indices[12] = 7;
		indices[13] = 6;
		indices[14] = 5;

		indices[15] = 7;
		indices[16] = 5;
		indices[17] = 4;

		//left
		indices[18] = 4;
		indices[19] = 5;
		indices[20] = 1;

		indices[21] = 4;
		indices[22] = 1;
		indices[23] = 0;

		//top
		indices[24] = 1;
		indices[25] = 5;
		indices[26] = 6;

		indices[27] = 1;
		indices[28] = 6;
		indices[29] = 2;

		//bottom
		indices[30] = 4;
		indices[31] = 0;
		indices[32] = 3;

		indices[33] = 4;
		indices[34] = 3;
		indices[35] = 7;
		mTriangle->Init(md3dDevice, vertices, 8, indices, 36);
	}

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
			mTimer.Tick();
			Update(mTimer);
			Draw(mTimer);
		}
	}
	return (int)msg.wParam;
}

void D3D11App::Update(const GameTimer &gt)
{
	std::wstring title;

	int fps = 1.0f / gt.DeltaTime();	
	title = mClientTitle + L". FPS: " + std::to_wstring(fps);

	SetWindowText(mMainWindow, title.c_str());
}

void D3D11App::Draw(const GameTimer &gt)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	md3dImmediateContext->RSSetState(mRS);

	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMVECTOR upV;

	upV = XMLoadFloat3(&up);

	mWorld *= XMMatrixRotationAxis(upV, gt.DeltaTime());

	mWorldViewProj = mWorld * mView * mProjection;

	//mCamera->Render();

	//mCamera->GetViewMatrix(mView);

	mTriangle->Render(md3dImmediateContext);

	mBasicShader->Render(md3dImmediateContext, mTriangle->GetIndexCount(), mWorldViewProj);

	mSwapChain->Present(0, 0);
}

float D3D11App::AspectRatio()
{
	return (float)(mClientWidth) / (float)(mClientHeight);
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
	case WM_ENTERSIZEMOVE:
		mTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		mTimer.Start();
		return 0;
	case WM_SIZE:
		if (isRunning)
		{
			mClientWidth = LOWORD(lParam);
			mClientHeight = HIWORD(lParam);
			OnResize();
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

D3D11App *D3D11App::mApp = nullptr;
D3D11App *D3D11App::GetApp()
{
	return mApp;
}