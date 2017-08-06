#include "myAppClass.h"
#include "ModelLoaderClass.h"

using namespace DirectX;

struct ObjectContants
{
	XMFLOAT4X4 wordViewProj = MathHelper::Identity4x4();
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};


myAppClass::myAppClass(HINSTANCE hInstance):Cmn3DApp(hInstance)
{
}


myAppClass::~myAppClass()
{
	if (m_UploadConstBuffer != nullptr)	m_UploadConstBuffer->Unmap(0, nullptr);
	m_ConstBufferCPUAddress = nullptr;

}

void myAppClass::onMouseDown(WPARAM btnState, int x, int y)
{
	//MessageBox(0, L"Privet :) ", L"Message", 0);
}

void myAppClass::Initialize()
{
	Cmn3DApp::Initialize();
	
	mPhi = 5.0f;
	mTheta = 1.5f*3.14f;
	mRadius = 10.0f;

	ThrowIfFailed(m_CmdList->Reset(m_CmdAllocator.Get(), nullptr));

	BuildDescriptorHeaps();
	BuildConstantBuffer();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();

	ThrowIfFailed(m_CmdList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, cmdsLists);

	FlushCommandQueue();	
}

void myAppClass::MoveObj(int Sig)
{
	if (Sig == -1) mTheta -= XMConvertToRadians(1.1f);
	else mTheta += XMConvertToRadians(1.1f);
}

void myAppClass::Draw()
{
	if (ModeFlag != prevModeFlag)
	{
		BuildPSO();
		prevModeFlag = ModeFlag;
	}
	
	ThrowIfFailed(m_CmdAllocator->Reset());
	ThrowIfFailed(m_CmdList->Reset(m_CmdAllocator.Get(), m_pso.Get()));

	m_CmdList->RSSetScissorRects(1, &m_ScissorRect);
	m_CmdList->RSSetViewports(1, &m_ScreenViewport);

	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChainBuffers[m_swapChain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	FLOAT mycolor[4] = { 0.5f, 0.5f, 0.5f, 0.1f };
	m_CmdList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_swapChain->GetCurrentBackBufferIndex(), m_rtvDescriptorSize), mycolor,0,nullptr);
	m_CmdList->ClearDepthStencilView(dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_CmdList->OMSetRenderTargets(1, &CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_swapChain->GetCurrentBackBufferIndex(), m_rtvDescriptorSize), true, &dsvHeap->GetCPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	m_CmdList->SetDescriptorHeaps(1, descriptorHeaps);

	m_CmdList->SetGraphicsRootSignature(m_RootSignature.Get());

	m_CmdList->IASetVertexBuffers(0, 1, &m_box->vertexBufferView());
	//m_CmdList->IASetIndexBuffer(&m_box->indexBufferView());
	m_CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	//m_CmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//m_CmdList->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	m_CmdList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

	//m_CmdList->DrawIndexedInstanced(m_box->box.IndexCount, 1, 0, 0, 0);
	m_CmdList->DrawInstanced(m_box->box.VertextCount, 1, 0, 0);
	
	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChainBuffers[m_swapChain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	
	ThrowIfFailed(m_CmdList->Close());

	ID3D12CommandList* CmdLists[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, CmdLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));
	FlushCommandQueue();
}

void myAppClass::Update()
{		
	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj  = MathHelper::Identity4x4();

	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*3.1415f, AspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	ObjectContants objConstants;
	XMStoreFloat4x4(&objConstants.wordViewProj, XMMatrixTranspose(worldViewProj));

	memcpy(&m_ConstBufferCPUAddress[0], &objConstants, sizeof(ObjectContants));
	int c43 = 2;
}

void myAppClass::BuildDescriptorHeaps()
{
	//Create Descriptor Heap for Constan buffer View
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
}

void myAppClass::BuildConstantBuffer()
{
	/*
		1) - Create Committed resource in Upload Heap
		2) - Map this GPU resource to CPU address
		3) - Create View on this resource.
		4) - When it is required, just copy data to this resource
	*/

	// Create committed resource for constant buffer
	int mElementByteSize = CalcConstantBufferByteSize(sizeof(ObjectContants));
	{		
		//Create resource for Constant Buffer
		ThrowIfFailed(m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_UploadConstBuffer)));

		ThrowIfFailed(m_UploadConstBuffer->Map(0, nullptr, reinterpret_cast<void**> (&m_ConstBufferCPUAddress)));
	}

	//Create Constant Buffer View
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbGPUAddress = m_UploadConstBuffer->GetGPUVirtualAddress();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = cbGPUAddress;
		cbvDesc.SizeInBytes = mElementByteSize;

		m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

void myAppClass::BuildRootSignature()
{
	
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> error = nullptr;
	/*
	D3D12_DESCRIPTOR_RANGE cbRange = {};
	cbRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbRange.NumDescriptors = 1;
	cbRange.BaseShaderRegister = 0;
	cbRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParam = {};	
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParam.DescriptorTable = { 1, &cbRange };
		
	D3D12_ROOT_SIGNATURE_DESC rootDescr = {};
	rootDescr.NumParameters = 1;
	rootDescr.pParameters = { &rootParam };
	rootDescr.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ThrowIfFailed(D3D12SerializeRootSignature(&rootDescr, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializedRootSig, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
	*/

	CD3DX12_ROOT_PARAMETER slotParametr[1];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotParametr[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootDecs(1, slotParametr, 0, nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ThrowIfFailed(D3D12SerializeRootSignature(&rootDecs, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));


}

void myAppClass::BuildShadersAndInputLayout()
{
	m_vsByteCode = CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	m_InputLayout = 
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};
}

void myAppClass::BuildBoxGeometry()
{
	// Load model from file 
	ModelLoaderClass ModelLoader;
	ModelLoader.LoadModelFromFile(L"PlainModel.obj");

	int Count = ModelLoader.GetVectorSizeV();
		
	std::vector<Vertex> vertices;
	ModelLoader.SetToBeginV();
	for (int i = 0; i < Count; i++)
	{
		VertexModelLoader tmpVert = ModelLoader.GetNextV();
		vertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y, tmpVert.z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }));
	}
	

	/*
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)}),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) })
	};
	*/

	std::array<std::uint16_t, 36> indices =
	{// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7 
	};
	
	const UINT vbByteSize = (UINT) vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);
	
	m_box = std::make_unique<MeshGeometry>();
	
	m_box->VertexBufferGPU = CreateDeafultBuffer(vertices.data(), vbByteSize, m_box->VertexBufferUploader);
	m_box->IndexBufferGPU = CreateDeafultBuffer(indices.data(), ibByteSize, m_box->IndexBufferUploader);

	m_box->vertexByteStride = sizeof(Vertex);
	m_box->vertexBufferByteSize = vbByteSize;
	m_box->indexFormat = DXGI_FORMAT_R16_UINT;
	m_box->IndexBufferByteSize = ibByteSize;

	m_box->box.IndexCount = (UINT)indices.size();
	m_box->box.VertextCount = (UINT)Count;
	m_box->box.StartIndexLocation = 0;
	m_box->box.BaseVertexLocation = 0;
}

void myAppClass::BuildPSO()
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()), m_vsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()), m_psByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	if (!ModeFlag)
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	else 
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;


	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;// D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_BackBufferFormat;
	psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.SampleDesc.Count = 1;
	
	//ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(&m_pso)));
	HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));


}


ComPtr<ID3DBlob> myAppClass::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	UINT compileflag = 0;
#if defined(DEBUG ) || defined(_DEBUG)
	compileflag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;	
#endif
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3D10Blob> errors;

	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileflag,0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());
	ThrowIfFailed(hr);

	return byteCode;
}


ComPtr<ID3D12Resource> myAppClass::CreateDeafultBuffer(const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> defaultBuffer;

	//create the actual default buffer resource
	ThrowIfFailed(m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), 
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&defaultBuffer)));

	// To copy CPU memory to Default buffer, we need to create an intermediate upload heap

	HRESULT hr = m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf()));
	//ThrowIfFailed();

	//describe the data we want to copy into default buffer
	D3D12_SUBRESOURCE_DATA subResource = {};
	subResource.pData = initData;
	subResource.RowPitch = byteSize;
	subResource.SlicePitch = byteSize;

	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	
	UpdateSubresources<1>(m_CmdList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResource);

	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	return defaultBuffer;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	try
	{			
		myAppClass commonApp(hInstance);
		commonApp.Initialize();

	    return commonApp.Run();
	
	}
	catch (const std::exception&)
	{
		MessageBox(nullptr, L"Exception error", L"HR Failed", MB_OK);
		return 0;
	}
	
}