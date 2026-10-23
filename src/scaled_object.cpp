#include "scaled_object.h"

ScaledObject::ScaledObject()
{
    FUNCTION_MARKER(0x80062628, "SLUS_003.73");
    model.owner = this;
    model.scale.x = ONE;
    model.scale.y = ONE;
    model.scale.z = ONE;
}

uint32 ScaledObject::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    if (model.flags & 1024)
        ModelInstance::ReleaseFlash(model.flash);
    MarkDestroyed();
    model.InvalidateReferences();
    if (reason & 1)
        return ReleaseStorage();
    return 0;
}
