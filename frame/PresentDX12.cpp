#include "PCH.h"
#include "PresentDX12.h"
#include <iostream>
#include <stdexcept>

PresentDX12::PresentDX12() :
    hr(S_OK),
    init(),
    pDebugController(nullptr),
    FeatureLevel(D3D_FEATURE_LEVEL_11_0),
    pDevice(nullptr),
    pFence(nullptr),
    uiRtvDescriptorSize(0),
    uiDsvDescriptorSize(0),
    uiCbvDescriptorSize(0),
    pCommandQueue(nullptr),
    pCommandAllocator(nullptr),
    pCommandList(nullptr),
    pDxgiFactory(nullptr),
    pSwapChain(nullptr),
    pRtvHeap(nullptr),
    pDsvHeap(nullptr),
	uiCurrentFence(0)
{}

PresentDX12::~PresentDX12(void)
{
	SAFE_RELEASE(this->pDsvHeap)
	SAFE_RELEASE(this->pRtvHeap)
	SAFE_RELEASE(this->pSwapChain)
	SAFE_RELEASE(this->pDxgiFactory)
	SAFE_RELEASE(this->pCommandList)
	SAFE_RELEASE(this->pCommandAllocator)
	SAFE_RELEASE(this->pCommandQueue)
	SAFE_RELEASE(this->pFence)
	SAFE_RELEASE(this->pDebugController)
	SAFE_RELEASE(this->pDevice) // at the end
}

void PresentDX12::Create(const PresentInit& init)
{
	LOGD("The PresentDX12::Create method is being executed.")
	this->init = init;

	//TODO: SetFullscreen?
	//TODO: OnResize?
	//TODO: Resize buffers?

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	GetDefaultSwapChainDesc(SwapChainDesc);

	IDXGIAdapter1* pAdapter;
	GetDefaultOutputDesc(&pAdapter);

#if defined(DEBUG) || defined(_DEBUG) 	
	{
		LR(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)))
		LLE()
		pDebugController->EnableDebugLayer();		
	}
#endif

	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT DriverTypesCount = sizeof(DriverTypes) / sizeof(DriverTypes[0]);

	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	UINT FeatureLevelsCount = sizeof(FeatureLevels) / sizeof(FeatureLevels[0]); 

	this->FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	this->pDevice = nullptr;

	for (UINT DriverTypesIndex = 0; DriverTypesIndex < DriverTypesCount; DriverTypesIndex++)
	{
		D3D_DRIVER_TYPE DriverType = DriverTypes[DriverTypesIndex];
		LOGD("The PresentDX12::Create method is calling the D3D12CreateDevice method.");
		LR(D3D12CreateDevice(pAdapter, this->FeatureLevel, IID_PPV_ARGS(&pDevice)))
		if (hr == S_OK) break;
		if (hr == S_FALSE) 
		{
			LOGW("Warning! D3D11CreateDeviceAndSwapChain method returned alternate success value: S_FALSE.");
			break;
		}
		if (hr != S_OK && hr != S_FALSE)
		{
			throw std::runtime_error("EXCEPTION: The PresentDX12::Create method failed!");
		}
	}

	const char device_name[] = "PresentDX12::pDevice";
	this->pDevice->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(device_name) - 1, device_name);
	this->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
	this->uiRtvDescriptorSize = this->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	this->uiDsvDescriptorSize = this->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	this->uiCbvDescriptorSize = this->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//TODO: CheckFeatureSupport?
	CreateCommandQueueAndList();
	CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory));
	this->pDxgiFactory->CreateSwapChain(pCommandQueue, &SwapChainDesc, &pSwapChain);
	CreateDescriptorHeaps();

	LOGD("The PresentDX12::Create method has been executed.")
}

void PresentDX12::Render(void)
{
	//TODO: ?
}

void PresentDX12::OnResizeWindow(const unsigned short width, const unsigned short height)
{
	assert(this->pDevice);
	assert(this->pSwapChain);
	assert(this->pCommandAllocator);
	assert(this->pCommandList);

	FlushCommandQueue();
	LR(this->pCommandList->Reset(this->pCommandAllocator, nullptr))

	//for (int i = 0; i < PresentDX12::iSwapChainBufferCount; ++i) mSwapChainBuffer[i].Reset();
	//mDepthStencilBuffer.Reset();

	LR(this->pSwapChain->ResizeBuffers(PresentDX12::iSwapChainBufferCount, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH))
	this->uiCurrentBackBuffer = 0;
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(this->pRtvHeap->GetCPUDescriptorHandleForHeapStart());

	//TODO: dalej.....................
}

void PresentDX12::GetDefaultSwapChainDesc(DXGI_SWAP_CHAIN_DESC& SwapChainDesc) const
{
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	UINT width;
	UINT height;
	GetClientWidthAndHeight(width, height);

	//TODO: only one buffer including the front buffer?
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = width;
	SwapChainDesc.BufferDesc.Height = height;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // proper for D2D
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	//SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	//TODO: is rendering needed here (DXGI_USAGE_READ_ONLY)?
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = this->init.hWindow;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	//SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // because BufferCount = 1 
	SwapChainDesc.Windowed = true;
}

void PresentDX12::GetClientWidthAndHeight(UINT& uiWidth, UINT& uiHeight) const
{
	RECT rc;
	GetClientRect(this->init.hWindow, &rc);
	uiWidth = rc.right - rc.left;
	uiHeight = rc.bottom - rc.top;
}

DXGI_OUTPUT_DESC PresentDX12::GetDefaultOutputDesc(IDXGIAdapter1** ppAdapter) const
{
	DXGI_OUTPUT_DESC OutputDescription;
	ZeroMemory(&OutputDescription, sizeof(DXGI_OUTPUT_DESC));
	std::vector<IDXGIAdapter1*> vAdapters = EnumerateAdapters1();
	if (vAdapters.empty())
	{
		LOGE("There are no graphics adapters detected by IDXGIFactory interface!");
	}
	else
	{
		std::vector<IDXGIAdapter1*>::iterator itAdapters;
		for (itAdapters = vAdapters.begin(); itAdapters != vAdapters.end(); ++itAdapters)
		{
			std::vector<IDXGIOutput*> vOutputs = EnumerateOutputs(*itAdapters);
			if (vOutputs.empty())
			{
				LOGW("There are no outputs for one of graphics adapters detected by IDXGIFactory interface!");
			}
			else
			{
				std::vector<IDXGIOutput*>::iterator itOutputs;
				for (itOutputs = vOutputs.begin(); itOutputs != vOutputs.end(); ++itOutputs)
				{
					DXGI_OUTPUT_DESC TempOutputDescription = GetOutputDesc(*itOutputs);
					if (TempOutputDescription.AttachedToDesktop == TRUE)
					{
						*ppAdapter = *itAdapters;
						OutputDescription = TempOutputDescription;
						return OutputDescription;
					}
					else
					{
						LOG("Output not attached to desktop is being encountered.");
					}
				}
			}
		}
	}
	//TODO: how to get more optimal default output desc. (DXGI_OUTPUT_DESC)?
	return OutputDescription;
}

std::vector<IDXGIAdapter1*> PresentDX12::EnumerateAdapters1(void) const
{
	IDXGIAdapter1* pAdapter;
	std::vector<IDXGIAdapter1*> vAdapters;
	IDXGIFactory1* pFactory = NULL;

	LR(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory))
		if (FAILED(hr)) return vAdapters;

	for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
	}

	SAFE_RELEASE(pFactory)
	return vAdapters;
}

std::vector<IDXGIOutput*> PresentDX12::EnumerateOutputs(IDXGIAdapter1* const pAdapter) const
{
	IDXGIOutput* pOutput;
	std::vector<IDXGIOutput*> vOutputs;

	for (UINT i = 0; pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vOutputs.push_back(pOutput);
	}

	return vOutputs;
}

DXGI_OUTPUT_DESC PresentDX12::GetOutputDesc(IDXGIOutput* const pOutput) const
{
	DXGI_OUTPUT_DESC OutputDescription;
	ZeroMemory(&OutputDescription, sizeof(DXGI_OUTPUT_DESC));
	LR(pOutput->GetDesc(&OutputDescription));
	if (FAILED(hr)) LOGE("The IDXGIOutput::GetDesc method has failed!");
	return OutputDescription;
}

void PresentDX12::CreateCommandQueueAndList(void)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	this->pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue));
	this->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator));
	this->pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&pCommandList));
	this->pCommandList->Close();
}

void PresentDX12::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = PresentDX12::iSwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	this->pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pRtvHeap));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	this->pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pDsvHeap));
}

void PresentDX12::FlushCommandQueue(void)
{
	uiCurrentFence++;
	LR(this->pCommandQueue->Signal(this->pFence, uiCurrentFence))

	if (this->pFence->GetCompletedValue() < uiCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		LR(this->pFence->SetEventOnCompletion(uiCurrentFence, eventHandle))
		// ...		
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
