#pragma once

#include <D3D11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	XMMATRIX mView, mProjection;
public:
	Camera();
	virtual ~Camera();

	// Not used at this moment
	void Init();

	void XM_CALLCONV GetViewMatrix(XMMATRIX&);
	void XM_CALLCONV GetProjectionMatrix(XMMATRIX&);

	void XM_CALLCONV SetViewMatrix(XMMATRIX);

	void CreateProjection(float fov, float aspectRatio, float nearClip, float farClip);
};

