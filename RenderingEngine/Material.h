#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Material
{
	XMFLOAT4 Color;
protected:
	Material();

public:
	Material(XMFLOAT4 color);
	~Material();

	//virtual void GetMaterial(Mat *material) { material = mMaterial; }

private:
};

