#pragma once


#include <string>
#include "GameTimer.h"

// Interface new implimentarions should derive from
class IRenderFramework
{
public:

	// width, height and title of the window
	int mClientWidth = 1600, mClientHeight = 900;// 1600 900
protected:
	GameTimer mTimer;

	std::wstring mClientTitle = L"DX11 Rendering Engine";
public:
	IRenderFramework();
	virtual ~IRenderFramework();

	virtual void OnResize(bool) = 0;
	virtual int Run() = 0;
	void GetScreenWidth(int &width);
	void GetScreenHeight(int &height);

protected:
	virtual void Start() = 0;
	virtual void Update(const GameTimer &gt) = 0;
	virtual void Draw(const GameTimer &gt) = 0;
};

