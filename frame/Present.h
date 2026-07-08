#pragma once

struct PresentInit
{
	HWND hWindow;

	PresentInit(void)
	{
		this->hWindow = NULL;
	}

	PresentInit(HWND WindowHandle)
	{
		this->hWindow = WindowHandle;
	}

	PresentInit& operator = (const PresentInit& x)
	{
		this->hWindow = x.hWindow;
		return *this;
	}
};

class Present
{
public:
	Present(void);
	virtual ~Present(void);
	virtual void Create(const PresentInit& init) = 0;
	virtual void Render(void) = 0;
};

class PresentFactory
{
private:
	Present* present;
public:
	PresentFactory(void);
	~PresentFactory(void);
	Present* CreatePresent(const PresentInit& init);
	Present* PresentGet(void);
};
