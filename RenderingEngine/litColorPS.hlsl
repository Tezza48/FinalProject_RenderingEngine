struct AmbientLight
{
	float4 Ambient;
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
};

cbuffer cbPerFrame
{
	AmbientLight gAmbientLight;
	DirectionalLight gDirLight;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 output;

	output = input.color;

	// ambient
	output = input.color * gAmbientLight.Ambient;


	return output;
}