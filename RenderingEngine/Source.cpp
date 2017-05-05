#include <Windows.h>
#include <exception>
#include "NoobieEngine.h"

/*
 * http://www.directxtutorial.com/
 */

NoobieEngine gEngine;

HWND gMainWindow;
bool gIsRunning = false;

std::wstring gClientTitle = L"Noobie Engine";
//int gClientWidth = 1600, gClientHeight = 900;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(gMainWindow);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ENTERSIZEMOVE:
		gEngine.GetTimer().Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		gEngine.GetTimer().Start();
		return 0;
	case WM_SIZE:
		if (gIsRunning)
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			gEngine.OnResize(gMainWindow, width, height);
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool InitWindowsApp(HINSTANCE hInstance, int nShowCmd, int width, int height)
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

	gMainWindow = CreateWindow(
		L"DX11RE",
		gClientTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		hInstance,
		0);

	if (gMainWindow == 0)
	{
		return false;//add message box
	}

	ShowWindow(gMainWindow, nShowCmd);
	UpdateWindow(gMainWindow);

	return true;
}

bool Run()
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
			gEngine.Update();
			gEngine.Draw();
		}
	}
	return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	int width = 1600, height = 900;
	try
	{
		if (!InitWindowsApp(hInstance, nShowCmd, width, height))
			throw std::exception();

		if (!gEngine.Init(gMainWindow, width, height))
			throw std::exception();
		gEngine.Start();

		gIsRunning = true;
		
		return Run();
	}
	catch (std::exception e)
	{
		MessageBox(NULL, L"Something went wrong.", L"ERROR", NULL);
		return 0;
	}
}




