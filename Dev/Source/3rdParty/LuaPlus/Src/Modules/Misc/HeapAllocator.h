#ifndef MISC_HEAPALLOCATOR_H
#define MISC_HEAPALLOCATOR_H

class HeapAllocator
{
public:
	void* Alloc(size_t size)					{  return new unsigned char[size];  }
	void Free(void* ptr)						{  delete[] (unsigned char*)ptr;  }
	void FreeAll()								{  }
	void SetBlockSize(int)						{  }
	void SetObjectSize(int)						{  }
};

#endif // MISC_HEAPALLOCATOR_H
