#pragma once
#include "Cmn3DApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include <memory>
#include <vector>
#include <array>
#include "FrameResource.h"
#include <unordered_map>


static const int gNumFrameResourcesCount = 3;
static const int VerticesCount = 100002; // tmp decision

struct SubMesh
{
	UINT IndexCount = 0;
	UINT VertextCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
};

struct MeshGeometry
{
public:
	std::string Name;

	//ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	//ComPtr<ID3DBlob> VertexBufferCPU = nullptr;

	ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;


	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;
	UINT IndexBufferByteSize = 0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R32_UINT;

	std::unordered_map<std::string, SubMesh> DrawArgs;	

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertexByteStride;
		vbv.SizeInBytes = vertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW indexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = indexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};


struct RenderItem
{
	DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
	int numDirtyCB = gNumFrameResourcesCount; // dirty flag for perConstaBuffer	
	int numDirtyVI = gNumFrameResourcesCount; // dirty flag for Vertices/Indices
	
	UINT objCBIndex = 1;
	
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	MeshGeometry* Geo = nullptr;
	Material* Mat = nullptr;

	UINT IndexCount = 0;
	UINT VertexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
		
};

class myAppClass : public Cmn3DApp
{
public:
	myAppClass(HINSTANCE hInstance);
	~myAppClass();

	virtual void Initialize()override;
	virtual void Draw() override;
	virtual void Update() override;
	virtual void MoveObj(int Sig) override;	
		
private:	
	int flag = 0;
	bool m_MouseDown = false;
	POINT m_MouseDownPoint;
	ComPtr<ID3D12PipelineState> m_pso;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ComPtr<ID3D12Resource> m_UploadConstBuffer;
	ComPtr<ID3D12RootSignature> m_RootSignature;
	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;	
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
	std::unique_ptr< MeshGeometry> m_box = nullptr; 
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometry;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::vector<CPULight> mLights;

	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;

	std::vector<RenderItem> m_AllRenderItems;
	
	std::vector<std::unique_ptr<FrameResource>> m_FrameResources;
	FrameResource* m_CurrentFrameResource = nullptr;
	int m_iCurrentResourceIndex = 0;

	float mRadius;
	float mPhi;
	float mTheta;

	float mRadiusCamera;
	float mPhiCamera;
	float mThetaCamera;
	
	int moveLR = 0;
	int moveUD = 0;	
	int moveFB = 0;
	
	int moveLightLR = 0;
	int moveLightFB = 0;
	int moveLightUD = 0;
	int lightRotateLR = 0;
	int lightRotateUD = 0;
	int lightScaleRadius = 0;
	int lightScaleSpotRadius = 0;
	bool lightTurnOnOff = 0;

	bool keyPressedCtrl = 0;
	
	DirectX::XMFLOAT3 mEyePos;// = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mView;// = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj;// = MathHelper::Identity4x4();

	float pi = 3.1415926;
	size_t lightCount = 0;
	DirectX::XMFLOAT3 TargetPos;

	BYTE* m_ConstBufferCPUAddress = nullptr;

	virtual void onMouseDown(WPARAM btnState, int x, int y) override;
	virtual void onMouseUp(WPARAM btnState, int x, int y) override;
	virtual void onMouseMove(WPARAM btnState, int x, int y) override;	
	virtual void onKeyDown(WPARAM btnState) override;
	virtual void onKeyUp(WPARAM btnState) override;

	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<RenderItem>* rItems);

	void BuildDescriptorHeaps();
	void BuildFrameResourcse();
	void BuildConstantBuffer();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildGeometry();
	void BuildMaterial();
	void BuildRenderItems();
	void BuildLight(int lightType, int renderItemID);
	void BuildPSO();
	
	void UpdateCamera();
	void UpdateGeometry();
	void UpdateLight();
	void UpdateLightToRenderIntem();
	void UpdateLightToPassCB();
	void UpdateCB();
	void UpdateMaterialCB();
	void UpdatePassCB();
	
	
	void InitLight();

	//Helper Functions
	ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
	ComPtr<ID3D12Resource> CreateDeafultBuffer(const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer) ;
	void CreateConstGeometry(std::string name, const void* vertData, const void* indexData, int vertexSize, int indexSize, int vertexCount, int indexCount);
	static UINT myAppClass::CalcConstantBufferByteSize(UINT byteSize)
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}
};



