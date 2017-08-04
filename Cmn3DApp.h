#pragma once

#include <Windows.h>
#include <exception>
#include <vector>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "stdafx.h"
#include <comdef.h>
#include "Timer.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

class Cmn3DApp
{
public:
	Cmn3DApp(HINSTANCE hInstance);
	~Cmn3DApp();

	static Cmn3DApp* GetCurrectApp();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();	
	
protected: //Child class also can use it

	static Cmn3DApp* thisApp;
	const static UINT m_BufferCount = 2;
	bool ModeFlag = false;
	bool prevModeFlag = false;
	UINT m_rtvDescriptorSize = 0;
	float AspectRatio = 1.0f;

	DXGI_FORMAT m_BackBufferFormat;
	DXGI_FORMAT m_DepthStencilFormat;
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;
	
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Resource> m_SwapChainBuffers[m_BufferCount];

	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_CmdQueue;
	ComPtr<ID3D12CommandAllocator> m_CmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_CmdList;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	
	Timer gameTimer;

	void FlushCommandQueue();

	virtual void Create_RTV_DSV();
	virtual void Initialize();
	virtual void Draw() {}
	virtual void Update() {}
	virtual void MoveObj(int Sig) {}
	virtual void onMouseDown(WPARAM btnState, int x, int y) {}
	virtual void onMouseUp(WPARAM btnState, int x, int y) {}
	virtual void onMouseMove(WPARAM btnState, int x, int y) {}

private: //Only for this class
	
	HINSTANCE m_hInstance;
	HWND m_hMainWind;
	HANDLE m_fenceEvent;

	int m_width = 100;
	int m_height;
	

	UINT64 m_fenceValue;
		
	ComPtr<ID3D12Resource> m_DepthStencilSBuffer;
	ComPtr<ID3D12Fence> m_Fence;

	void InitMainWindow();
	void InitDirect3D();

	//void GetHardwareAdapter(IDXGIFactory2* pfactory, IDXGIAdapter1** ppadapter);
	void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

	void LogAdapters(); // Get list of Video Adapters
	void LogAdapterOutput(IDXGIAdapter1* pAdapter); //Get list of Monitors for specific Video Adapter
	void LogOutputDisplayMode(IDXGIOutput* pOutput, DXGI_FORMAT); // Get list of resolution (DisplayMode) for specific monitor for specific format;

};

/*
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif
*/

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		//Set a breakpoint to this line to catch DirectX API erros
		throw std::exception();
	}
}