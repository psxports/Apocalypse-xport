#include "game_sdk_bindings.h"
#include "native_object_storage.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(XPORT_XCRT)
#define XCRT_CPP_IMPLEMENTATION
#include "platform/win/xcrt.h"
#endif

struct NativeObjectStorage
{
    uint32 guest_address;
    void *address;
    Object *object;
    size_t size;
    NativeObjectStorage *guest_next;
    NativeObjectStorage *pointer_next;
};

static NativeObjectStorage *native_objects[1024];
static NativeObjectStorage *guest_objects[1024];

static NativeObjectStorage *FindStorage(uint32 guest_address)
{
    uint32 bucket = ((guest_address >> 4) ^ guest_address) & 1023;
    NativeObjectStorage *storage = native_objects[bucket];
    while (storage && storage->guest_address != guest_address)
        storage = storage->guest_next;
    return storage;
}

static size_t PointerBucket(const void *pointer)
{
    return (reinterpret_cast<size_t>(pointer) >> 4) & 1023;
}

static NativeObjectStorage *FindPointerStorage(const void *pointer)
{
    NativeObjectStorage *storage = guest_objects[PointerBucket(pointer)];
    while (storage && storage->address != pointer)
        storage = storage->pointer_next;
    return storage;
}

Object::Object()
{
    NativeObjectStorage *storage = FindPointerStorage(this);
    if (!storage || storage->object)
    {
        GameDiagnostic("Invalid native object construction\n");
        abort();
    }
    storage->object = this;
}

Object::~Object()
{
    NativeObjectStorage *storage = FindPointerStorage(this);
    if (!storage || storage->object != this)
    {
        GameDiagnostic("Invalid native object destruction\n");
        abort();
    }
    storage->object = 0;
}

void *AllocateNativeObjectStorage(uint32 guest_address, size_t size)
{
    NativeObjectStorage *storage = FindStorage(guest_address);
    if (!storage)
    {
        storage = (NativeObjectStorage *)malloc(sizeof(NativeObjectStorage));
        if (!storage)
            abort();
        uint32 bucket = ((guest_address >> 4) ^ guest_address) & 1023;
        storage->guest_address = guest_address;
        storage->address = 0;
        storage->object = 0;
        storage->size = 0;
        storage->guest_next = native_objects[bucket];
        storage->pointer_next = 0;
        native_objects[bucket] = storage;
    }
    if (storage->object)
        storage->object->~Object();
    if (storage->size < size)
    {
        if (storage->address)
        {
            NativeObjectStorage **previous = &guest_objects[PointerBucket(storage->address)];
            while (*previous != storage)
                previous = &(*previous)->pointer_next;
            *previous = storage->pointer_next;
            free(storage->address);
        }
        storage->address = malloc(size);
        if (!storage->address)
        {
            GameDiagnostic("Native object allocation failed at %08X\n", guest_address);
            abort();
        }
        storage->size = size;
        size_t bucket = PointerBucket(storage->address);
        storage->pointer_next = guest_objects[bucket];
        guest_objects[bucket] = storage;
    }
    return storage->address;
}

void ReleaseNativeObjectStorage(uint32 guest_address)
{
    NativeObjectStorage *storage = FindStorage(guest_address);
    if (!storage)
        return;
    if (storage->object)
        storage->object->~Object();
    NativeObjectStorage **previous = &native_objects[((guest_address >> 4) ^ guest_address) & 1023];
    while (*previous != storage)
        previous = &(*previous)->guest_next;
    *previous = storage->guest_next;
    previous = &guest_objects[PointerBucket(storage->address)];
    while (*previous != storage)
        previous = &(*previous)->pointer_next;
    *previous = storage->pointer_next;
    free(storage->address);
    free(storage);
}

void *FindNativeObjectStorage(uint32 guest_address)
{
    NativeObjectStorage *storage = FindStorage(guest_address);
    if (!storage)
    {
        GameDiagnostic("TODO Unconstructed native object at %08X\n", guest_address);
        abort();
    }
    return storage->address;
}

uint32 NativeObjectGuestAddress(const void *object)
{
    NativeObjectStorage *storage = guest_objects[PointerBucket(object)];
    while (storage && storage->address != object)
        storage = storage->pointer_next;
    if (!storage)
    {
        GameDiagnostic("TODO Native object has no guest storage\n");
        abort();
    }
    return storage->guest_address;
}
uint32 GuestMemoryAddress(const void *pointer)
{
    size_t address = reinterpret_cast<size_t>(pointer);
    size_t ram = reinterpret_cast<size_t>(psx_addr(0x80000000, 1));
    if (address >= ram && address - ram < 0x200000)
        return 0x80000000 + (uint32)(address - ram);
    size_t scratchpad = reinterpret_cast<size_t>(psx_addr(0x1F800000, 1));
    if (address >= scratchpad && address - scratchpad < 1024)
        return 0x1F800000 + (uint32)(address - scratchpad);
    return 0;
}
