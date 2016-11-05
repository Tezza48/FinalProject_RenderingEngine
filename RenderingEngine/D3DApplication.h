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
	bool InitWindowsApp(HINSTANCE instanceHandle, int show);
	bool InitD3D();
protected:

public:
	// Ctor
	D3DApplication(HINSTANCE);
	//Dtor
	~D3DApplication();

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	bool Init(HINSTANCE instanceHandle, int show);
	int Run();
	static D3DApplication *getApp();

};

