#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Emmissive;
	XMFLOAT4 Specular;
};
