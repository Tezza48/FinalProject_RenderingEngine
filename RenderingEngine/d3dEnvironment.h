#pragma once

#include <windows.h>

// dont forget to set Target Platform Version to '10.x' p.xxxiii (Luna)
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

namespace willterry
{
/* 
 * Responsible for setting up D3D12 API:
 * * Create a Window
 * * Initialise D3D and relevant resources etc
 */
class d3dEnvironment
{
private:
	HWND mhWnd = 0;// The window handle
	LRESULT CALLBACK WndProc(HWND _hWnd, UINT _msg, WPARAM wParam, LPARAM lParam);
public:
	d3dEnvironment();
	virtual ~d3dEnvironment();

	bool InitWin32(HINSTANCE, int);
};

}

