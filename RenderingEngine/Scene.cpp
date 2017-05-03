#include "Scene.h"

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

void Scene::Start(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{

	mContent = new ContentManager();

	//mMeshes = mContent->LoadFBX(md3dDevice, "res/fbx/light_demo.fbx", mNumMeshes);
	size_t temp;
	mNumMeshes = 13;
	mMeshes = new Mesh[mNumMeshes];
	mMeshes[0] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_skap.FBX", temp);
	mMeshes[1] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_S_ba.FBX", temp);
	mMeshes[2] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_St_kp.FBX", temp);
	mMeshes[3] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_STUB.FBX", temp);
	mMeshes[4] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_KAMEN.FBX", temp);
	mMeshes[5] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_KAMEN-stup.FBX", temp);
	mMeshes[6] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_PROZOR.FBX", temp);
	mMeshes[7] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_reljef.FBX", temp);
	mMeshes[8] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_Sp_luk.FBX", temp);
	mMeshes[9] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_vrata_ko.FBX", temp);
	mMeshes[10] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_vrata_kr.FBX", temp);
	mMeshes[11] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_x01_st.FBX", temp);
	mMeshes[12] = *mContent->LoadFBX(device, "res/fbx/sponza/sponza_UNKNOWN.FBX", temp);

	mNumTextures = 13;
	mTextures = new Texture[mNumTextures];

	mTextures[0] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/00_skap.tga");
	mTextures[1] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/01_S_ba.tga");
	mTextures[2] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/01_St_kp.tga");
	mTextures[3] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/01_STUB.tga");
	mTextures[4] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/KAMEN.tga");
	mTextures[5] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/KAMEN-stup.tga");
	mTextures[6] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/prozor1.tga");
	mTextures[7] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/reljef.tga");
	mTextures[8] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/sp_luk.tga");
	mTextures[9] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/vrata_ko.tga");
	mTextures[10] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/vrata_kr.tga");
	mTextures[11] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/x01_st.tga");
	mTextures[12] = *mContent->LoadTGA(device, deviceContext, "res/tga/sponza/KAMEN-stup.tga");

	for (size_t i = 0; i < mNumMeshes; i++)
	{
		// Material we will use for that mesh
		Material *tempMat = new Material();

		// Texture to referance in the material
		tempMat->texDiffuse = &mTextures[i];

		// Not bothering to set individual values for each mesh
		tempMat->colEmmissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		tempMat->colSpecular = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);

		mMeshes[i].SetMaterial(tempMat);
	}

	mMainCamera = new Camera();

	// Set up the camera's projection
	mMainCamera->CreateProjection(XM_PI / 2.0f, (float)(mEngine->mClientWidth) / (float)(mEngine->GetScreenHeight), 0.1f, 1000.0f);

	XMFLOAT4 targetXMFloat = XMFLOAT4(0.0f, 4.0f, 2.0f, 1.0f);

	XMVECTOR pos = XMVectorSet(-7.0f, 1.8f, -2.0f, 1.0f);
	XMVECTOR target = XMLoadFloat4(&targetXMFloat);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX  V = XMMatrixLookAtLH(pos, target, up);
	mMainCamera->SetViewMatrix(V);

	mDirLight = new DirectionalLight();
	mDirLight->Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mDirLight->Intensity = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLight->Direction = XMFLOAT3(0.5773f, -0.5773f, 0.5773f);

	mPointLight = new PointLight();
	mPointLight->Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mPointLight->Intensity = XMFLOAT4(1.0f, 0.8f, 0.8f, 1.0f);
	mPointLight->Position = XMFLOAT3(0.0f, 2.0f, 0.0f);
	mPointLight->Range = 10.0f;
	mPointLight->Attenuation = XMFLOAT3(0.0f, 0.0f, 0.5f);

	mSpotLight = new SpotLight();
	//mSpotLight->Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//mSpotLight->Intensity = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//mSpotLight->Position = XMFLOAT3(-4.0f, 100.0f, -90.0f);
	//mSpotLight->Range = 150.0f;
	//mSpotLight->Direction = XMFLOAT3(0.36f, -0.78f, 0.51f);
	//mSpotLight->Spot = 15.0f;
	//mSpotLight->Attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

void Scene::Update(const GameTimer & gt)
{
}

void Scene::Draw(ID3D11DeviceContext * deviceContext)
{
	XMMATRIX world = XMMatrixIdentity(), view = XMMatrixIdentity(), projection = XMMatrixIdentity(), worldViewProj = XMMatrixIdentity(), worldInvTrans = XMMatrixIdentity();

	mEngine->mLitShader->UpdateFrame(deviceContext, mDirLight, mPointLight, mSpotLight,
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
