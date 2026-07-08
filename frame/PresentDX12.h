#pragma once
#include "Present.h"

class PresentDX12 : public Present
{
public:
	PresentDX12(void);
	virtual ~PresentDX12(void);
	virtual void Create(const PresentInit& init);
	virtual void Render(void);
};

