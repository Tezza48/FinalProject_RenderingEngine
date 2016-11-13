#pragma once
#include "GameTimer.h"

// Interface new API implimentarions should derive from
class IRenderFramework
{
protected:
	GameTimer mTimer;

	virtual void Start() = 0;
	virtual void Update(const GameTimer &gt) = 0;
	virtual void Draw(const GameTimer &gt) = 0;;

public:
	virtual void OnResize(bool) = 0;
	virtual int Run() = 0;

	IRenderFramework();
	virtual ~IRenderFramework();
};

