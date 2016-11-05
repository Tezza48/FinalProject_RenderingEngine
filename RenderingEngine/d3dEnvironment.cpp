#include "d3dEnvironment.h"
/*
using namespace willterry;

LRESULT CALLBACK d3dEnvironment::WndProc(HWND _hWnd, UINT _msg, WPARAM wParam, LPARAM lParam)
{
	
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World!");

	switch (_msg)
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
		return DefWindowProc(hWnd, _msg, wParam, lParam);
	}

	return 0;
}

d3dEnvironment::d3dEnvironment()
{
}


d3dEnvironment::~d3dEnvironment()
{
}

bool d3dEnvironment::InitWin32(HINSTANCE _hInstance, int show)
{
	
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;// redraw the window if moved/resized in horizontal/vertical
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	
}
*/