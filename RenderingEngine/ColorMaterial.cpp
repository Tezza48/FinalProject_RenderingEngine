#include "ColorMaterial.h"

ColorMaterial::ColorMaterial()
{

}

ColorMaterial::ColorMaterial(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular)
{
	Ambient = ambient;
	Diffuse = diffuse;
	Specular = specular;
}

ColorMaterial::~ColorMaterial()
{

}
