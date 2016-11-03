#include <windows.h>
#include <wrl.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
//#include "d3dx12.h"// from 

// dont forget to set Target Platform Version to '10.x' p.xxxiii (Luna)
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

static TCHAR szWindowClass[] = _T("Rendering Engine");// the name of the application
static TCHAR szTitle[] = _T("DirectX 12 powered Rendering Engine");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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
	
	hInst = hInstance;

	// Now we can create the window
	HWND hWnd = CreateWindow(szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("DX12 Rendering Engine"), NULL);

		return 1;
	}


	// now display the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Create the Device
#if defined(DEBUG) || defined(_DEBUG)
	// Enable D3D12 debug layer
	ComPtr<ID3D12Debug> debugController;

#endif // defined(DEBUG) || defined(_DEBUG)
	//HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS)

	// Set up Message Loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World!");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		TextOut(
			hdc,
			5, 5,
			greeting, _tcslen(greeting));

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);// quits the program when the window is closed (i assume(confirmed))
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}