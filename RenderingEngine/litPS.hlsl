#include "lit.hlsli"
#include "LightHelper.hlsli"

Texture2D gTexAlbedo;
SamplerState gSampleType;

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gWorldViewProj;
};

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float4 gEyePosW;
};

cbuffer cbPerMaterial : register(b2)
{
	Material gMat;
}

void ComputeLightDirectional(DirectionalLight light, float specExponent, float3 pos, float3 nrm, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -light.Direction;

	ambient = light.Ambient;

	float diffuseFactor = dot(lightVec, nrm);// dot means angle between

	[flatten] // i dont understand what dynamic branching is
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, nrm);
		float specFactor = pow(max(dot(v, toEye), 0.0f), specExponent) * 255.0f;

		diffuse = diffuseFactor * light.Intensity;
		specular = specFactor * light.Intensity;
	}
}

void ComputeLightPoint(PointLight light, float specExponent, float3 pos, float3 nrm, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = light.Position - pos;

	float d = length(lightVec);

	if (d > light.Range)
		return;

	lightVec /= d;

	ambient = light.Ambient;

	float diffuseFactor = dot(lightVec, nrm);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, nrm);
		float specFactor = pow(max(dot(v, toEye), 0.0f), specExponent);

		diffuse = diffuseFactor * light.Intensity;
		specular = specFactor * light.Intensity;
	}

	float att = 255.0f / dot(light.Attenuation, float3(1.0f, d, d*d));

	diffuse *= att;
	specular *= att;

}

void ComputeLightSpot(SpotLight light, float specExponent, float3 pos, float3 nrm, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = light.Position - pos;

	float d = length(lightVec);

	if (d > light.Range)
		return;

	lightVec /= d;

	ambient = light.Ambient;

	float diffuseFactor = dot(lightVec, nrm) * 255.0f;

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, nrm);
		float specFactor = pow(max(dot(v, toEye), 0.0f), specExponent) * 255.0f;

		diffuse = diffuseFactor * light.Intensity;
		specular = specFactor * light.Intensity;
	}

	float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);

	float att = spot / dot(light.Attenuation, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	specular *= att;
}

float4 main(PixelInputType input) : SV_TARGET
{
	float3 toEye = normalize(gEyePosW - input.positionW).xyz;

	float4 diffuseTexture = gTexAlbedo.Sample(gSampleType, input.tex);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 a, d, s; //ambient, diffuse, specular components

	ComputeLightDirectional(gDirLight, gMat.Specular.w, input.positionW, input.normalW, toEye, a, d, s);
	ambient += a;
	diffuse += d;
	specular += s;

	ComputeLightPoint(gPointLight, gMat.Specular.w, input.positionW, input.normalW, toEye, a, d, s);
	ambient += a;
	diffuse += d;
	specular += s;

	ComputeLightSpot(gSpotLight, gMat.Specular.w, input.positionW, input.normalW, toEye, a, d, s);
	ambient += a;
	diffuse += d;
	specular += s;

	float4 litColor = ambient + diffuse + specular + gMat.Emmissive;

	litColor.a = 1.0f;

	//return litColor;
	return diffuseTexture * litColor;
}