#pragma once

#include "Mesh.h"
#include "GameTimer.h"

// doubly linked tree?
class SceneObject
{
private:
	SceneObject *Parent;
	SceneObject *Children;

	Mesh *mMesh;
public:
	SceneObject();
	~SceneObject();

	void Start();
	void Update(const GameTimer &gt);
	void Draw(ID3D11DeviceContext *deviceContext);
};

