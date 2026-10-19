#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>

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

class ObjectReferenceBase;
struct GameHeapBlock;

// Intrusive native retirement preserves destruction order across allocation owners
struct ObjectRetirement
{
    ObjectRetirement *next;
    void (*destroy)(void *owner);
    void *owner;
};

// Native lifetime interface shared by the game's allocation owners
class Object
{
public:
    static void *operator new(size_t size);
    static void *operator new(size_t size, GameHeapBlock *reservation);
    static void operator delete(void *object);
    static void operator delete(void *object, GameHeapBlock *reservation);
    virtual ~Object();
    virtual uint32 Destroy(uint32 reason) = 0;

    // Native lifetime state is separate from the original gameplay flags
    bool IsAlive() const
    {
        return alive;
    }

    void MarkDestroyed();

protected:
    Object();
    uint32 ReleaseStorage();

private:
    friend class ObjectReferenceBase;
    friend uint32 ReleaseObjectStorage(Object *object);
    friend uint32 ReleaseEffectStorage(Object *object);
    friend uint32 ReleaseWeaponStorage(Object *object);
    bool alive;
    ObjectReferenceBase *references;
    ObjectRetirement retirement;
};

// Intrusive native references expire at logical destruction without an address registry
class ObjectReferenceBase
{
public:
    ObjectReferenceBase();
    ObjectReferenceBase(const ObjectReferenceBase &other);
    ~ObjectReferenceBase();
    ObjectReferenceBase &operator=(const ObjectReferenceBase &other);
    void Bind(Object *target);
    void Reset();

    Object *GetObject() const
    {
        return object;
    }

private:
    friend class Object;
    Object *object;
    ObjectReferenceBase *prev;
    ObjectReferenceBase *next;
};

template <class T>
class ObjectReference : private ObjectReferenceBase
{
public:
    ObjectReference()
    {
    }

    explicit ObjectReference(T *target)
    {
        Bind(target);
    }

    ObjectReference(const ObjectReference &other) : ObjectReferenceBase(other)
    {
    }

    ~ObjectReference()
    {
    }

    ObjectReference &operator=(const ObjectReference &other)
    {
        ObjectReferenceBase::operator=(other);
        return *this;
    }

    ObjectReference &operator=(T *target)
    {
        Bind(target);
        return *this;
    }

    T *Get() const
    {
        return static_cast<T *>(GetObject());
    }

    T *operator->() const
    {
        return Get();
    }

    operator T *() const
    {
        return Get();
    }
};

#endif
