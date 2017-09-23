#include "myAppClass.h"
#include "ModelLoaderClass.h"

using namespace DirectX;


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
	m_MouseDown = true;
	m_MouseDownPoint.x = x;
	m_MouseDownPoint.y = y;
}
void myAppClass::onMouseUp(WPARAM btnState, int x, int y)
{
	m_MouseDown = false;
}
void myAppClass::onMouseMove(WPARAM btnState, int x, int y)
{
	if (m_MouseDown)
	{

		//if CTRL
		if (btnState == (MK_LBUTTON | MK_CONTROL))
		{
			if (y != m_MouseDownPoint.y) 
				mTheta += (y - m_MouseDownPoint.y)/abs(y - m_MouseDownPoint.y) *0.5f;
			
			if (x != m_MouseDownPoint.x)
				mPhi += (m_MouseDownPoint.x - x)/abs(m_MouseDownPoint.x - x)* 0.5f;
		
		}		
		else if (btnState == (MK_RBUTTON | MK_CONTROL))
		{
			if (x != m_MouseDownPoint.x)
				mRadius += (m_MouseDownPoint.x - x)/abs(m_MouseDownPoint.x - x)* 1.0f;
		}
		
		else if (btnState == MK_LBUTTON )
		{
			if (y != m_MouseDownPoint.y)
				mThetaCamera += (y - m_MouseDownPoint.y) / abs(y - m_MouseDownPoint.y) *0.5f;

			if (x != m_MouseDownPoint.x)
				mPhiCamera += -1*((m_MouseDownPoint.x - x) / abs(m_MouseDownPoint.x - x)* 0.5f);
			
		}
		m_MouseDownPoint.x = x;
		m_MouseDownPoint.y = y;
	}
}

void myAppClass::onKeyDown(WPARAM btnState)
{	
	switch (btnState)
	{
	//case 'A': moveLR = -1; return;
	//case 'D': moveLR = 1; return;
	case 'W': moveFB =1 ; return;
	case 'S': moveFB =-1 ; return;
	case VK_OEM_4: lightScaleSpotRadius = -1; return;
	case VK_OEM_6: lightScaleSpotRadius = 1; return;
	case VK_CONTROL: keyPressedCtrl = 1; return;
	case VK_NUMPAD8: lightRotateUD = 1; return;
	case VK_NUMPAD2: lightRotateUD = -1; return;
	case VK_NUMPAD4: lightRotateLR = 1; return;
	case VK_NUMPAD6: lightRotateLR = -1; return;
	case VK_NUMPAD5: lightTurnOnOff = 1; return;
		//move Light
	case VK_NUMPAD7: moveLightLR = 1; return;
	case VK_NUMPAD9: moveLightLR = -1; return;
	case VK_NUMPAD1: moveLightFB = 1; return;
	case VK_NUMPAD3: moveLightFB = -1; return;
	case VK_ADD:
	{
		if (keyPressedCtrl)
			lightScaleRadius = 1;
		else
			moveLightUD = 1;
	}
	return;
	case VK_SUBTRACT: 
	{
		if (keyPressedCtrl)
			lightScaleRadius = -1;
		else
			moveLightUD = -1;
	}
	return;
		
	}
}

void myAppClass::onKeyUp(WPARAM btnState)
{
	switch (btnState)
	{
	//case 'A': moveLR = 0; return;
	//case 'D': moveLR = 0; return;
	case 'W': moveFB = 0; return;
	case 'S': moveFB = 0; return;
	case VK_CONTROL: keyPressedCtrl = 0; return;
	case VK_NUMPAD8: lightRotateUD = 0; return;
	case VK_NUMPAD2: lightRotateUD = 0; return;
	case VK_NUMPAD4: lightRotateLR = 0; return;
	case VK_NUMPAD6: lightRotateLR = 0; return;
		//move Light
	case VK_NUMPAD7: moveLightLR = 0; return;
	case VK_NUMPAD9: moveLightLR = 0; return;
	case VK_NUMPAD1: moveLightFB = 0; return;
	case VK_NUMPAD3: moveLightFB = 0; return;
	case VK_ADD: lightScaleRadius  = moveLightUD = 0; return;
	case VK_SUBTRACT: lightScaleRadius = moveLightUD = 0; return;
	case VK_OEM_4: lightScaleSpotRadius = 0; return;
	case VK_OEM_6: lightScaleSpotRadius = 0; return;
	}
}

void myAppClass::Initialize()
{
	Cmn3DApp::Initialize();
	
	mPhi = 34;// 5.0f;
	mTheta = 78;// 1.5f*3.14f;
	mThetaCamera = 168;
	mPhiCamera = -323.5;
	mRadius = 69.5f;
	mRadiusCamera = mRadius;

	ThrowIfFailed(m_CmdList->Reset(m_CmdAllocator.Get(), nullptr));

	mEyePos.x = mRadius*sin(mTheta / 180 * pi)*sin(mPhi / 180 * pi);
	mEyePos.z = mRadius*sin(mTheta / 180 * pi)*cos(mPhi / 180 * pi);
	mEyePos.y = mRadius*cos(mTheta / 180 * pi);

	mEyePos.x = -11.13f;
	mEyePos.y = 109.46f;
	mEyePos.z = -17.621f;

	BuildDescriptorHeaps();
	BuildConstantBuffer();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildGeometry();	
	BuildMaterial();
	BuildRenderItems();	
	BuildFrameResourcse();
	BuildPSO();

	InitLight();

	ThrowIfFailed(m_CmdList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, cmdsLists);

	FlushCommandQueue();	
}

void myAppClass::BuildFrameResourcse()
{
	UINT passcount = 1;
	UINT objectCount = m_AllRenderItems.size();
	UINT materialCount = mMaterials.size();

	for (int i = 0; i < gNumFrameResourcesCount; i++)
	{
		m_FrameResources.push_back(std::make_unique<FrameResource>(m_device.Get(), passcount, objectCount, materialCount, VerticesCount, VerticesCount * 3));
	}
}

void myAppClass::MoveObj(int Sig)
{
	/*
	if (Sig == -1) mTheta -= XMConvertToRadians(1.1f);
	else mTheta += XMConvertToRadians(1.1f);
	*/
	int selectLight = 3;

	if (lightIndexSelID != selectLight)
	{
		float dx = 1.0f;// / 10.0f;// / 180.0f*XM_PI;

		mLights.at(selectLight).posTheta+= dx;
		mLights.at(selectLight).needToUpdateRI = 1;
	}

	 selectLight = 4;
	if (lightIndexSelID != selectLight)
	{
		float dx = 1.0f;// / 10.0f;// / 180.0f*XM_PI;

		mLights.at(selectLight).mPhi += dx;
		mLights.at(selectLight).needToUpdateRI = 1;
	}
}

void myAppClass::Draw()
{
	if (ResetPSO)
	{
		//BuildPSO();		
	}
	
	auto cmdListAllocator = m_CurrentFrameResource->cmdListAllocator;

	ThrowIfFailed(cmdListAllocator->Reset());
	ThrowIfFailed(m_CmdList->Reset(cmdListAllocator.Get(), m_pso.Get()));

	m_CmdList->RSSetScissorRects(1, &m_ScissorRect);
	m_CmdList->RSSetViewports(1, &m_ScreenViewport);

	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChainBuffers[m_swapChain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	FLOAT mycolor[4] = { 0.5f, 0.5f, 0.5f, 0.1f };
	m_CmdList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_swapChain->GetCurrentBackBufferIndex(), m_rtvDescriptorSize), mycolor,0,nullptr);
	m_CmdList->ClearDepthStencilView(dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_CmdList->OMSetRenderTargets(1, &CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_swapChain->GetCurrentBackBufferIndex(), m_rtvDescriptorSize), true, &dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	//m_CmdList->SetDescriptorHeaps(1, descriptorHeaps);

	m_CmdList->SetGraphicsRootSignature(m_RootSignature.Get());

	
	
	/*
	if (Mode == FaceMode)
	{
		m_CmdList->IASetIndexBuffer(&m_box->indexBufferView());
		m_CmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (Mode == LineMode)
		m_CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	else if (Mode == PointMode)
		m_CmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// --------- m_CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	// --------- m_CmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		

	m_CmdList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

	if (FaceCountToDraw > m_box->box.IndexCount) FaceCountToDraw = m_box->box.IndexCount;	

	if (Mode == FaceMode)
		m_CmdList->DrawIndexedInstanced(FaceCountToDraw, 1, 0, 0, 0);
	else
		m_CmdList->DrawInstanced(m_box->box.VertextCount, 1, 0, 0);
	*/
	// Draw one more time

	auto passCB = m_CurrentFrameResource->passCB->Resource();

	m_CmdList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	
	DrawRenderItems(m_CmdList.Get(), &m_AllRenderItems);

	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChainBuffers[m_swapChain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	
	ThrowIfFailed(m_CmdList->Close());

	ID3D12CommandList* CmdLists[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, CmdLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));
	FlushCommandQueue();
}

void myAppClass::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<RenderItem>* rItems)
{
	UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(ObjectContants));
	UINT matCBByteSize = CalcConstantBufferByteSize(sizeof(MaterialConstants));
	
	auto objectCB = m_CurrentFrameResource->perObjectCB->Resource();
	auto matCB = m_CurrentFrameResource->materialCB->Resource();
	
	for (size_t i = 0; i < rItems->size(); i++)
	{
		RenderItem ri = rItems->at(i);

		cmdList->IASetVertexBuffers(0, 1, &ri.Geo->vertexBufferView());
		cmdList->IASetIndexBuffer(&ri.Geo->indexBufferView());
		cmdList->IASetPrimitiveTopology(ri.primitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri.objCBIndex*objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS objMatCBAddress = matCB->GetGPUVirtualAddress() + ri.Mat->MatCBIndex*matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(1, objMatCBAddress);

		UINT indexCount = ri.Geo->DrawArgs["grid"].IndexCount;
		UINT startIndex= ri.Geo->DrawArgs["grid"].StartIndexLocation;
		UINT baseVertexLocation = ri.Geo->DrawArgs["grid"].BaseVertexLocation;

		if (ri.primitiveType != D3D10_PRIMITIVE_TOPOLOGY_POINTLIST)
		cmdList->DrawIndexedInstanced(indexCount, 1, startIndex, baseVertexLocation, 0);

		//Draw just points
		//m_CmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		//m_CmdList->DrawInstanced(ri.Geo->DrawArgs["grid"].VertextCount, 1, 0, 0);
	}
}

void myAppClass::Update()
{		
	m_iCurrentResourceIndex = (m_iCurrentResourceIndex + 1) % gNumFrameResourcesCount;
	m_CurrentFrameResource = m_FrameResources.at(m_iCurrentResourceIndex).get();
	
	UpdateLight();
	UpdateLightToRenderIntem();

	UpdateCamera();
	UpdatePassCB();
	UpdateCB();
	UpdateMaterialCB();
	UpdateGeometry();
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

	CD3DX12_ROOT_PARAMETER slotParametr[3];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	//slotParametr[0].InitAsDescriptorTable(1, &cbvTable);

	slotParametr[0].InitAsConstantBufferView(0);
	slotParametr[1].InitAsConstantBufferView(1);
	slotParametr[2].InitAsConstantBufferView(2);

	CD3DX12_ROOT_SIGNATURE_DESC rootDecs(3, slotParametr, 0, nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};
}

void myAppClass::BuildBoxGeometry()
{
	/*
	// Load model from file 
	ModelLoaderClass ModelLoader;
	ModelLoader.GenerateDelone(N);
	//ModelLoader.LoadModelFromFile(L"PlainModel.obj");
//	ModelLoader.LoadModelFromFile(L"m4.obj");

	int VertexCount = ModelLoader.GetVectorSizeV();
		
	std::vector<Vertex> vertices;
	ModelLoader.SetToBeginV();
	for (int i = 0; i < VertexCount; i++)
	{
		VertexModelLoader tmpVert = ModelLoader.GetNextV();
		tmpVert.z = 1;// / VertexCount;
		vertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y, tmpVert.z), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }));
	}
	
	int IndexCount = ModelLoader.GetVectorSizeI();

	std::vector<uint16_t> indices;
	ModelLoader.SetToBeginI();
	for (int i = 0; i < IndexCount; i++)
	{
		uint16_t tmpIndex = ModelLoader.GetNextI();
		indices.push_back(tmpIndex);
	}
	
	const UINT vbByteSize = (UINT) vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT) indices.size() * sizeof(uint16_t);
	
	m_box = std::make_unique<MeshGeometry>();
	
	m_box->VertexBufferGPU = CreateDeafultBuffer(vertices.data(), vbByteSize, m_box->VertexBufferUploader);
	m_box->IndexBufferGPU = CreateDeafultBuffer(indices.data(), ibByteSize, m_box->IndexBufferUploader);

	m_box->vertexByteStride = sizeof(Vertex);
	m_box->vertexBufferByteSize = vbByteSize;
	m_box->indexFormat = DXGI_FORMAT_R16_UINT;
	m_box->IndexBufferByteSize = ibByteSize;

	FaceCountToDraw = IndexCount;
	m_box->box.IndexCount = (UINT)IndexCount;
	m_box->box.VertextCount = (UINT)VertexCount;
	m_box->box.StartIndexLocation = 0;
	m_box->box.BaseVertexLocation = 0;
	*/
}

void myAppClass::BuildGeometry()
{
	
	// -------------------    Create Geometry entry for Terrain
	auto geo = std::make_unique<MeshGeometry>();

	geo->Name = "terrainGeo";
	geo->VertexBufferGPU = nullptr;
	geo->IndexBufferGPU = nullptr;

	mGeometry["terrainGeo"] = std::move(geo);

	renderNewTrianles = true;
	N = -1;

	// -------------------     Create geometry entry for Target point of View
	// Load model from file 
		
	std::vector<Vertex> tpVertices;
	std::vector<uint16_t> tpIindices;
	{
		ModelLoaderClass ModelLoader;
		ModelLoader.LoadModelFromFile(L"targetPoint.obj");

		int VertexCount = ModelLoader.GetVectorSizeV();

		ModelLoader.CalculateNormal();

		ModelLoader.SetToBeginV();
		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			tpVertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y , tmpVert.z),  tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();

		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			tpIindices.push_back(tmpIndex);

		}
		CreateConstGeometry("targetPoint", tpVertices.data(), tpIindices.data(), sizeof(Vertex), sizeof(UINT16), tpVertices.size(), tpIindices.size());
	}
	// ------------------- BOX 	
	{
		ModelLoaderClass ModelLoader;
		ModelLoader.LoadModelFromFile(L"worldBox.obj");
		ModelLoader.CalculateNormal();
		int VertexCount = ModelLoader.GetVectorSizeV();
		
		tpVertices.clear();
		tpIindices.clear();
		ModelLoader.SetToBeginV();
		
		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			tpVertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y , tmpVert.z), tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();

		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			tpIindices.push_back(tmpIndex);

		}
		CreateConstGeometry("Box", tpVertices.data(), tpIindices.data(), sizeof(Vertex), sizeof(UINT16), tpVertices.size(), tpIindices.size());
	}

	// -------------------     Create Geometry entry for Coordinate system

	std::array<Vertex, 4> csVertices = {
		Vertex({ XMFLOAT3{ 0.0f, 0.0f, 0.0f }, XMFLOAT3{ 0.0f, 0, 0 } }),
		Vertex({ XMFLOAT3{ 10.0f, 0.0f, 0.0f }, XMFLOAT3{ 1.0f, 0, 0 } }),
		Vertex({ XMFLOAT3{ 0.0f, 10.0f, 0.0f }, XMFLOAT3{ 0, 1.f, 0 } }),
		Vertex({ XMFLOAT3{ 0.0f, 0.0f, 10.0f }, XMFLOAT3{ 0, 0, 1.0f} })
	};

	std::array<UINT16, 6> csIndices = {
		0, 1,
		0, 2,
		0, 3
	};

	CreateConstGeometry("CS", &csVertices, &csIndices, sizeof(Vertex), sizeof(UINT16), csVertices.size(), csIndices.size());

	// ------------------- Create Geometry entry for Directional Light 	
	{
		ModelLoaderClass ModelLoader;
		ModelLoader.LoadModelFromFile(L"DirectionalLight.obj");
		ModelLoader.CalculateNormal();
		int VertexCount = ModelLoader.GetVectorSizeV();

		tpVertices.clear();
		tpIindices.clear();
		ModelLoader.SetToBeginV();

		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			tpVertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y , tmpVert.z), tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();

		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			tpIindices.push_back(tmpIndex);

		}
		CreateConstGeometry("Dlight", tpVertices.data(), tpIindices.data(), sizeof(Vertex), sizeof(UINT16), tpVertices.size(), tpIindices.size());
	}

	// ------------------- Create Geometry entry for Point Light 	
	{
		ModelLoaderClass ModelLoader;
		ModelLoader.LoadModelFromFile(L"PointLight.obj");
		ModelLoader.CalculateNormal();
		int VertexCount = ModelLoader.GetVectorSizeV();

		tpVertices.clear();
		tpIindices.clear();
		ModelLoader.SetToBeginV();

		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			tpVertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y , tmpVert.z), tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();

		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			tpIindices.push_back(tmpIndex);

		}
		CreateConstGeometry("Plight", tpVertices.data(), tpIindices.data(), sizeof(Vertex), sizeof(UINT16), tpVertices.size(), tpIindices.size());
	}

	// ------------------- Create Geometry entry for Spot Light 	
	{
		ModelLoaderClass ModelLoader;
		ModelLoader.LoadModelFromFile(L"SpotLight.obj");
		ModelLoader.CalculateNormal();
		int VertexCount = ModelLoader.GetVectorSizeV();

		tpVertices.clear();
		tpIindices.clear();
		ModelLoader.SetToBeginV();

		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			tpVertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y , tmpVert.z), tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();

		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			tpIindices.push_back(tmpIndex);

		}
		CreateConstGeometry("Slight", tpVertices.data(), tpIindices.data(), sizeof(Vertex), sizeof(UINT16), tpVertices.size(), tpIindices.size());
	}

	// ------------------- Create Geometry entry for the Wall	
	{
		ModelLoaderClass ModelLoader;
		ModelLoader.LoadModelFromFile(L"theWall.obj");
		ModelLoader.CalculateNormal();
		int VertexCount = ModelLoader.GetVectorSizeV();

		tpVertices.clear();
		tpIindices.clear();
		ModelLoader.SetToBeginV();

		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			tpVertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y , tmpVert.z), tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();

		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			tpIindices.push_back(tmpIndex);

		}
		CreateConstGeometry("theWall", tpVertices.data(), tpIindices.data(), sizeof(Vertex), sizeof(UINT16), tpVertices.size(), tpIindices.size());
	}
}

void myAppClass::BuildMaterial()
{
	auto mat = std::make_unique<Material>();
	
	// Material 2
	mat->Name = "material1";
	mat->MatCBIndex = 0;
	mat->NumFrameDirty = gNumFrameResourcesCount;
	mat->DiffuseAlbedo = XMFLOAT4(0.5f, 0.52f, 0.5f, 1.0f);
	//mat->DiffuseAlbedo = XMFLOAT4(1.0f, 0.4f, 1.0f, 1.0f);
	mat->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	mat->Roughness = 0.0;

	mMaterials[mat->Name] = std::move(mat);

	// Material 2
	auto mat2 = std::make_unique<Material>();
	mat2->Name = "material2";
	mat2->MatCBIndex = 1;
	mat2->NumFrameDirty = gNumFrameResourcesCount;
	mat2->DiffuseAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat2->FresnelR0 = XMFLOAT3(0.1f, 0.3f, 0.0f);
	mat2->Roughness = 0.1;

	mMaterials[mat2->Name] = std::move(mat2);

	// Material 3
	auto mat3 = std::make_unique<Material>();
	mat3->Name = "LightSelected";
	mat3->MatCBIndex = 2;
	mat3->NumFrameDirty = gNumFrameResourcesCount;
	mat3->DiffuseAlbedo = XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f);
	mat3->FresnelR0 = XMFLOAT3(0.1f, 0.3f, 0.0f);
	mat3->Roughness = 1.0f;

	mMaterials[mat3->Name] = std::move(mat3);


	// Material 4
	auto mat4 = std::make_unique<Material>();
	mat4->Name = "LightUnSelected";
	mat4->MatCBIndex = 3;
	mat4->NumFrameDirty = gNumFrameResourcesCount;
	mat4->DiffuseAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat4->FresnelR0 = XMFLOAT3(0.1f, 0.3f, 0.0f);
	mat4->Roughness = 1.0f;

	mMaterials[mat4->Name] = std::move(mat4);


	// Material 5
	auto mat5 = std::make_unique<Material>();
	mat5->Name = "LightTurnOff";
	mat5->MatCBIndex = 4;
	mat5->NumFrameDirty = gNumFrameResourcesCount;
	mat5->DiffuseAlbedo = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mat5->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.1f);
	mat5->Roughness = 0.1f;

	mMaterials[mat5->Name] = std::move(mat5);

}

void myAppClass::BuildRenderItems()
{
	//auto terrainItem = std::make_unique<RenderItem>();
	RenderItem renderItem;// = std::make_unique<RenderItem>();

	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 0;
	renderItem.Geo = mGeometry["terrainGeo"].get();
	renderItem.Mat = mMaterials["material1"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	//m_AllRenderItems.push_back(std::move(terrainItem));
	m_AllRenderItems.push_back(renderItem);

	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 0;
	renderItem.Geo = mGeometry["CS"].get();
	renderItem.Mat = mMaterials["material1"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	renderItem.numDirtyVI = 0;	
	m_AllRenderItems.push_back(renderItem);
	
	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 1;
	renderItem.Geo = mGeometry["targetPoint"].get();
	renderItem.Mat = mMaterials["material2"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);

	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 0;
	renderItem.Geo = mGeometry["Box"].get();
	renderItem.Mat = mMaterials["material1"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);

	// Directional light
	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 2;
	renderItem.Geo = mGeometry["Dlight"].get();
	renderItem.Mat = mMaterials["LightUnSelected"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);
	BuildLight(0, m_AllRenderItems.size() - 1);

	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 3;
	renderItem.Geo = mGeometry["Dlight"].get();
	renderItem.Mat = mMaterials["LightUnSelected"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);
	BuildLight(0, m_AllRenderItems.size() - 1);	
	
	// Point Light
	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 4;
	renderItem.Geo = mGeometry["Plight"].get();
	renderItem.Mat = mMaterials["LightUnSelected"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);
	BuildLight(1, m_AllRenderItems.size() - 1);

	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 5;
	renderItem.Geo = mGeometry["Plight"].get();
	renderItem.Mat = mMaterials["LightUnSelected"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);
	BuildLight(1, m_AllRenderItems.size() - 1);

	// Spot Light
	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 6;
	renderItem.Geo = mGeometry["Slight"].get();
	renderItem.Mat = mMaterials["LightUnSelected"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);
	BuildLight(2, m_AllRenderItems.size() - 1);

	renderItem.world = MathHelper::Identity4x4();
	renderItem.objCBIndex = 1;
	renderItem.Geo = mGeometry["theWall"].get();
	renderItem.Mat = mMaterials["material1"].get();
	renderItem.primitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem.numDirtyCB = renderItem.numDirtyVI = 0;
	m_AllRenderItems.push_back(renderItem);

}

void myAppClass::BuildLight(int lightType, int renderItemID)
{
	if (lightType == 0) // Create Directional Light
	{
		CPULight tmpLight;
		tmpLight.mPhi = 0;
		tmpLight.mTheta = 0;
		tmpLight.mRadius = 1;
		tmpLight.Position = XMFLOAT3(0, 0, 0);
		tmpLight.lightType = 0;
		tmpLight.Strength = XMFLOAT3(0.8f, 0.8f, 0.8f);		
		tmpLight.renderItemID = renderItemID;
		
		tmpLight.needToUpdateRI = 1;
		tmpLight.needToUpdateLight = 1;
		mLights.push_back(tmpLight);
		lightCount++;
	}
	else if (lightType == 1) // Create Point Light
	{
		CPULight tmpLight;
		tmpLight.lightType = 1;
		tmpLight.mPhi = 0;
		tmpLight.mTheta = 0;
		tmpLight.mRadius = 0;
		tmpLight.Position = XMFLOAT3(0, 0, 0);
		tmpLight.Strength = XMFLOAT3(0.8f, 0.8f, 0.8f);
		tmpLight.mRadius = 15.0f;
		tmpLight.falloffStart = 1;
		tmpLight.falloffEnd = tmpLight.mRadius;		
		tmpLight.renderItemID = renderItemID;

		tmpLight.needToUpdateRI = 1;
		tmpLight.needToUpdateLight = 1;
		mLights.push_back(tmpLight);
		lightCount++;
	}	
	else if (lightType == 2) // Create SpotLight
		{
			CPULight tmpLight;
			tmpLight.lightType = 2;
			tmpLight.mPhi = 0;
			tmpLight.mTheta = 0;
			tmpLight.mRadius = 0;
			tmpLight.Position = XMFLOAT3(0, 0, 0);
			tmpLight.Strength = XMFLOAT3(0.8f, 0.8f, 0.8f);
			tmpLight.mRadius = 15.0f;			
			tmpLight.spotPower = 7;
			tmpLight.falloffStart = 1;
			tmpLight.falloffEnd = tmpLight.mRadius;
			tmpLight.renderItemID = renderItemID;

			tmpLight.needToUpdateRI = 1;
			tmpLight.needToUpdateLight = 1;
			mLights.push_back(tmpLight);
			lightCount++;
		}
}

// ------------------------------------------------------------------------------------------------------- Inicialization --------------

void myAppClass::InitLight()
{
	// init Light position
	/*
	mLights.at(0).Position = XMFLOAT3(10, 40, 0);
	mLights.at(1).Position = XMFLOAT3(0, 20, 0);
	mLights.at(2).Position = XMFLOAT3(10, 5, 0);
	mLights.at(3).Position = XMFLOAT3(-10, 10, -10);
	mLights.at(4).Position = XMFLOAT3(20, 10, 15);
	*/

	mLights.at(0).Strength = XMFLOAT3(0.8f, 0.8f, 0.8f);
	mLights.at(1).Strength = XMFLOAT3(0.8f, 0.2f, 0.2f);
	mLights.at(2).Strength = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mLights.at(3).Strength = XMFLOAT3(1.0f, 1.0f, 1.0f);	
	mLights.at(4).Strength = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Light Directtional 1
	mLights.at(0).posRadius = 10;
	mLights.at(0).posPhi = 10;
	mLights.at(0).posTheta = 20;

	mLights.at(0).mRadius = 3;
	mLights.at(0).mPhi = 805.4f;
	mLights.at(0).mTheta = 730.3f;

	// Light Directtional 2
	mLights.at(1).posRadius = 10;
	mLights.at(1).posPhi = -10;
	mLights.at(1).posTheta = 20;
	mLights.at(1).turnOn = 0;

	// Light Point 1
	mLights.at(2).posRadius = 20;
	mLights.at(2).posPhi = 10;
	mLights.at(2).posTheta = 30;

	// Light Point 2
	mLights.at(3).posRadius = 60;
	mLights.at(3).posPhi = -94;
	mLights.at(3).posTheta = 30;

	mLights.at(3).mRadius = 73.5f;
	mLights.at(3).mPhi = -82.6f;
	mLights.at(3).mTheta = 0;
	mLights.at(3).falloffEnd = mLights.at(3).mRadius;

	// Light Spot 1
	mLights.at(4).posRadius = 29;
	mLights.at(4).posPhi = -369.4f;
	mLights.at(4).posTheta = 43.7f;

	mLights.at(4).mRadius = 29.1f;
	mLights.at(4).mPhi = -82.6f;
	mLights.at(4).mTheta = -40.8f;
	mLights.at(4).falloffEnd = mLights.at(4).mRadius;
}

// --------------------------------------------------------------------------------------------------------- Updateting ----------------

void myAppClass::UpdateGeometry()
{
	int terrainIndex = -1;
	for (size_t i = 0; i < m_AllRenderItems.size(); i++)
	{
		if (m_AllRenderItems.at(i).Geo->Name == "terrainGeo")
		{
			terrainIndex = i;
		}
	}

	if (terrainIndex == -1) return;

	if (renderNewTrianles || renderPrevTiangles || newFliplevel || prevFlipLevel)
	{
		// нужно построить и загрузить новую порцию данные/продвинутьс€ на 1 триугольник в треангул€ции
		if (renderNewTrianles)
		{
			N++;
			FlipLevel = 0;
		}
		else if (renderPrevTiangles)
		{
			N--;
			FlipLevel = 0;
		}
		
		if (newFliplevel) FlipLevel++;
		else if (prevFlipLevel) FlipLevel--;
		

		if (N < 0) N = 0;
		if (FlipLevel < 0) FlipLevel = 0;

		renderNewTrianles = false;
		renderPrevTiangles = false;
		newFliplevel = false;
		prevFlipLevel = false;

		m_vertices.clear();
		m_indices.clear();

		// Load model from file 
		ModelLoaderClass ModelLoader;
		//ModelLoader.GenerateDelone(N, FlipLevel);
		//ModelLoader.LoadModelFromFile(L"PlainModel.obj");
		ModelLoader.LoadModelFromFile(L"terrain.obj"); 
		ModelLoader.CalculateNormal();
		int VertexCount = ModelLoader.GetVectorSizeV();

		ModelLoader.SetToBeginV();
		for (int i = 0; i < VertexCount; i++)
		{
			VertexModelLoader tmpVert = ModelLoader.GetNextV();
			//tmpVert.z = tmpVert.y;// / VertexCount;
			//tmpVert.y = 0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1 - 0)));;
			m_vertices.push_back(Vertex({ XMFLOAT3(tmpVert.x, tmpVert.y, tmpVert.z), tmpVert.normal }));
		}

		int IndexCount = ModelLoader.GetVectorSizeI();
		
		ModelLoader.SetToBeginI();
		for (int i = 0; i < IndexCount; i++)
		{
			uint16_t tmpIndex = ModelLoader.GetNextI();
			m_indices.push_back(tmpIndex);
		}

		m_AllRenderItems.at(terrainIndex).numDirtyVI = gNumFrameResourcesCount;
	}
	
	for (size_t i = 0; i < m_AllRenderItems.size(); i++)
	{
		if (m_AllRenderItems.at(i).numDirtyVI > 0)
		{
			//если есть новые данные которые надо загрузить в буффер Vertices/Indices FrameResource, делаем это дл€ кажого FrameResource
			int VertexCount = m_vertices.size();
			int IndexCount = m_indices.size();

			auto currVertices = m_CurrentFrameResource->vertices.get();
			auto currIndices = m_CurrentFrameResource->indices.get();

			for (int i = 0; i < VertexCount; i++)
			{
				Vertex v;
				currVertices->CopyData(i, m_vertices.at(i));
			}

			for (int i = 0; i < IndexCount; i++)
			{
				Vertex v;
				currIndices->CopyData(i, m_indices.at(i));
			}

			const UINT vbByteSize = (UINT)m_vertices.size() * sizeof(Vertex);
			const UINT ibByteSize = (UINT)m_indices.size() * sizeof(UINT32);

			mGeometry["terrainGeo"]->vertexByteStride = sizeof(Vertex);
			mGeometry["terrainGeo"]->vertexBufferByteSize = vbByteSize;
			mGeometry["terrainGeo"]->indexFormat = DXGI_FORMAT_R32_UINT;
			mGeometry["terrainGeo"]->IndexBufferByteSize = ibByteSize;

			mGeometry["terrainGeo"]->VertexBufferGPU = currVertices->Resource();
			mGeometry["terrainGeo"]->IndexBufferGPU = currIndices->Resource();

			SubMesh submesh;
			submesh.IndexCount = (UINT)IndexCount;
			submesh.VertextCount = (UINT)VertexCount;
			submesh.StartIndexLocation = 0;
			submesh.BaseVertexLocation = 0;

			mGeometry["terrainGeo"]->DrawArgs["grid"] = submesh;

			m_AllRenderItems.at(i).numDirtyVI--; //текущий FrameResource мы обнвовили
		}
	}
	
}

void myAppClass::UpdateLight()
{
	if (lightIndexSelID >= lightCount)
		lightIndexSelID = 0;
	
	float dx = 1.0f/10.0f;// / 180.0f*XM_PI;
	
	mLights.at(lightIndexSelID).mPhi += dx*lightRotateLR;
	mLights.at(lightIndexSelID).mTheta += dx*lightRotateUD;

	dx = 1.0f / 100.0f;
	mLights.at(lightIndexSelID).mRadius += dx*lightScaleRadius;
	if (mLights.at(lightIndexSelID).mRadius < 3)
		mLights.at(lightIndexSelID).mRadius = 3.0f;
	else if (mLights.at(lightIndexSelID).mRadius > 100)
		mLights.at(lightIndexSelID).mRadius = 100.0f;

	mLights.at(lightIndexSelID).falloffEnd = mLights.at(lightIndexSelID).mRadius;
	
	dx = 1.0f / 100.0f;
	mLights.at(lightIndexSelID).spotPower += dx*lightScaleSpotRadius;
	if (mLights.at(lightIndexSelID).spotPower < 1)
		mLights.at(lightIndexSelID).spotPower = 1.0f;
	else if (mLights.at(lightIndexSelID).spotPower > 50)
		mLights.at(lightIndexSelID).spotPower = 50.0f;

	
	/*
	dx = 1.0f/100.0f;
	XMFLOAT3 tmpPos = mLights.at(lightIndexSelID).Position;
	tmpPos.x += moveLightFB*dx;
	tmpPos.y+= moveLightUD*dx;
	tmpPos.z += moveLightLR*dx;	
	mLights.at(lightIndexSelID).Position = tmpPos;
	*/

	dx = 1.0f / 10.0f;
	mLights.at(lightIndexSelID).posRadius += moveLightUD*dx/10.0f;
	mLights.at(lightIndexSelID).posPhi += moveLightLR*dx;
	mLights.at(lightIndexSelID).posTheta += moveLightFB *dx;


	mLights.at(lightIndexSelID).needToUpdateRI |= (lightRotateLR != 0) || (lightRotateUD != 0) || (moveLightFB != 0)
		|| (moveLightLR != 0) || (moveLightUD != 0) || (lightScaleRadius != 0) || (lightScaleSpotRadius != 0);

	mLights.at(lightIndexSelID).needToUpdateLight |= mLights.at(lightIndexSelID).needToUpdateRI;

	// Calculate Position of light 
	for (size_t i = 0; i < mLights.size(); i++)
	{		
		float x = mLights.at(i).posRadius* sin(mLights.at(i).posTheta / 180.0f*XM_PI)*sin(mLights.at(i).posPhi / 180.0f*XM_PI);
		float z = mLights.at(i).posRadius* sin(mLights.at(i).posTheta / 180.0f*XM_PI)*cos(mLights.at(i).posPhi / 180.0f*XM_PI);
		float y = mLights.at(i).posRadius* cos(mLights.at(i).posTheta / 180.0f*XM_PI);

		mLights.at(i).Position = XMFLOAT3(x, y, z);
	}
}

void myAppClass::UpdateLightToRenderIntem()
{
	for (size_t i = 0; i < mLights.size(); i++)
	{
		if (mLights.at(i).needToUpdateRI)
		{			
			XMMATRIX tmpMatrix;
			XMMATRIX tmpScaleMatrix;
			// not used --- tmpMatrix = XMLoadFloat4x4(&m_AllRenderItems.at(mLights.at(i).renderItemID).world); 
			
			// Scalse do only for point light
			if (mLights.at(i).lightType == 1)
			{
				tmpScaleMatrix = XMMatrixScaling(mLights.at(i).mRadius / 15.0f, mLights.at(i).mRadius / 15.0f, mLights.at(i).mRadius / 15.0f);
			}
			else if (mLights.at(i).lightType == 2)
			{
				tmpScaleMatrix = XMMatrixScaling(mLights.at(i).mRadius / 15.0f, mLights.at(i).spotPower/7, mLights.at(i).spotPower / 7);
			}
			else
				tmpScaleMatrix = XMMatrixIdentity();

			tmpMatrix = XMMatrixTranslation(mLights.at(i).Position.x, mLights.at(i).Position.y, mLights.at(i).Position.z);
			
			tmpMatrix = tmpScaleMatrix*XMMatrixRotationRollPitchYaw(0, mLights.at(i).mPhi/180.0f*XM_PI, mLights.at(i).mTheta / 180.0f*XM_PI)*tmpMatrix;
			//tmpMatrix = (XMMatrixRotationZ(mLights.at(i).mTheta)*XMMatrixRotationY(mLights.at(i).mPhi))*tmpMatrix;
									
			XMStoreFloat4x4(&m_AllRenderItems.at(mLights.at(i).renderItemID).world, tmpMatrix);
			
			m_AllRenderItems.at(mLights.at(i).renderItemID).numDirtyCB = gNumFrameResourcesCount;
			mLights.at(i).needToUpdateRI = 0;
		}
		
		
		if (lightTurnOnOff)
		{
			lightTurnOnOff = 0;
			mLights.at(lightIndexSelID).turnOn = !mLights.at(lightIndexSelID).turnOn;
		}
		
		// highlighted by Material selected Light
		if (lightIndexSelID == i)
		{
			m_AllRenderItems.at(mLights.at(i).renderItemID).Mat = mMaterials["LightSelected"].get();
		}
		else if (mLights.at(i).turnOn) // If Light is not selected, but TurnOn
		{
			m_AllRenderItems.at(mLights.at(i).renderItemID).Mat = mMaterials["LightUnSelected"].get();
		}	
		else // If Light is not selected and TurnOff
		{
			m_AllRenderItems.at(mLights.at(i).renderItemID).Mat = mMaterials["LightTurnOff"].get();
		}

	}
}

void myAppClass::UpdateCB()
{
	auto currCBObject = m_CurrentFrameResource->perObjectCB.get();

	for (size_t i = 0; i < m_AllRenderItems.size(); i++)
	{
		if (m_AllRenderItems.at(i).numDirtyCB > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&m_AllRenderItems.at(i).world);

			//sworld = XMMatrixRotationX(90);

			ObjectContants objConstants;
			XMStoreFloat4x4(&objConstants.word, XMMatrixTranspose(world));

			currCBObject->CopyData(m_AllRenderItems.at(i).objCBIndex, objConstants);

			m_AllRenderItems.at(i).numDirtyCB--; // ќбновили Constant object дл€ текущего FrameResource;
		}
	}
}

void myAppClass::UpdateMaterialCB()
{
	auto currMatCB = m_CurrentFrameResource->materialCB.get();

	for (auto& e: mMaterials)
	{
		Material* mat = e.second.get();
		if (mat->NumFrameDirty>0)
		{			
			MaterialConstants matCB;

			matCB.DiffuseAlbedo = mat->DiffuseAlbedo;
			matCB.FresnelR0 = mat->FresnelR0;
			matCB.Roughness = mat->Roughness;

			currMatCB->CopyData(mat->MatCBIndex , matCB);

			mat->NumFrameDirty--; // ќбновили Material Const Buffer дл€ текущего FrameResource;
		}
	}
}

void myAppClass::UpdatePassCB()
{
	{
		//this should be in onResize section
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*3.14, AspectRatio, 1.0f, 1000.0f);
		XMStoreFloat4x4(&mProj, P);
	}

	PassConstants mMainPassCB;
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)m_width, (float)m_height);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_width, 1.0f / m_height);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = 0;// gt.TotalTime();
	mMainPassCB.DeltaTime = 0;//gt.DeltaTime();
	//mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

	//XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, mSunTheta, mSunPhi);

	for (size_t i = 0; i < mLights.size(); i++)
	{
		//if (mLights.at(i).needToUpdateLight)
		{
			float tmpTheta = 90 + mLights.at(i).mTheta;
			float tmpPhi = 90 + mLights.at(i).mPhi;

			float x = sin(tmpTheta / 180.0f*XM_PI)*sin(tmpPhi / 180.0f*XM_PI);
			float z = sin(tmpTheta / 180.0f*XM_PI)*cos(tmpPhi / 180.0f*XM_PI);
			float y = cos(tmpTheta / 180.0f*XM_PI);
			
			XMVECTOR lightDir = XMVectorSet(x,-y,z,0);

			XMStoreFloat3(&mMainPassCB.Lights[i].Direction, lightDir);
			mMainPassCB.Lights[i].Strength = mLights.at(i).Strength;
			mMainPassCB.Lights[i].Position = mLights.at(i).Position;
			mMainPassCB.Lights[i].spotPower = mLights.at(i).spotPower;
			mMainPassCB.Lights[i].falloffStart = mLights.at(i).falloffStart;
			mMainPassCB.Lights[i].falloffEnd = mLights.at(i).falloffEnd;			
			mMainPassCB.Lights[i].lightType = mLights.at(i).lightType + 1;
			mMainPassCB.Lights[i].turnOn = mLights.at(i).turnOn;
			mLights.at(i).needToUpdateLight = 0;
		}

		
	}
	auto currPassCB = m_CurrentFrameResource->passCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void myAppClass::UpdateCamera()
{
	/*
	// through hyperbolic sin and cos (?)
	mEyePos.x = mRadius*sinf(mPhi)*cosf(mTheta);
	mEyePos.z = mRadius*sinf(mPhi)*sinf(mTheta);
	mEyePos.y = mRadius*cosf(mPhi);
	*/


	// Spherical to Deckard


	TargetPos.x = sin(mThetaCamera / 180 * pi)*sin(mPhiCamera / 180 * pi);
	TargetPos.z = sin(mThetaCamera / 180 * pi)*cos(mPhiCamera / 180 * pi);
	TargetPos.y = cos(mThetaCamera / 180 * pi);

	int dx = 60;
	mEyePos.x += (TargetPos.x *moveFB)/ dx;
	mEyePos.z += (TargetPos.z *moveFB)/ dx;
	mEyePos.y += (TargetPos.y *moveFB)/ dx;

	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	
	//XMVECTOR target = XMVectorZero();
	
	XMVECTOR target = XMVectorSet(TargetPos.x, TargetPos.y, TargetPos.z, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	
	//XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMMATRIX view = XMMatrixLookToLH(pos, target, up);
	XMMATRIX tmpWorldTargetPoint = XMMatrixTranslationFromVector(target);
	
	XMStoreFloat4x4(&mView, view);


	for (size_t i = 0; i < m_AllRenderItems.size(); i++)
	{
		if (m_AllRenderItems.at(i).Geo->Name == "targetPoint")
		{
			XMStoreFloat4x4(&m_AllRenderItems.at(i).world, tmpWorldTargetPoint);
			m_AllRenderItems.at(i).numDirtyCB = gNumFrameResourcesCount;
		}
	}
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
	ModeFlag = true;
	if (!ModeFlag)
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	else 
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;


	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	
	if (Mode == FaceMode)
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	else if (Mode == LineMode)
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	else if (Mode == PointMode)
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	
	FrontCounterClockwise = false;
	psoDesc.RasterizerState.FrontCounterClockwise = FrontCounterClockwise;
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

void myAppClass::CreateConstGeometry(std::string name, const void* vertexData, const void* indexData, int vertexSize, int indexSize, int vertexCount, int indexCount)
{
	auto geo = std::make_unique<MeshGeometry>();

	geo->Name = name;

	int vertexByteStride = vertexSize;
			
	geo->vertexBufferByteSize = vertexSize * vertexCount;
	geo->IndexBufferByteSize = indexSize * indexCount;
	geo->vertexByteStride = vertexByteStride;
	geo->indexFormat = DXGI_FORMAT_R16_UINT;

	geo->VertexBufferGPU = CreateDeafultBuffer(vertexData, geo->vertexBufferByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = CreateDeafultBuffer(indexData, geo->IndexBufferByteSize, geo->IndexBufferUploader);

	SubMesh submesh;
	submesh.IndexCount = (UINT)indexCount;
	submesh.VertextCount = (UINT)vertexCount;
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;
	mGeometry[name] = std::move(geo);
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