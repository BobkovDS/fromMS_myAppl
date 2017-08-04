#pragma once
#include "Cmn3DApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include <memory>
#include <vector>
#include <array>


struct SubMesh
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
};

struct MeshGeometry
{
public:
	std::string Name;

	ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;


	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;
	UINT IndexBufferByteSize = 0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;

	SubMesh box;

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
	
	ComPtr<ID3D12PipelineState> m_pso;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ComPtr<ID3D12Resource> m_UploadConstBuffer;
	ComPtr<ID3D12RootSignature> m_RootSignature;
	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;	
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
	std::unique_ptr< MeshGeometry> m_box = nullptr;
	float mRadius;
	float mPhi;
	float mTheta;
	

	BYTE* m_ConstBufferCPUAddress = nullptr;

	virtual void onMouseDown(WPARAM btnState, int x, int y) override;
	
	void BuildDescriptorHeaps();
	void BuildConstantBuffer();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();
	
	//Helper Functions
	ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
	ComPtr<ID3D12Resource> CreateDeafultBuffer(const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer) ;

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



