#include <Windows.h>
#include "D3DApplication.h"

/*
 * Might call this: Noobie
 * as i'm a noobie and noobies are not great but strive to be better
 * and that's exactly what this engine's doing.
 */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	try
	{
	D3DApplication d3dApp = D3DApplication(hInstance);
	d3dApp.Init();

	return d3dApp.Run();

	}
	catch (DxException &e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}