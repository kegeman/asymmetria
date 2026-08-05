#pragma once
#include "Framework.h"
#include <iostream>

#if defined(LOGGING)
	#ifndef LOG
	#define LOG(x)			{ std::cout << "INFO: " << x << std::endl; }
	#endif
	#ifndef LOGD
	#define LOGD(x)			{ std::cout << "DEBUG: " << x << std::endl; }
	#endif
	#ifndef LOGW
	#define LOGW(x)			{ std::cout << "WARN: " << x << std::endl; }
	#endif
	#ifndef LOGE
	#define LOGE(x)			{ std::cout << "ERROR: " << x << std::endl; }
	#endif
	#ifndef LLE
	#define LLE()           { Logs::LogLastError(); }
	#endif
	#ifndef LR
	#define LR(x)			{ hr = (x); if( FAILED(hr) ) { Logs::LogResult( hr, L#x); } }
	#endif
#else
	#ifndef LOG
	#define LOG(x)			{}
	#endif
	#ifndef LOGD
	#define LOGD(x)			{}
	#endif
	#ifndef LOGW
	#define LOGW(x)			{}
	#endif
	#ifndef LOGE
	#define LOGE(x)			{}
	#endif
	#ifndef LLE
	#define LLE()			{}
	#endif
	#ifndef LR
	#define LR(x)			{ hr = (x); }
	#endif
#endif

class Logs
{
public:
	static void LogLastError(void);
	static void LogResult(HRESULT hr, const wchar_t* sMessage);
};