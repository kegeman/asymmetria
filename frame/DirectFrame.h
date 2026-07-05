#pragma once

#include "Frame.h"

#define ADJUST_WINDOW_RECTANGLE

class DirectFrame : public Frame
{
private:
	HINSTANCE hApplicationInstance;
	HWND hWindow;
public:
	DirectFrame(void);
	virtual ~DirectFrame(void);
	virtual void Create(const FrameInit& init);	
	virtual int Run(void);
private:
	void RegisterWindowsClass();
	BOOL UnregisterWindowsClass() const;
	HWND CreateInstanceWindow(const wchar_t* sWindowTitle);
	LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	static const LPCWSTR sClassName;
	static LRESULT WINAPI StaticMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
