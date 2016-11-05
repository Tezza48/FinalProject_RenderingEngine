#include <Windows.h>
#include "D3DApplication.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	D3DApplication d3dApp = D3DApplication(hInstance);
	d3dApp.InitWindowsApp(hInstance, nCmdShow);

	return d3dApp.Run();
}