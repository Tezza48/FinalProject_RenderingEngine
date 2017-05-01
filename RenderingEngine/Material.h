#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	Texture *texDiffuse;
	//Texture texNormal;
	//Texture texSpecular;
	//Texture texEmmissive;


	XMFLOAT4 colEmmissive;
	XMFLOAT4 colSpecular;
};
