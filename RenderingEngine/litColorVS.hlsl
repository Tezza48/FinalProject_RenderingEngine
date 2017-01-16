#include "litColor.hlsli"
#include "LightHelper.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gWorldViewProj;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float4x4 worldInverseTranspose = transpose(gWorld);

	output.positionH = mul(float4(input.positionL, 1.0f), gWorldViewProj);

	output.positionW = mul(float4(input.positionL, 1.0f), gWorld).xyz;

	output.normalW = mul(input.normalL, (float3x3)gWorldInvTrans);
	output.normalW = normalize(output.normalW);
	output.tex = input.tex;
	output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return output;
}