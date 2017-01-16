#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include "utils.h"

class Texture
{
private:
	ID3D11Texture2D *mTexture;
	ID3D11ShaderResourceView *mTextureView;
public:
	Texture();
	~Texture();

	void Init(ID3D11Device *device, ID3D11DeviceContext *deviceContext, char *imageData, size_t width, size_t height);
	void Init(ID3D11Device *device, char *filename);

	ID3D11ShaderResourceView *GetSRV();
};

