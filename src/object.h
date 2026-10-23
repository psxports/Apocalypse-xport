#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>

#include "object_reference_base.h"

extern "C"
{
#include "psx.h"
}

void BeginObjectBorrow();
void EndObjectBorrow();

class ObjectBorrow
{
public:
    ObjectBorrow()
    {
        BeginObjectBorrow();
    }

    ~ObjectBorrow()
    {
        EndObjectBorrow();
    }

private:
    ObjectBorrow(const ObjectBorrow &);
    ObjectBorrow &operator=(const ObjectBorrow &);
};

class Object;
class ObjectReferenceBase;
struct GameHeapBlock;

// Intrusive native retirement preserves destruction order across allocation owners
struct ObjectRetirement
{
    ObjectRetirement *next;
    void (*destroy)(Object *owner);
    Object *owner;
};

// Native lifetime interface shared by the game's allocation owners
class Object
{
public:
    virtual uint32 Destroy(uint32 reason) = 0;

    // Native lifetime state is separate from the original gameplay flags
    bool IsAlive() const
    {
        return alive;
    }

private:
    friend class ObjectReferenceBase;

    bool alive;
    ObjectReferenceBase *references;
    ObjectRetirement retirement;

public:
    static void *operator new(size_t size);
    static void *operator new(size_t size, GameHeapBlock *reservation);
    static void operator delete(void *object);
    static void operator delete(void *object, GameHeapBlock *reservation);
    virtual ~Object();
    void MarkDestroyed();

protected:
    Object();
    uint32 ReleaseStorage();
};

#endif
