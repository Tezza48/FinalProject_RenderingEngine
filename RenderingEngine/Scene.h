#pragma once

#include <D3D11.h>
#include "Camera.h"
#include "ContentManager.h"
#include "GameTimer.h"
#include "LightHelper.h"
#include "SceneObject.h"

class Scene
{
private:
	SceneObject *mRoot;

	size_t mNumMeshes;
	Mesh *mMeshes; // an array of meshes;

	size_t mNumTextures;
	Texture *mTextures;

	DirectionalLight *mDirLight;
	PointLight *mPointLight;
	SpotLight *mSpotLight;

	ContentManager *mContent;

	class D3D11App *mEngine;

	// Matrices we use when rendering

public:
	// Main Camera (only camera)
	Camera *mMainCamera;

public:
	Scene();
	~Scene();

	void Start();
	void Update(const GameTimer &gt);
	void Draw(ID3D11DeviceContext *deviceContext);
};
