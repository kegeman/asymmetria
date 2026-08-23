#pragma once

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
		#define D3D_DEBUG_INFO
	#endif
#endif

#include "Present.h"
#include <vector>
#include <dxgi.h>
// #include <dxgi1_2.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
//#include <d3dcompiler.h>
#include "d3dx12.h"

#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3d12.lib" )

class PresentDX12 : public Present
{
private:
	static const int iSwapChainBufferCount = 2;

private:
	mutable HRESULT hr;
	PresentInit init;
	ID3D12Debug* pDebugController;
	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D12Device* pDevice;
	ID3D12Fence* pFence;
	UINT uiRtvDescriptorSize;
	UINT uiDsvDescriptorSize;
	UINT uiCbvDescriptorSize;
	ID3D12CommandQueue* pCommandQueue;
	ID3D12CommandAllocator* pCommandAllocator;
	ID3D12GraphicsCommandList* pCommandList;
	IDXGIFactory1* pDxgiFactory;
	IDXGISwapChain* pSwapChain;
	ID3D12DescriptorHeap* pRtvHeap;
	ID3D12DescriptorHeap* pDsvHeap; // Depth Stencil View
	UINT64 uiCurrentFence;
	UINT uiCurrentBackBuffer;
	ID3D12Resource* pSwapChainBuffer[iSwapChainBufferCount];
	ID3D12Resource* pDepthStencilBuffer;
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

public:
	PresentDX12(void);
	virtual ~PresentDX12(void);
	virtual void Create(const PresentInit& init);
	virtual void Render(void);
	virtual void OnResizeWindow(const unsigned short width, const unsigned short height); // WM_SIZE

private:
	void GetDefaultSwapChainDesc(DXGI_SWAP_CHAIN_DESC& SwapChainDesc) const;
	void GetClientWidthAndHeight(UINT& uiWidth, UINT& uiHeight) const;
	DXGI_OUTPUT_DESC GetDefaultOutputDesc(IDXGIAdapter1** ppAdapter) const;
	std::vector<IDXGIAdapter1*> EnumerateAdapters1(void) const;
	std::vector<IDXGIOutput*> EnumerateOutputs(IDXGIAdapter1* const pAdapter) const;
	DXGI_OUTPUT_DESC GetOutputDesc(IDXGIOutput* const pOutput) const;
	void CreateCommandQueueAndList(void);
	void CreateDescriptorHeaps(void);
	void FlushCommandQueue(void);
};
