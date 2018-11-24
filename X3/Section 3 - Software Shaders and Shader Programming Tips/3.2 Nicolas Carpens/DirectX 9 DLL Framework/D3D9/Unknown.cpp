#include "Unknown.hpp"

#include "Error.hpp"

namespace D3D9
{
	Unknown::Unknown()
	{
		referenceCount = 1;
	}

	Unknown::~Unknown()
	{
		if(referenceCount != 0)
		{
			throw INTERNAL_ERROR;
		}
	}

	unsigned long Unknown::AddRef()
	{
		referenceCount++;

		return referenceCount;
	}

	unsigned long Unknown::Release()
	{
		int current = referenceCount--;

		if(!referenceCount)
		{
			delete this;
		}

		return current;
	}
}