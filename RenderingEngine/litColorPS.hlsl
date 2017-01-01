struct AmbientLight
{
	float4 Ambient;
};

//struct DirectonalLight
//{
//	float4 Ambient;
//	float4 Diffuse;
//	float4 Specular;
//	float3 Direction;
//	float pad;
//};

cbuffer cbPerFrame
{
	AmbientLight gAmbientLight;
	//DirectionalLight gDirLight;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 output;

	output = mul(input.color, gAmbientLight.Ambient);

	return output;
}