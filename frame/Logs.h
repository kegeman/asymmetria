#pragma once

namespace Auxiliary
{
	//////////////////////////////////////////////////////////////////////
	#if defined(LOGGING)
	#ifndef LOG
	#define LOG(x)			{ CharlieAux::Logs::Instance->WriteLine(CharlieAux::Logs::Information, x); }
	#endif
	#ifndef LOGD
	#define LOGD(x)			{ CharlieAux::Logs::Instance->WriteLine(CharlieAux::Logs::Debug, x); }
	#endif
	#ifndef LOGW
	#define LOGW(x)			{ CharlieAux::Logs::Instance->WriteLine(CharlieAux::Logs::Warning, x); }
	#endif
	#ifndef LOGE
	#define LOGE(x)			{ CharlieAux::Logs::Instance->WriteLine(CharlieAux::Logs::Error, x); }
	#endif
	#ifndef LLE
	#define LLE()           { CharlieAux::Logs::Instance->LogLastError(); }
	#endif
	#ifndef LR
	#define LR(x)			{ hr = (x); if( FAILED(hr) ) { CharlieAux::Logs::Instance->LogResult( hr, L#x); } }
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
	//////////////////////////////////////////////////////////////////////

	class Logs
	{};
}