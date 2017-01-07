struct AmbientLight
{
	float4 Ambient;
};

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
	AmbientLight gAmbientLight;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, gWorldViewProj);

	output.normal = input.normal;
	output.color = input.color;

	return output;
}