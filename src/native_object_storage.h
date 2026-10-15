#ifndef NATIVE_OBJECT_STORAGE_H
#define NATIVE_OBJECT_STORAGE_H

#include <stddef.h>
extern "C"
{
#include "psx.h"
}

// ABI adapters own address translation and placement storage
void *AllocateNativeObjectStorage(uint32 guest_address, size_t size);
void *FindNativeObjectStorage(uint32 guest_address);
uint32 NativeObjectGuestAddress(const void *object);
uint32 GuestMemoryAddress(const void *pointer);

#endif
