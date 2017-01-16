#include "D3D11App.h"

D3D11App::D3D11App()
{
	assert(mApp == nullptr);
	mApp = this;

	mIsRunning = false;

	mContent = nullptr;

	mMeshes = nullptr;

	mMainCamera = nullptr;

	mLitColorShader = nullptr;

	mAmbientLight = nullptr;
	mDirLight = nullptr;
	mPointLight = nullptr;
	mSpotLight = nullptr;

	mWorld = XMMatrixIdentity();
	mView = XMMatrixIdentity();
	mProjection = XMMatrixIdentity();

	mRS = nullptr;

	mMainWindow = 0;

	md3dDevice = nullptr;
	md3dImmediateContext = nullptr;
	mSwapChain = nullptr;

	mDepthStencilBuffer = nullptr;
	mRenderTargetView = nullptr;
	mDepthStencilView = nullptr;
}

D3D11App::~D3D11App()
{
	mDepthStencilView->Release();
	mRenderTargetView->Release();
	mDepthStencilBuffer->Release();

	mSwapChain->Release();
	md3dImmediateContext->Release();
	md3dDevice->Release();
	
	mRS->Release();
	
	delete mLitColorShader;
	mLitColorShader = nullptr;

	delete mContent;
	mContent = nullptr;

	delete[] mMeshes;
	mMeshes = nullptr;

	delete mMainCamera;
	mMainCamera = nullptr;

	delete mAmbientLight;
	mAmbientLight = nullptr;

	delete mDirLight;
	mDirLight = nullptr;

	delete mPointLight;
	mPointLight = nullptr;

	delete mSpotLight;
	mSpotLight = nullptr;

	mApp = nullptr;
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
	mIsRunning = true;
	return true;
}

// Standard Win32 initialization
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

	// if we're in debug, enable the debug layer
	#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevel;

	// Create Device
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

	// Check availablity of 4X MSAA (All DX11 devices should be capable)
	ThrowIfFailed(md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));

	assert(m4xMsaaQuality > 0);

	// For ease (and laziness) We initialize the rest by calling OnResize
	OnResize(mIsRunning);

	return true;
}

// Resize the buffers we're using
// If it's running, also remake the camera's projection.
void D3D11App::OnResize(bool isRunning)
{
	// Describe the Swap Chain
	DXGI_SWAP_CHAIN_DESC sd;
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

	// Create the swap chain using the given description
	IDXGIDevice *dxgiDevice = 0;
	ThrowIfFailed(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter *dxgiAdapter = 0;
	ThrowIfFailed(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory *dxgiFactory = 0;
	ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	ThrowIfFailed(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
	dxgiFactory->MakeWindowAssociation(mMainWindow, DXGI_MWA_NO_ALT_ENTER);

	// release the helper objects
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	// set up the back buffer
	ID3D11Texture2D *backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer));
	md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);
	backBuffer->Release();

	// Describe the Depth Stencil
	D3D11_TEXTURE2D_DESC dsd;
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

	// Create the Depth Stencil buffer and view
	ThrowIfFailed(md3dDevice->CreateTexture2D(
		&dsd,
		0,
		&mDepthStencilBuffer));

	ThrowIfFailed(md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer,
		0,
		&mDepthStencilView));

	// Set the RTV and DSV as render targets
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set up the viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)(mClientWidth);
	vp.Height = (float)(mClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &vp);

	if (isRunning)
	{
		mMainCamera->ResizeAspectRatio(AspectRatio());
	}
}

bool D3D11App::InitPipeline()
{
	// Rasterizer Description.
	// Only using it for quick debugging.
	// May possibly add multiple so
	// i can draw some objects differently
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;
	rd.AntialiasedLineEnable = false;
	
	md3dDevice->CreateRasterizerState(&rd, &mRS);
	mLitColorShader = new LitColorShader();

	// Initialize the basic shader we're using
	if (!mLitColorShader->Init(md3dDevice))
	{
		MessageBox(mMainWindow, L"Could not initialize the Lit Color Shader object", L"ERROR", MB_OK);
		return false;
	}

	mContent = new ContentManager();

	return true;
}

void D3D11App::Start()
{
	// Set up the Game timer
	mTimer = GameTimer();
	mTimer.Reset();

	mMeshes = mContent->LoadFBX(md3dDevice, "res/fbx/teapot.fbx", mNumMeshes);

	Texture *t = mContent->LoadTGA("res/tga/crate1_diffuse.tga");

	mMainCamera = new Camera();

	// Set up the camera's projection
	mMainCamera->CreateProjection(XM_PI / 4.0f, AspectRatio(), 1.0f, 100.0f);

	XMFLOAT4 targetXMFloat = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);

	XMVECTOR pos = XMVectorSet(0.0f, 1.0f, -4.0f, 1.0f);
	XMVECTOR target = XMLoadFloat4(&targetXMFloat);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX  V = XMMatrixLookAtLH(pos, target, up);
	mMainCamera->SetViewMatrix(V);

	mColorMaterial = new ColorMaterial(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 100.0f));
	
	mAmbientLight = new AmbientLight();
	mAmbientLight->Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mDirLight = new DirectionalLight();
	mDirLight->Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mDirLight->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLight->Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLight->Direction =  XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mPointLight = new PointLight();
	mPointLight->Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mPointLight->Diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	mPointLight->Specular = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	mPointLight->Position = XMFLOAT3(1.0f, 1.0f, -2.0f);
	mPointLight->Range = 2.5f;
	mPointLight->Attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);

	mSpotLight = new SpotLight();
	mSpotLight->Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mSpotLight->Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mSpotLight->Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mSpotLight->Position = XMFLOAT3(1.0f, 2.0f, 0.0f);
	mSpotLight->Range = 3.0f;
	mSpotLight->Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);// {-0.666667, -0.666667, -0.333333}
	mSpotLight->Spot = 1.0f;
	mSpotLight->Attenuation = XMFLOAT3(0.0f, 1.0f, 1.0f);

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
	// Adding the FPS to the window title
	std::wstring title;

	int fps = (int)(1.0f / gt.DeltaTime());	
	title = mClientTitle + L". FPS: " + std::to_wstring(fps);

	SetWindowText(mMainWindow, title.c_str());

	// Make the thing spin
	XMFLOAT3 diag = XMFLOAT3(0.5773f, 0.5773f, 0.5773f);
	XMVECTOR diagV;
	diagV = XMLoadFloat3(&diag);

	// Rotate the cube in y by the deltatime
	XMFLOAT3 up = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMVECTOR upV;
	upV = XMLoadFloat3(&up);

	XMMATRIX world;
	mMeshes[0].GetWorldMatrix(world);
	world = XMMatrixRotationAxis(upV, gt.DeltaTime()) * world;
	mMeshes[0].SetWorldMatrix(world);
}

void D3D11App::Draw(const GameTimer &gt)
{
	// Clear the RTV and DSV in preparation for drawing
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	// Set the rasterizer state to our settings
	md3dImmediateContext->RSSetState(mRS);

	mLitColorShader->UpdateFrame(md3dImmediateContext, mAmbientLight, mDirLight, mPointLight, mSpotLight,
		mColorMaterial->GetMaterial(), mMainCamera->GetWorldPosition());

	mMainCamera->GetViewMatrix(mView);
	mMainCamera->GetProjectionMatrix(mProjection);

	for (size_t i = 0; i < mNumMeshes; i++)
	{
		mMeshes[i].Render(md3dImmediateContext);
		mMeshes[i].GetWorldMatrix(mWorld);

		// DONT FORGET TO DO THIS! YOU MORON
		mWorldViewProj = mWorld * mView * mProjection;
		mWorldInvTrans = XMMatrixInverse(NULL, mWorld);
		mWorldInvTrans = XMMatrixTranspose(mWorldInvTrans);

		mLitColorShader->Render(md3dImmediateContext, mMeshes[i].GetIndexCount(), mWorld, mWorldViewProj, mWorldInvTrans);
	}

	// present the back buffer
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
		if (mIsRunning)
		{
			mClientWidth = LOWORD(lParam);
			mClientHeight = HIWORD(lParam);
			OnResize(true);
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