#include "PCH.h"
#include "Logs.h"

void Logs::LogLastError(void)
{
	DWORD dwError = GetLastError();
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	std::cout << "ERROR: " << lpMsgBuf << std::endl;
}

void Logs::LogResult(HRESULT hr, const wchar_t* sMessage)
{
	size_t nBufSize = _tcslen(sMessage) + 23;
	wchar_t* sBuf = new wchar_t[nBufSize];
	swprintf_s(sBuf, nBufSize, L"%s returned: 0x%x.", sMessage, hr);
	std::cout << "ERROR: " << &sBuf << std::endl;
	SAFE_DELETE(sBuf);
}
