#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::Init()
{
}

void XM_CALLCONV Camera::GetViewMatrix(XMMATRIX &other)
{
	other = mView;
}

XMFLOAT4 Camera::GetWorldPosition()
{
	XMVECTOR nullVector;
	XMVECTOR eyePosVec;
	XMFLOAT4 eyePos;
	XMMatrixDecompose(&nullVector, &nullVector, &eyePosVec, mView);
	XMStoreFloat4(&eyePos, eyePosVec);
	return eyePos;
}

void XM_CALLCONV Camera::GetProjectionMatrix(XMMATRIX &other)
{
	other = mProjection;
}

void XM_CALLCONV Camera::SetViewMatrix(XMMATRIX other)
{
	mView = other;
}


void Camera::CreateProjection(float _fov, float _aspectRatio, float _nearClip, float _farClip)
{
	fov = _fov;
	aspectRatio = _aspectRatio;
	nearClip = _nearClip;
	farClip = _farClip;

	mProjection = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}

void Camera::ResizeAspectRatio(float _aspectRatio)
{
	aspectRatio = _aspectRatio;

	mProjection = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}