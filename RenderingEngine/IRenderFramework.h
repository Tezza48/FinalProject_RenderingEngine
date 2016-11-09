#pragma once
#include "GameTimer.h"

class IRenderFramework
{
protected:
	GameTimer mTimer;

	virtual void Start() = 0;
	virtual void Update(const GameTimer &gt) = 0;
	virtual void Draw(const GameTimer &gt) = 0;;

public:
	virtual void OnResize() = 0;
	virtual int Run() = 0;

	IRenderFramework();
	virtual ~IRenderFramework();
};

