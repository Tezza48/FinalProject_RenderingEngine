#pragma once

#include <windows.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Intensity;
	XMFLOAT3 Direction;
private:
	float pad;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Intensity;
	
	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Attenuation;
private:
	float pad;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Intensity;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	XMFLOAT3 Attenuation;
private:
	float pad;
};
