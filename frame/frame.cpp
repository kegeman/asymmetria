#include "PCH.h"
#include "Frame.h"
#include "DirectFrame.h"

// Frame
Frame::Frame(void) {}
Frame::~Frame(void) {}

// FrameFactory
FrameFactory::FrameFactory(void)
{
	this->frame = nullptr;
}

FrameFactory::~FrameFactory(void)
{
	SAFE_DELETE(this->frame);
}

Frame* FrameFactory::CreateFrame(const FrameInit& init)
{
	assert(this->frame == nullptr);
	this->frame = new DirectFrame();
	this->frame->Create(init);
	return this->frame;
}
