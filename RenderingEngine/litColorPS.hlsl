struct AmbientLight
{
	float4 Ambient;
};

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
	AmbientLight gAmbientLight;
};

//struct DirectonalLight
//{
//	float4 Ambient;
//	float4 Diffuse;
//	float4 Specular;
//	float3 Direction;
//	float pad;
//};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 output;output = input.color;

	output = input.color * gAmbientLight.Ambient;

	return output;
}