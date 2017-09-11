
#include "Cmn3DApp.h"
#include <WindowsX.h>

//using namespace Microsoft::WRL;

Cmn3DApp::Cmn3DApp(HINSTANCE hInstance):m_hInstance(hInstance), m_width(600),m_height(600), m_BackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM), m_DepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)
{
	assert(thisApp == nullptr);
	thisApp = this;	
}


Cmn3DApp::~Cmn3DApp()
{
	FlushCommandQueue();
	CloseHandle(m_fenceEvent);
}

Cmn3DApp* Cmn3DApp::GetCurrectApp()
{
	return thisApp;
}

Cmn3DApp* Cmn3DApp::thisApp = nullptr;



LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	return Cmn3DApp::GetCurrectApp()->MsgProc(hwnd, msg, wParam, lParam);
}

LRESULT Cmn3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			//PostMessage(hwnd, WM_DESTROY, 0, 0);
			PostQuitMessage(0);
			return 0;
		}
		else if (wParam == VK_LEFT) MoveObj(-1);
		else if (wParam == VK_RIGHT) MoveObj(1);
		//else if (wParam == VK_SPACE) { ModeFlag = !ModeFlag; ResetPSO = true;}
		else if (wParam == VK_SPACE) { renderNewTrianles = true;}
		
		else if (wParam == 'Q') { newFliplevel = true; }
		//else if (wParam == 'W') { prevFlipLevel = true; }
		else if (wParam == 'N') { renderPrevTiangles = true; }
		else if (wParam == 'F')	{ Mode = FaceMode; ResetPSO = true;}
		else if (wParam == 'L') { Mode = LineMode; ResetPSO = true;}
		else if (wParam == 'P') { Mode = PointMode; ResetPSO = true;}
		else if (wParam == 'Z') { Pause = !Pause;}
		else if (wParam == 'C') {
			FrontCounterClockwise = !FrontCounterClockwise; ResetPSO = true;
		}
		else if (wParam == VK_OEM_PLUS)
			FaceCountToDraw++;
		else if (wParam == VK_OEM_MINUS)
		{
			FaceCountToDraw--;
			if (FaceCountToDraw < 1) FaceCountToDraw = 1;
		}
		else if ((wParam == 'A') || (wParam == 'D') ||( wParam == 'W') ||( wParam == 'S')) 	{ onKeyPress(wParam); }
		return 0;

	case WM_KEYUP:
		if ((wParam == 'A') || (wParam == 'D') || (wParam == 'W') || (wParam == 'S'))	{ onKeyUp(wParam);}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		onMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		onMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEMOVE:
		onMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_SIZE:

		if (m_device)
		{
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);
			Create_RTV_DSV();
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Cmn3DApp::Initialize()
{
	InitMainWindow();
	InitDirect3D();
}

void Cmn3DApp::InitMainWindow()
{
	WNDCLASS windowClass = {};

	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = m_hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszClassName = L"DXCommonApplClass";
	
	if (!RegisterClass(&windowClass))
	{
		MessageBox(0, L"Window creation error",0,0);
		return;
	}

	RECT windowRect = { 0,0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	int WindWidth = windowRect.right - windowRect.left;
	int WindHeight = windowRect.bottom - windowRect.top;

	m_hMainWind = CreateWindow(L"DXCommonApplClass", L"Direct3D application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WindWidth, WindHeight, 0, 0, m_hInstance, 0);
	
	ShowWindow(m_hMainWind, SW_SHOW);
	UpdateWindow(m_hMainWind);

}

void Cmn3DApp::InitDirect3D()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	// Create Factory	
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

	//Create Adapter and Device
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter(m_factory.Get(), &hardwareAdapter);
		
	ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
	
#if defined (_DEBUG)
	LogAdapters();
#endif

	//Create Command Objects;
	{
		//Command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CmdQueue)));

		//Command list allocator
		ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CmdAllocator)));

		//Command list
		ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CmdList)));
		m_CmdList->Close();
	}
	
	//Create SwapShain
	{
		//m_swapChain.Reset(); use if Swapchain need re-create

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = m_width;
		swapChainDesc.Height = m_height;
		swapChainDesc.BufferCount = m_BufferCount;
		swapChainDesc.Format = m_BackBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> pSwapChain;	

		ThrowIfFailed(m_factory->CreateSwapChainForHwnd(m_CmdQueue.Get(), m_hMainWind, &swapChainDesc, nullptr, nullptr, &pSwapChain));
		pSwapChain.As(&m_swapChain);
	}

	//Create Render Target View and Depth/Stensil buffer View (RTV and DSV Descriptor Heaps)
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = m_BufferCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;						
		m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));		
	}
	
	// Create Fence and Event Handle for it
	{
		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_fenceValue = 0;
		
		m_fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}

	//Create RTV and DS Views
	Create_RTV_DSV();
}

void Cmn3DApp::Create_RTV_DSV()
{
	assert(m_device);
	assert(m_swapChain);
	assert(m_CmdAllocator);

	//This method is used also when Window's size is changed. So we should reset required resources (swapshain, depth/stensil buffer), resize it and 
	// and then creatre Views again (Dsecriptors in Descriptor Heaps) for this.

	// Before how to reset SwapChain and DSB we should be sure that 3D Adapter does not use it right now, to do it, we should 'Flush off' current
	// command queue - so 3D Adapter nothing to do.

	FlushCommandQueue();
	//Reset Command List
	ThrowIfFailed(m_CmdList->Reset(m_CmdAllocator.Get(), nullptr));

	// Resize SwapChain
	{
		// Reset SwapChain buffers (Release the previous resources)
		for (int i = 0; i < m_BufferCount; i++)
			m_SwapChainBuffers[i].Reset();		

		// Resize SwapChain
		/* MSDN:
		Before you call ResizeBuffers, ensure that the application releases all references(by calling the appropriate
		number of Release invocations) on the resources, any views to the resource, and any command lists that use either
		the resources or views, and ensure that neither the resource nor a view is still bound to a device context.	.
		*/
		ThrowIfFailed(m_swapChain->ResizeBuffers(m_BufferCount, m_width, m_height, m_BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
		UINT rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create SwapChain Views
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < m_BufferCount; i++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])));
			m_device->CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, rtvDescriptorSize);
		}
	}

	// We can't just resize Depth/Stensil buffer, we should create it again
	{
		m_DepthStencilSBuffer.Reset();

		//Create Resource for Depth Stencil buffer
		D3D12_RESOURCE_DESC depthStencilDescr = {};
		depthStencilDescr.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDescr.Alignment = 0;
		depthStencilDescr.Width = m_width;
		depthStencilDescr.Height = m_height;
		depthStencilDescr.DepthOrArraySize = 1;
		depthStencilDescr.MipLevels = 1;
		depthStencilDescr.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilDescr.SampleDesc.Count = 1;
		depthStencilDescr.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDescr.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		
		D3D12_CLEAR_VALUE optClear;
		optClear.Format = m_DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		ThrowIfFailed(m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthStencilDescr, 
				D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(&m_DepthStencilSBuffer)));


		//Create View for Depth/Stencil buffer
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = m_DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		m_device->CreateDepthStencilView(m_DepthStencilSBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	//Transition the resource for DepthStgencil from common state to be used as depth buffer
	m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilSBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands
	ThrowIfFailed(m_CmdList->Close());
	ID3D12CommandList* cmdsList[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, cmdsList); 
	
	//Wait when resize is done
	FlushCommandQueue();

	//Update the viewport transform to cover the client area
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(m_width);
	m_ScreenViewport.Height = static_cast<float>(m_height);
	m_ScreenViewport.MinDepth = 0;
	m_ScreenViewport.MaxDepth = 1.0f;

	m_ScissorRect = { 0,0,m_width,m_height };

	AspectRatio = m_width/ m_height;

}

void Cmn3DApp::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// do not select the Basic Render Device adapter; 
			continue;
		}

		// Check to see if the adapter support Dirtect3D 12, but don't create the actual device yet
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	*ppAdapter = adapter.Detach();
}

void Cmn3DApp::FlushCommandQueue()
{
	m_fenceValue++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(m_CmdQueue->Signal(m_Fence.Get(), m_fenceValue));

	//Wait until the GPU has completed up to this new fence point
	if (m_Fence->GetCompletedValue() < m_fenceValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void Cmn3DApp::LogAdapters()
{
	ComPtr<IDXGIAdapter1> adapter;	

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";		
		
		OutputDebugString(text.c_str());

		LogAdapterOutput(adapter.Get());	
	}
}

void Cmn3DApp::LogAdapterOutput(IDXGIAdapter1* pAdapter)
{
	UINT i = 0;
	IDXGIOutput* pOutput = nullptr;

	while (pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		pOutput->GetDesc(&desc);

		std::wstring text = L"*****Output: ";
		text += desc.DeviceName;
		text += L"\n";

		OutputDebugString(text.c_str());

		LogOutputDisplayMode(pOutput, m_BackBufferFormat);

		ReleaseCom(pOutput);
		i++;
	}
}

void Cmn3DApp::LogOutputDisplayMode(IDXGIOutput* pOutput, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;
	
	// call with null parametr to get list count
	pOutput->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	pOutput->GetDisplayModeList(format, flags, &count, &modeList[0]);
	
	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) + L"\n";

		::OutputDebugString(text.c_str());
	}
}


int Cmn3DApp::Run()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();

			if (!Pause)
				if (gameTimer.Tick()) MoveObj(1);
			Draw();

			std::wstring  statusText;
			statusText = L"Mode: ";
			if (Mode==FaceMode)
				statusText += L"FaceMode ";
			else if (Mode == LineMode)
				statusText += L"LineMode ";
			if (Mode == PointMode)
				statusText += L"PointMode ";
			
			statusText += L"Faces count: " + std::to_wstring(FaceCountToDraw);
			if (Pause)	statusText += L" (PAUSE)";

			statusText += L" N: " + std::to_wstring(N);
			statusText += L" FlipLlv: " + std::to_wstring(FlipLevel);

			SetWindowText(m_hMainWind,statusText.c_str());


		}
	}
	return 0;
}


