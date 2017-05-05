#include "NoobieEngine.h"



NoobieEngine::NoobieEngine()
{
	mGraphics = nullptr;
	mContent = nullptr;
}


NoobieEngine::~NoobieEngine()
{
	delete mContent;
	mContent = nullptr;

	delete mGraphics;
	mGraphics = nullptr;
}

D3D11Graphics * NoobieEngine::GetGraphics() const
{
	return mGraphics;
}

GameTimer NoobieEngine::GetTimer() const
{
	return mTimer;
}

bool NoobieEngine::Init(HWND hwnd, int width, int height)
{
	mGraphics = new D3D11Graphics();
	if (!mGraphics->Init(hwnd, width, height))
		return false;

	mContent = new ContentManager();

	mTimer = GameTimer();
	mTimer.Reset();

	return true;
}

void NoobieEngine::OnResize(HWND hwnd, int width, int height)
{
	mGraphics->OnResize(hwnd, width, height);
	mMainCamera->ResizeAspectRatio((float)width / (float)height);
}

void NoobieEngine::Start()
{

	mContent = new ContentManager();
	// Initialize Scene Objects Here

	mMeshes = mContent->LoadFBX(mGraphics->GetDevice(), "res/fbx/cratepallet_01.FBX", mNumMeshes);

	mNumTextures = 1;
	mTextures = new Texture[mNumTextures];

	mTextures[0] = *mContent->LoadTGA(mGraphics->GetDevice(), mGraphics->GetImmediateContext(), "res/tga/cratepallet_01_alb.tga");

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
	mMainCamera->CreateProjection(XM_PI / 2.0f, (float)(mGraphics->GetScreenWidth()) / (float)(mGraphics->GetScreenHeight()), 0.1f, 1000.0f);

	XMFLOAT4 targetXMFloat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR pos = XMVectorSet(-2.0f, 2.0f, -2.0f, 1.0f);
	XMVECTOR target = XMLoadFloat4(&targetXMFloat);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX  V = XMMatrixLookAtLH(pos, target, up);
	mMainCamera->SetViewMatrix(V);

	mDirLight = new DirectionalLight();
	mDirLight->Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
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

void NoobieEngine::Update()
{
	// Run Updates Here
	mTimer.Tick();
}

void NoobieEngine::Draw()
{
	mGraphics->DrawBegin();

	XMMATRIX world = XMMatrixIdentity(), view = XMMatrixIdentity(), projection = XMMatrixIdentity(), worldViewProj = XMMatrixIdentity(), worldInvTrans = XMMatrixIdentity();

	mGraphics->mLitShader->UpdateFrame(mGraphics->GetImmediateContext(), mDirLight, mPointLight, mSpotLight,
		mMainCamera->GetWorldPosition());

	mMainCamera->GetViewMatrix(view);
	mMainCamera->GetProjectionMatrix(projection);

	// when using multiple, render all meshes with material together. faster.

	for (size_t i = 0; i < mNumMeshes; i++)
	{
		mGraphics->mLitShader->UpdateMaterial(mGraphics->GetImmediateContext(), mMeshes[i].GetMaterial());

		mMeshes[i].Render(mGraphics->GetImmediateContext());
		mMeshes[i].GetWorldMatrix(world);

		// DONT FORGET TO DO THIS! YOU MORON
		worldViewProj = world * view * projection;
		worldInvTrans = XMMatrixInverse(NULL, world);
		worldInvTrans = XMMatrixTranspose(worldInvTrans);

		mGraphics->mLitShader->Render(mGraphics->GetImmediateContext(), mMeshes[i].GetIndexCount(), world, worldViewProj, worldInvTrans);
	}

	mGraphics->DrawEnd();
}
