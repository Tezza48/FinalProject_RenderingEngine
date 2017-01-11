#include <Windows.h>
#include <exception>
#include "D3D11App.h"

/*
 * http://www.directxtutorial.com/
 */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	try
	{
		D3D11App thisApp;
		if (!thisApp.Init(hInstance, nShowCmd))
			throw std::exception();

		return thisApp.Run();
	}
	catch (std::exception e)
	{
		return 0;
	}
}




