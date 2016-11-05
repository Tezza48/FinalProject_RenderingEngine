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
		CW_USEDEFAULT,
		CW_USEDEFAULT,
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
	return true;
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

		}
	}

	return (int)msg.wParam;
}

D3DApplication *D3DApplication::mApp = nullptr;
D3DApplication *D3DApplication::getApp()
{
	return mApp;
}
