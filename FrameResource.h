#pragma once
#include "Cmn3DApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include <memory>
#include <vector>
#include <array>

#define MaxLights 10

struct Light
{
	DirectX::XMFLOAT3 Strength;
	float falloffStart;
	DirectX::XMFLOAT3 Direction;
	float falloffEnd;
	DirectX::XMFLOAT3 Position;
	float spotPower;
	float lightType;
};

struct Material
{
	std::string Name;
	int MatCBIndex = -1;

	int NumFrameDirty ;

	//Material Constant Buffer data
	DirectX::XMFLOAT4 DiffuseAlbedo = { 0.5f, 0.5f, 0.5f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.1f, 0.51f, 0.1f };
	float Roughness = 0.1f;	
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct ObjectContants
{
	DirectX::XMFLOAT4X4 word = MathHelper::Identity4x4();
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.89f, 0.84f, 0.72f, 1.0f };

	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light Lights[MaxLights];
};


struct MaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo;
	DirectX::XMFLOAT3 FresnelR0;
	float Roughness ;

	// Used in texture mapping.
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
};


class FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passcount, UINT objectCount, UINT materialCount, UINT VerticesCount, UINT IndicesCount);
	~FrameResource();

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdListAllocator;
	std::unique_ptr<UploadBuffer<PassConstants>> passCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectContants>> perObjectCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> materialCB = nullptr;

	std::unique_ptr<UploadBuffer<Vertex>> vertices = nullptr;
	std::unique_ptr<UploadBuffer<UINT32>> indices = nullptr;

	UINT64 fence = 0;
};

