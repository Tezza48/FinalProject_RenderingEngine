#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Material.h"

using namespace DirectX;

class ColorMaterial :
	public Material
{
private:
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

protected:
	ColorMaterial();

public:
	ColorMaterial(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular);
	~ColorMaterial();

	XMFLOAT4 GetAmbient() { return Ambient; }
	XMFLOAT4 GetDiffuse() { return Diffuse; }
	XMFLOAT4 GetSpecular() { return Specular; }

private:

public:

};

