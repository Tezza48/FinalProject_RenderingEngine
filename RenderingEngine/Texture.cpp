#include "Texture.h"



Texture::Texture()
{
	mTexture = nullptr;
	mTextureView = nullptr;
}


Texture::~Texture()
{
	if (mTextureView)
	{
		mTextureView->Release();
		mTextureView = nullptr;
	}

	if (mTexture)
	{
		mTexture->Release();
		mTexture = nullptr;
	}	
}

void Texture::Init(ID3D11Device *device, ID3D11DeviceContext *deviceContext, char *imageData, size_t width, size_t height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, NULL, &mTexture));

	unsigned int rowPitch = (width * 4) * sizeof(char);

	deviceContext->UpdateSubresource(mTexture, 0, NULL, imageData, rowPitch, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	DX::ThrowIfFailed(device->CreateShaderResourceView(mTexture, &srvDesc, &mTextureView));

	deviceContext->GenerateMips(mTextureView);
}

void Texture::Init(ID3D11Device * device, char * filename)
{
	D3DX11CreateShaderResourceViewFromFile(device, L"res/dds/crate1_diffuse.dds", 0, 0, &mTextureView, 0);
}

ID3D11ShaderResourceView * Texture::GetSRV()
{
	return mTextureView;
}
