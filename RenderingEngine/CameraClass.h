#pragma once

#include <DirectXMath.h> //need to write an intermediate maths library.

using namespace DirectX;

#define DEGTORAD XM_PI / 180

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float mPositionX, mPositionY, mPositionZ;
	float mRotationX, mRotationY, mRotationZ;

	XMMATRIX mViewMatrix;
};

