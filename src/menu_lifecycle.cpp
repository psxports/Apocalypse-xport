#include "menu_lifecycle.h"
#include "object.h"

void DestroyMenuObject(Object *object)
{
    ObjectBorrow borrow;
    object->Destroy(3);
}
