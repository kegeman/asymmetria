#pragma once

struct FrameInit {
	HINSTANCE hInstance;
	int nCmdShow;
	const wchar_t* WindowTitle;
};

class Frame {
public:
	Frame(void);
	virtual ~Frame(void);
	virtual void Create(const FrameInit& init) = 0;
	virtual int Run(void) = 0;
};

class FrameFactory {
private:
	Frame* frame;
public:
	FrameFactory(void);
	~FrameFactory(void);
	Frame* CreateFrame(const FrameInit & init);
};