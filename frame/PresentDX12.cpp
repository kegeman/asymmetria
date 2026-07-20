#include "PCH.h"
#include "PresentDX12.h"

PresentDX12::PresentDX12(void) 
{
	this->pDebugController = nullptr;
}

PresentDX12::~PresentDX12(void)
{
	SAFE_RELEASE(this->pDebugController)
}

void PresentDX12::Create(const PresentInit& init)
{
	LOGD(L"The PresentDX12::Create method is being executed.")
	this->init = init;

	//TODO: SetFullscreen
	//TODO: OnResize
	//TODO: Resize buffers

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
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT FeatureLevelsCount = sizeof(FeatureLevels) / sizeof(FeatureLevels[0]); 


	//TODO: dalej.............

	LOGD(L"The PresentDX12::Create method has been executed.")
}

void PresentDX12::Render(void)
{
	//TODO:
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
		LOGE(L"There are no graphics adapters detected by IDXGIFactory interface!");
	}
	else
	{
		std::vector<IDXGIAdapter1*>::iterator itAdapters;
		for (itAdapters = vAdapters.begin(); itAdapters != vAdapters.end(); ++itAdapters)
		{
			std::vector<IDXGIOutput*> vOutputs = EnumerateOutputs(*itAdapters);
			if (vOutputs.empty())
			{
				LOGW(L"There are no outputs for one of graphics adapters detected by IDXGIFactory interface!");
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
						LOG(L"Output not attached to desktop is being encountered.");
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
	if (FAILED(hr)) LOGE(L"The IDXGIOutput::GetDesc method has failed!");
	return OutputDescription;
}
