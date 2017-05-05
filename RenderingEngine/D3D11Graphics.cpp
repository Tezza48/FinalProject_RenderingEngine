#include "D3D11Graphics.h"

D3D11Graphics::D3D11Graphics()
{
	mLitShader = nullptr;

	mRS = nullptr;

	md3dDevice = nullptr;
	md3dImmediateContext = nullptr;
	mSwapChain = nullptr;

	mDepthStencilBuffer = nullptr;
	mRenderTargetView = nullptr;
	mDepthStencilView = nullptr;
}

D3D11Graphics::~D3D11Graphics()
{
	mDepthStencilView->Release();
	mRenderTargetView->Release();
	mDepthStencilBuffer->Release();

	mSwapChain->Release();
	md3dImmediateContext->Release();
	md3dDevice->Release();
	
	mRS->Release();
	
	delete mLitShader;
	mLitShader = nullptr;
}

ID3D11Device * D3D11Graphics::GetDevice() const
{
	return md3dDevice;
}

ID3D11DeviceContext * D3D11Graphics::GetImmediateContext() const
{
	return md3dImmediateContext;
}

int D3D11Graphics::GetScreenWidth()
{
	return mClientWidth;
}

int D3D11Graphics::GetScreenHeight()
{
	return mClientHeight;
}

bool D3D11Graphics::Init(HWND hwnd, int width, int height)
{
	mClientWidth = width;
	mClientHeight = height;

	if (!InitD3D(hwnd))
		return false;

	if (!InitPipeline())
		return false;
	return true;
}

bool D3D11Graphics::InitD3D(HWND hwnd)
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
	OnResize(hwnd, mClientWidth, mClientHeight);

	return true;
}

// Resize the buffers we're using
// If it's running, also remake the camera's projection.
void D3D11Graphics::OnResize(HWND hwnd, int width, int height)
{
	mClientWidth = width;
	mClientHeight = height;

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
	sd.OutputWindow = hwnd;
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
	dxgiFactory->MakeWindowAssociation(hwnd, /*DXGI_MWA_NO_ALT_ENTER*/0);

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
}

bool D3D11Graphics::InitPipeline()
{
	// Rasterizer Description.
	// Only using it for quick debugging.
	// May possibly add multiple so
	// i can draw some objects differently
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;
	rd.AntialiasedLineEnable = false;
	
	md3dDevice->CreateRasterizerState(&rd, &mRS);
	mLitShader = new LitShader();

	// Initialize the basic shader we're using
	if (!mLitShader->Init(md3dDevice))
	{
		MessageBox(NULL, L"Could not initialize the Lit Color Shader object", L"ERROR", MB_OK);
		return false;
	}

	return true;
}

void D3D11Graphics::DrawBegin()
{	
	// Clear the RTV and DSV in preparation for drawing
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, DirectX::Colors::CornflowerBlue);

	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, (UINT8) 0.0f);

	// Set the rasterizer state to our settings
	md3dImmediateContext->RSSetState(mRS);
}

void D3D11Graphics::DrawEnd()
{
	// present the back buffer
	mSwapChain->Present(0, 0);
}
