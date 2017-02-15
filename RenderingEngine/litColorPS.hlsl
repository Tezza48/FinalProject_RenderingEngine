#include "litColor.hlsli"
#include "LightHelper.hlsli"

Texture2D gTexture;
SamplerState gSampleType;

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gWorldViewProj;
};

cbuffer cbPerFrame : register(b1)
{
	AmbientLight gAmbientLight;
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	Material gMat;
	float4 gEyePosW;
};

//cbuffer cbPerFrame
//{
//};

void ComputeLightAmbient(AmbientLight light, Material mat, out float4 ambient)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ambient = mat.Ambient * light.Ambient;
}

void ComputeLightDirectional(DirectionalLight light, Material mat, float3 pos, float3 nrm, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -light.Direction;

	ambient = mat.Ambient * light.Ambient;

	float diffuseFactor = dot(lightVec, nrm);// dot means angle between

	[flatten] // i dont understand what dynamic branching is
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, nrm);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Intensity;
		specular = specFactor * mat.Specular * light.Intensity;
	}
}

void ComputeLightPoint(PointLight light, Material mat, float3 pos, float3 nrm, float3 toEye,
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

	ambient = mat.Ambient * light.Ambient;

	float diffuseFactor = dot(lightVec, nrm);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, nrm);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Intensity;
		specular = specFactor * mat.Specular * light.Intensity;
	}

	float att = 1.0f / dot(light.Attenuation, float3(1.0f, d, d*d));

	diffuse *= att;
	specular *= att;

}

void ComputeLightSpot(SpotLight light, Material mat, float3 pos, float3 nrm, float3 toEye,
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

	ambient = mat.Ambient * light.Ambient;

	float diffuseFactor = dot(lightVec, nrm);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, nrm);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Intensity;
		specular = specFactor * mat.Specular * light.Intensity;
	}

	float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);

	float att = spot / dot(light.Attenuation, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	specular *= att;
}

float4 main(PixelInputType input) : SV_TARGET
{
	float3 toEye = normalize(gEyePosW - input.positionW);

	float4 diffuseTexture = gTexture.Sample(gSampleType, input.tex);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 a, d, s; //ambient, diffuse, specular components

	// ambient
	ComputeLightAmbient(gAmbientLight, gMat, a);
	ambient += a;

	ComputeLightDirectional(gDirLight, gMat, input.positionW, input.normalW, toEye, a, d, s);
	ambient += a;
	diffuse += d;
	specular += s;

	ComputeLightPoint(gPointLight, gMat, input.positionW, input.normalW, toEye, a, d, s);
	ambient += a;
	diffuse += d;
	specular += s;

	ComputeLightSpot(gSpotLight, gMat, input.positionW, input.normalW, toEye, a, d, s);
	ambient += a;
	diffuse += d;
	specular += s;

	float4 litColor = ambient + diffuse + specular;

	litColor.a = 1.0f;

	return litColor;
	//return diffuseTexture * litColor;
}