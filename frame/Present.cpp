#include <assert.h>
#include "PCH.h"
#include "Present.h"
#include "PresentDX12.h"

// Present
Present::Present(void) {}
Present::~Present(void) {}

// PresentFactory
PresentFactory::PresentFactory(void) { this->present = nullptr; }
PresentFactory::~PresentFactory(void) { SAFE_DELETE(this->present); }
Present* PresentFactory::CreatePresent(const PresentInit& init)
{
	assert(this->present == nullptr);
	this->present = new PresentDX12();
	this->present->Create(init);
	return this->present;
}
Present* PresentFactory::PresentGet(void) { return this->present; }