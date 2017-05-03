#include "SceneObject.h"



SceneObject::SceneObject()
{
	Parent = nullptr;
	Children = nullptr;
}


SceneObject::~SceneObject()
{
}

void SceneObject::Start()
{
}

void SceneObject::Update(const GameTimer & gt)
{
}

void SceneObject::Draw(ID3D11DeviceContext * deviceContext)
{
}
