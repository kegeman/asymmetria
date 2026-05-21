#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p); (p)=nullptr; } }
#endif

struct FrameInit {
	HINSTANCE hInstance;
	int nCmdShow;
};

class Frame {
};

class FrameFactory {
private:
	Frame* frame;
public:
	FrameFactory(void);
	~FrameFactory(void);
	Frame* CreateFrame(const FrameInit& init);
};