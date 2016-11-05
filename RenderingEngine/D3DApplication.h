#pragma once

#include <Windows.h>
#include <assert.h>

// Handles all DirectX specific stuff
class D3DApplication
{
private:
	HWND mhMainWindow = 0;
	static D3DApplication *mApp;
	D3DApplication();
protected:

public:
	D3DApplication(HINSTANCE);
	~D3DApplication();
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	bool InitWindowsApp(HINSTANCE instanceHandle, int show);
	int Run();
	static D3DApplication *getApp();

};

