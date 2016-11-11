#include "CameraClass.h"



CameraClass::CameraClass()
{
	mPositionX = 0;
	mPositionY = 0;
	mPositionZ = 0;

	mRotationX = 0;
	mRotationY = 0;
	mRotationZ = 0;
}

CameraClass::CameraClass(const CameraClass &)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	mPositionX = x;
	mPositionY = y;
	mPositionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	mRotationX = x;
	mRotationY = y;
	mRotationZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(mPositionX, mPositionY, mPositionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(mRotationX, mRotationY, mRotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	position.x = mPositionX;
	position.y = mPositionY;
	position.z = mPositionZ;

	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	pitch = mRotationX * DEGTORAD;
	yaw = mRotationY * DEGTORAD;
	roll = mRotationZ * DEGTORAD;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);

	mViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void CameraClass::GetViewMatrix(XMMATRIX &viewMatrix)
{
	viewMatrix = mViewMatrix;
	return;
}
