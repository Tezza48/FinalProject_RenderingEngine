#pragma once

#include <D3D11.h>
#include <DirectXMath.h>

using namespace DirectX;

// http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16 12/01/2017
// solution to C4316 may not be alligned 16

class Camera
{
private:
	XMMATRIX mView, mProjection;

	XMFLOAT4 mEyePos;

	float fov, aspectRatio, nearClip, farClip;
public:
	Camera();
	virtual ~Camera();

	// Not used at this moment
	void Init();

	void XM_CALLCONV GetViewMatrix(XMMATRIX&);
	XMFLOAT4 GetWorldPosition();
	void XM_CALLCONV GetProjectionMatrix(XMMATRIX&);

	void XM_CALLCONV CreateViewMatrix(XMVECTOR pos, XMVECTOR target, XMVECTOR up);
	void XM_CALLCONV SetViewMatrix(XMMATRIX);

	void CreateProjection(float fov, float aspectRatio, float nearClip, float farClip);
	void ResizeAspectRatio(float _aspectRatio);
};

