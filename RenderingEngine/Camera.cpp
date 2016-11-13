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

void XM_CALLCONV Camera::GetProjectionMatrix(XMMATRIX &other)
{
	other = mProjection;
}

void XM_CALLCONV Camera::SetViewMatrix(XMMATRIX other)
{
	mView = other;
}


void Camera::CreateProjection(float fov, float aspectRatio, float nearClip, float farClip)
{
	mProjection = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}
