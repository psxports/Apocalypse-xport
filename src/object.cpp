#include <stdlib.h>

#include "game_diagnostics.h"
#include "object.h"
#include "game_object_allocation.h"
#include "game_heap.h"

#if defined(XPORT_XCRT)
    #define XCRT_CPP_IMPLEMENTATION
    #include "platform/win/xcrt.h"
#endif

// Native block metadata is independent of the original object layout
union ObjectAllocationHeader
{
    GameHeapBlock *reservation;
    double alignment;
};

static uint32 object_borrows;
static bool draining_objects;
static ObjectRetirement *retired_objects;
static ObjectRetirement *retired_objects_tail;

static ObjectAllocationHeader *GetAllocationHeader(void *object)
{
    return (ObjectAllocationHeader *)((uint8 *)object - sizeof(ObjectAllocationHeader));
}

static void *AllocateObjectMemory(GameHeapBlock *reservation, size_t size)
{
    ObjectAllocationHeader *header = (ObjectAllocationHeader *)malloc(sizeof(*header) + size);
    if (!header)
    {
        GameDiagnostic(("Native object allocation failed\n"));
        abort();
    }
    header->reservation = reservation;
    return header + 1;
}

static void RetireObject(ObjectRetirement *retirement)
{
    retirement->next = 0;
    if (object_borrows || draining_objects)
    {
        if (retired_objects_tail)
            retired_objects_tail->next = retirement;
        else
            retired_objects = retirement;
        retired_objects_tail = retirement;
    }
    else
        retirement->destroy(retirement->owner);
}

static void DeleteNativeObject(Object *owner)
{
    delete owner;
}

void *Object::operator new(size_t size)
{
    return AllocateObjectMemory(0, size);
}

void Object::operator delete(void *object)
{
    if (object)
        free(GetAllocationHeader(object));
}

Object::Object() : alive(true), references(0)
{
    retirement.next = 0;
    retirement.destroy = 0;
    retirement.owner = 0;
}

Object::~Object()
{
    MarkDestroyed();
    ObjectAllocationHeader *header = GetAllocationHeader(this);
    GameHeapBlock *reservation = header->reservation;
    header->reservation = 0;
    if (reservation)
        ReleaseHeapBlock(reservation);
}

uint32 Object::ReleaseStorage()
{
    if (retirement.destroy)
        return 0;
    ObjectAllocationHeader *header = GetAllocationHeader(this);
    GameHeapBlock *reservation = header->reservation;
    header->reservation = 0;
    MarkDestroyed();
    retirement.destroy = DeleteNativeObject;
    retirement.owner = this;
    RetireObject(&retirement);
    // Immediate retirement can delete this before the budget is released
    return reservation ? ReleaseHeapBlock(reservation) : 0;
}

void Object::MarkDestroyed()
{
    alive = false;
    while (references)
        references->Reset();
}

void BeginObjectBorrow()
{
    ++object_borrows;
}

void EndObjectBorrow()
{
    --object_borrows;
    if (object_borrows || draining_objects)
        return;
    draining_objects = true;
    while (retired_objects)
    {
        ObjectRetirement *retirement = retired_objects;
        retired_objects = retirement->next;
        if (!retired_objects)
            retired_objects_tail = 0;
        retirement->destroy(retirement->owner);
    }
    draining_objects = false;
}

void *Object::operator new(size_t size, GameHeapBlock *reservation)
{
    return AllocateObjectMemory(reservation, size);
}

void Object::operator delete(void *object, GameHeapBlock *)
{
    Object::operator delete(object);
}

GameHeapBlock *AllocateZeroedObject(uint32 size)
{
    FUNCTION_MARKER(0x800625AC, "SLUS_003.73");
    return AllocateHeapBlock(size, 0, 1);
}
