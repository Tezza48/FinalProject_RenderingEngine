struct VertexInputType
{
	float3 positionL : POSITION;//local position
	float3 normalL : NORMAL;//local Normal
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 positionH : SV_POSITION;//Homogenous Clip position
	float3 positionW : POSITION;//World Position
	float3 normalW : NORMAL;//World Normal
	float4 color : COLOR;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};