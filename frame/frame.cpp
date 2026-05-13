// frame.cpp : Definiuje funkcje biblioteki statycznej.
//

#include "pch.h"
#include "framework.h"
#include "frame.h"

FrameFactory::FrameFactory(void)
{
	this->frame = NULL;
}

FrameFactory::~FrameFactory(void)
{
	SAFE_DELETE(this->frame);
}

Frame* FrameFactory::CreateFrame(const FrameInit& init)
{
	return nullptr;
}
