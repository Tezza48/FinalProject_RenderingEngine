#include "IRenderFramework.h"

IRenderFramework::IRenderFramework()
{
}


IRenderFramework::~IRenderFramework()
{
}

void IRenderFramework::GetScreenWidth(int &width)
{
	width = mClientWidth;
}

void IRenderFramework::GetScreenHeight(int &height)
{
	height =  mClientHeight;
}
