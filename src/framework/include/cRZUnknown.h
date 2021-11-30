#include <cassert>
#include "cIGZUnknown.h"

class cRZUnknown : public cIGZUnknown
{
public:
	cRZUnknown() : refCount(0) { }
	~cRZUnknown() { }

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

protected:
	uint32_t refCount;
};