cbuffer cnPerObject
{
	float4x4 gWorldViewProj;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInputType main(VertexInputType input )
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, gWorldViewProj);
	output.color = input.color;

	return output;
}