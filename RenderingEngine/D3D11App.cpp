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
	#if defined(DEBUG) || defined(_DEBUG)
	
	#endif

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

	D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_10_1;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&FeatureLevels,
		1,
		D3D11_SDK_VERSION,
		&scd,
		&mSwapChain,
		&mDevice,
		NULL,
		&mDeviceContext);

	if (FAILED(hr))
	{
		ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_WARP,
			NULL,
			0,
			&FeatureLevels,
			1,
			D3D11_SDK_VERSION,
			&scd,
			&mSwapChain,
			&mDevice,
			NULL,
			&mDeviceContext
			));
	}

	ID3D11Texture2D *pBackBuffer;
	ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

	ThrowIfFailed(mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mBackBuffer));
	ThrowIfFailed(pBackBuffer->Release());

	mDeviceContext->OMSetRenderTargets(1, &mBackBuffer, NULL);

	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));

	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = mClientWidth;
	vp.Height = mClientHeight;// when this class andle window creation (width + height will be here)

	mDeviceContext->RSSetViewports(1, &vp);

	return true;
}

void D3D11App::InitPipeline()
{
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	ThrowIfFailed(mDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &mpVS));
	ThrowIfFailed(mDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &mpPS));

	mDeviceContext->VSSetShader(mpVS, 0, 0);
	mDeviceContext->PSSetShader(mpPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ThrowIfFailed(mDevice->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &mpLayout));
	mDeviceContext->IASetInputLayout(mpLayout);
}

void D3D11App::Start()
{
}

void D3D11App::CleanD3D()
{
	mpVS->Release();
	mpPS->Release();
	mSwapChain->Release();
	mBackBuffer->Release();
	mDevice->Release();
	mDeviceContext->Release();
}

void D3D11App::Draw(void)
{
	mDeviceContext->ClearRenderTargetView(mBackBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	// render stuff
	
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	
	ID3D11Buffer *vBuffers[] = {
		mpMesh->GetVBuffer()
	};

	mDeviceContext->IASetVertexBuffers(0, 1, vBuffers, &stride, &offset);

	mDeviceContext->IASetIndexBuffer(mpMesh->GetIBuffer(), DXGI_FORMAT_R32_UINT, 0);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->DrawIndexed(sizeof(mpMesh->GetIBuffer()), 0, 0);
	
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

	Start();

	VERTEX *vertices = new VERTEX[sizeof(VERTEX) * 4] {
		{ -0.5f, -0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ -0.5f, 0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ 0.5f, 0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
		{ 0.5f, -0.5f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)}
	};

	int *indices = new int[6] {
		0, 1, 2,
		0, 2, 3
	};

	mpMesh = new Mesh(vertices, indices, mDevice, mDeviceContext);

	delete indices;
	delete vertices;

	// description for triangle buffer
	
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
		MessageBox(0, L"RegisterClass FAILED", L"ERROR", 0);
		return false;
	}

	mMainWindow = CreateWindow(L"DX11RE", L"DX11 Rendering Engine",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		mClientWidth, mClientHeight, 0, 0, hInstance, 0);

	if (mMainWindow == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", L"ERROR", 0);
		return false;
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