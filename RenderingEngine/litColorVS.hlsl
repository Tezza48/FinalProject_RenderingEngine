struct AmbientLight
{
	float4 Ambient;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
	//AmbientLight gAmbientLight;
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

	output.position = mul(input.position, gWorld);
	output.position = mul(output.position, gView);
	output.position = mul(output.position, gProj);

	output.normal = input.normal;
	output.color = input.color;

	return output;
}