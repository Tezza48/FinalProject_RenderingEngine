#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	Material(XMFLOAT4 emmissive, XMFLOAT4 specular) { Emmissive = emmissive; Specular = specular; }
	XMFLOAT4 Emmissive;
	XMFLOAT4 Specular;
};
