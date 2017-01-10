#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Material.h"

using namespace DirectX;

struct Mat
{
	Mat(XMFLOAT4 a, XMFLOAT4 d, XMFLOAT4 s) { Ambient = a; Diffuse = d; Specular = s; }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
};

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

	Mat GetMaterial() { return Mat(Ambient, Diffuse, Specular); }

private:

public:

};

