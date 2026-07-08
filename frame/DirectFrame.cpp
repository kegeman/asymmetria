#include "PCH.h"
#include "DirectFrame.h"
#include "Present.h"

const LPCWSTR DirectFrame::sClassName = L"DirectFrame";

DirectFrame::DirectFrame(void) : hApplicationInstance(nullptr), hWindow(nullptr) {}

DirectFrame::~DirectFrame(void)
{
	UnregisterWindowsClass();
}

void DirectFrame::Create(const FrameInit& init)
{
	// Fields initialisation
	this->hApplicationInstance = init.hInstance;

	// The Windows' class and the window itself
	RegisterWindowsClass();
	hWindow = CreateInstanceWindow(init.WindowTitle);
	ShowWindow(hWindow, init.nCmdShow);
	UpdateWindow(hWindow);

	// Presenting mechanisms
	PresentInit presentInit = PresentInit(hWindow);
	presentFactory.CreatePresent(presentInit);

	LOG(L"The DirectFrame is ready to run.");
}

int DirectFrame::Run(void)
{
	//TODO: Opracować inny sposób animacji od SetFrameMoveEventTimer();

	LOG(L"Run! Entering into the main message loop.");
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

void DirectFrame::RegisterWindowsClass()
{
	WNDCLASSEX wcex;

	BOOL bClassExists = GetClassInfoEx(hApplicationInstance, sClassName, &wcex);
	if (bClassExists == 0)
	{
		LOGD(L"Windows' class registration.");
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = &DirectFrame::StaticMsgProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = this->hApplicationInstance;
		wcex.hIcon = NULL;
		//TODO: LoadCursorFromFile()
		wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = sClassName;
		wcex.hIconSm = NULL;

		if (RegisterClassEx(&wcex) != 0) return;
		LLE()
		//TODO: throw Exception(L"RegisterWindowsClass method has failed!");
	}
}

BOOL DirectFrame::UnregisterWindowsClass() const
{
	LOGD(L"Windows' class unregistration.");
	return UnregisterClass(sClassName, this->hApplicationInstance);
}

HWND DirectFrame::CreateInstanceWindow(const wchar_t* sWindowTitle)
{
	int width = 1920 / 3;
	int height = 1200 / 3;
	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_SYSMENU;

#if defined(ADJUST_WINDOW_RECTANGLE)
	RECT rc = { 0, 0, width, height };
	if (AdjustWindowRect(&rc, style, FALSE) == 0)
	{
		LLE()
		//TODO: throw Exception(L"CreateInstanceWindow method failed!");		
	}
	width = static_cast<int>(rc.right) - static_cast<int>(rc.left);
	height = static_cast<int>(rc.bottom) - static_cast<int>(rc.top);
#endif 

	hWindow = CreateWindow(sClassName, sWindowTitle, style, 100, 100, width, height, NULL, NULL, hApplicationInstance, this);
	if (hWindow == NULL)
	{
		LLE();
		//TODO: throw Exception(L"CreateInstanceWindow method failed!");
	}
	return hWindow;
}

LRESULT WINAPI DirectFrame::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static ULONG ulFrameNumber = 0;
	switch (msg)
	{
		//case WM_LBUTTONDOWN:
		//case WM_MBUTTONDOWN:
		//case WM_RBUTTONDOWN:
		//	//OUTPUT("WM_xBUTTONDOWN is being received by MsgProc function.");
		//	MouseCapture(msg);
		//	qMouseMessages.push(SYSTEM_MESSAGE(msg,wParam,lParam));
		//	return 0;
		//case WM_LBUTTONUP:
		//case WM_MBUTTONUP:
		//case WM_RBUTTONUP:
		//	//OUTPUT("WM_xBUTTONUP is being received by MsgProc function.");		
		//	MouseCapture(msg);
		//	qMouseMessages.push(SYSTEM_MESSAGE(msg,wParam,lParam));
		//	return 0;
		//case WM_MOUSEMOVE:
		//case WM_MOUSEWHEEL:
		//	qMouseMessages.push(SYSTEM_MESSAGE(msg,wParam,lParam));
		//	return 0;
		case WM_KEYDOWN:
			{
				//OUTPUT("WM_KEYDOWN is being received by MsgProc function.");
				//qKeysMessages.push(SYSTEM_MESSAGE(msg,wParam,lParam));
				switch (wParam)
				{
				case VK_ESCAPE:
					//TODO: The game engine should know something about this event!
					PostQuitMessage(0);
					break;
				}
			}
			return 0;
		//case WM_KEYUP:
		//	{
		//		//OUTPUT("WM_KEYUP is being received by MsgProc function.");
		//		qKeysMessages.push(SYSTEM_MESSAGE(msg,wParam,lParam));
		//	}
		//	return 0;
		case WM_SIZE:
			LOGD(L"WM_SIZE is being received by MsgProc function and not processed.");
			//	for(unsigned i = 0; i < RENDERING_DEVICE_COUNT; i++)
			//		this->aRenderingDevice[i]->OnResizeWindow(LOWORD(lParam), HIWORD(lParam));
				//TODO: Returning 0 or not would be better?
			break;
		case WM_CLOSE:
			LOGD(L"WM_CLOSE is being received by MsgProc function.");
			PostQuitMessage(0);
			return 0;
		case WM_DESTROY:
			LOGD(L"WM_DESTROY is being received by MsgProc function.");
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			//TODO: PresentFactory.PresentGet()->Render();
			 PAINTSTRUCT ps;
			 HDC hdc;
			 hdc = BeginPaint( hWnd, &ps );
			 // We can rise OnWindowPaint event here...
			 EndPaint( hWnd, &ps );
			break;
		case WM_TIMER:
			//if (wParam == uiFrameMoveTimerID)
			//{
				//(pOnFrameMoveHandler)(++ulFrameNumber);
				//while(!qMouseMessages.empty()) qMouseMessages.pop();
				//while(!qKeysMessages.empty()) qKeysMessages.pop();
			//}
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI DirectFrame::StaticMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)cs->lpCreateParams);
	}

	DirectFrame* pEngine = (DirectFrame*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	return pEngine->MsgProc(hWnd, message, wParam, lParam);
}
