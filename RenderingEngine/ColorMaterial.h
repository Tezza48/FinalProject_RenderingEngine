#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Material.h"

using namespace DirectX;

struct Mat
{
	Mat() { ZeroMemory(this, sizeof(this)); }
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

	Mat GetMaterial() 
	{
		Mat mat;
		mat.Ambient = Ambient;
		mat.Diffuse = Diffuse;
		mat.Specular = Specular;
		return mat; 
	}

private:

public:

};

