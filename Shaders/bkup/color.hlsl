#include "Lighting.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
}
cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
	float4 gAmbientLight;
	Light gLights[MaxLights];
};


struct VertexIn
{
	float3 PosL  : POSITION;
    float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut) 0.0f;
	
	// Transform to homogeneous clip space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;
	
	//Assumes non-uniform scaling; otherwise, need to use inverse-transpose of world matrix;
	vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
	 
	vout.PosH = mul(posW, gViewProj);
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	pin.NormalW = normalize(pin.NormalW);
	
	float3 toEyeW = normalize(gEyePosW - pin.PosW);	
	
	//Indirect ligthing
	float4 ambient = gAmbientLight * gDiffuseAlbedo;
	
	//Direct Lighting
	const float shininess = 1.0f - gRoughness;
	Material  mat = { gDiffuseAlbedo, gFresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW);
	float4 litColor = ambient + directLight;
	litColor.a = gDiffuseAlbedo.a;

    return litColor;
}
