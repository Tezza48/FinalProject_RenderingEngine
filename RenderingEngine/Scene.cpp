#include "Scene.h"

#ifdef DONOTCOMPILE

Scene::Scene()
{
	mContent = nullptr;

	mMeshes = nullptr;

	mTextures = nullptr;

	mMainCamera = nullptr;

	mDirLight = nullptr;
	mPointLight = nullptr;
	mSpotLight = nullptr;
}


Scene::~Scene()
{
	delete mContent;
	mContent = nullptr;

	delete[] mMeshes;
	mMeshes = nullptr;

	delete[] mTextures;
	mTextures = nullptr;

	delete mMainCamera;
	mMainCamera = nullptr;

	delete mDirLight;
	mDirLight = nullptr;

	delete mPointLight;
	mPointLight = nullptr;

	delete mSpotLight;
	mSpotLight = nullptr;
}

void Scene::Start()
{

}

void Scene::Update(const GameTimer & gt)
{
}

void Scene::Draw(ID3D11DeviceContext * deviceContext)
{
	XMMATRIX world = XMMatrixIdentity(), view = XMMatrixIdentity(), projection = XMMatrixIdentity(), worldViewProj = XMMatrixIdentity(), worldInvTrans = XMMatrixIdentity();

	mGraphics->mLitShader->UpdateFrame(deviceContext, mDirLight, mPointLight, mSpotLight,
		mMainCamera->GetWorldPosition());

	mMainCamera->GetViewMatrix(view);
	mMainCamera->GetProjectionMatrix(projection);

	// when using multiple, render all meshes with material together. faster.

	for (size_t i = 0; i < mNumMeshes; i++)
	{
		mEngine->mLitShader->UpdateMaterial(deviceContext, mMeshes[i].GetMaterial());

		mMeshes[i].Render(deviceContext);
		mMeshes[i].GetWorldMatrix(world);

		// DONT FORGET TO DO THIS! YOU MORON
		worldViewProj = world * view * projection;
		worldInvTrans = XMMatrixInverse(NULL, world);
		worldInvTrans = XMMatrixTranspose(worldInvTrans);

		mEngine->mLitShader->Render(deviceContext, mMeshes[i].GetIndexCount(), world, worldViewProj, worldInvTrans);
	}
}
#endif