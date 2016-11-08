#include "D3D11App.h"

D3D11App::D3D11App()
{
	assert(mApp == nullptr);
	mApp = this;
}


D3D11App::~D3D11App()
{
}

bool D3D11App::InitD3D()
{
<<<<<<< Updated upstream
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = mClientWidth;
	scd.BufferDesc.Height = mClientHeight;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = mMainWindow;
	scd.SampleDesc.Count = 4;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = true;

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
=======
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
>>>>>>> Stashed changes
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

<<<<<<< Updated upstream
	ID3D11Texture2D *pBackBuffer;
	ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
=======
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
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
	vp.Width = 800;// remove magic numbers
	vp.Height = 600;// when this class andle window creation (width + height will be here)
=======
	vp.Width = static_cast<float>(mClientWidth);
	vp.Height = static_cast<float>(mClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
>>>>>>> Stashed changes

	md3dImmediateContext->RSSetViewports(1, &vp);

	return true;
}

void D3D11App::InitPipeline()
{
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	ThrowIfFailed(md3dDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &mpVS));
	ThrowIfFailed(md3dDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &mpPS));

	md3dImmediateContext->VSSetShader(mpVS, 0, 0);
	md3dImmediateContext->PSSetShader(mpPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ThrowIfFailed(md3dDevice->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &mpLayout));
	md3dImmediateContext->IASetInputLayout(mpLayout);
}

void D3D11App::CleanD3D()
{
	mpVS->Release();
	mpPS->Release();
	mSwapChain->Release();
	mRenderTargetView->Release();
	md3dDevice->Release();
	md3dImmediateContext->Release();
}

void D3D11App::Draw(void)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	// render stuff

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
<<<<<<< Updated upstream
	mDeviceContext->IASetVertexBuffers(0, 1, &mpVBuffer, &stride, &offset);
=======
	
	ID3D11Buffer **vBuffers = {
		&mpVBuffer
	};

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mpVBuffer, &stride, &offset);

	md3dImmediateContext->IASetIndexBuffer(mpIBuffer, DXGI_FORMAT_R32_UINT, 0);
>>>>>>> Stashed changes

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

<<<<<<< Updated upstream
	mDeviceContext->Draw(3, 0);
=======
	md3dImmediateContext->DrawIndexed(6, 0, 0);
>>>>>>> Stashed changes

	ThrowIfFailed(mSwapChain->Present(0, 0));
}

bool D3D11App::Init(HINSTANCE hInstance, int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd))
		return false;
	if (!InitD3D())
		return false;

	InitPipeline();
	// other init code

	VERTEX TVertices[]{
<<<<<<< Updated upstream
		{ 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ -0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) }
=======
		/*
		 * 1   2
		 *
		 * 0   3
		 */
		{ -0.5f, -0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ -0.5f, 0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ 0.5f, 0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
		{ 0.5f, -0.5f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f) },

	};

	int TIndices[]{
		0, 1, 2,// front
		0, 2, 3
>>>>>>> Stashed changes
	};

	// description for triangle buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
<<<<<<< Updated upstream
	bd.ByteWidth = sizeof(VERTEX) * 3;
=======
	bd.ByteWidth = sizeof(VERTEX) * 8;
>>>>>>> Stashed changes
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ThrowIfFailed(md3dDevice->CreateBuffer(&bd, NULL, &mpVBuffer));

	D3D11_MAPPED_SUBRESOURCE ms;
	ThrowIfFailed(md3dImmediateContext->Map(mpVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
	memcpy(ms.pData, TVertices, sizeof(TVertices));
	md3dImmediateContext->Unmap(mpVBuffer, NULL);

<<<<<<< Updated upstream
=======
	//description for Indicie buffer
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(int) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ThrowIfFailed(md3dDevice->CreateBuffer(&bd, NULL, &mpIBuffer));
	ThrowIfFailed(md3dImmediateContext->Map(mpIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
	memcpy(ms.pData, TIndices, sizeof(TIndices));
	md3dImmediateContext->Unmap(mpIBuffer, NULL);
	
>>>>>>> Stashed changes
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
			Draw();
		}
	}
	return (int)msg.wParam;
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