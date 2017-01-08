#pragma once

#include <windows.h>
#include <DirectXMath.h>

using namespace DirectX;

struct AmbientLight
{
	AmbientLight() {ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
};

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
private:
	float pad;
};