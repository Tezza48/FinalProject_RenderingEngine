#pragma once

#include "ContentManager.h"
#include "D3D11Graphics.h"
#include "GameTimer.h"

#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"

class NoobieEngine
{
	// Members
	// Needed to run program
private:
	D3D11Graphics *mGraphics;
	ContentManager *mContent;
	GameTimer mTimer;
protected:
public:
	// Stuff that's going to change
private:
	int mNumMeshes;
	Mesh *mMeshes;

	int mNumTextures;
	Texture *mTextures;

	DirectionalLight *mDirLight;
	PointLight *mPointLight;
	SpotLight *mSpotLight;

	Camera *mMainCamera;

	// Methods
private:
protected:
public:
	NoobieEngine();
	~NoobieEngine();

	D3D11Graphics *GetGraphics() const;
	GameTimer GetTimer() const;

	bool Init(HWND hwnd);
	bool Run();

	void Start();
	void Update();
	void Draw();
};

