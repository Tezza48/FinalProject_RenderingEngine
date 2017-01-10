#include "litColor.hlsli"
#include "LightsHelper.hlsli"

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gWorldViewProj;
	AmbientLight gAmbientLight;
	DirectionalLight gDirLight;
	float3 gEyePosW;
	Material gMat;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float4x4 worldInverseTranspose = transpose(gWorld);

	//output.positionH = float4(input.positionL, 1.0f);

	output.positionH = mul(float4(input.positionL, 1.0f), gWorldViewProj);

	output.positionW = mul(float4(input.positionL, 1.0f), gWorld).xyz;

	output.normalW = mul(input.normalL, (float3x3)gWorldInvTrans);
	output.normalW = normalize(output.normalW);
	//output.color = input.color;

	return output;
}