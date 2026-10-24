#include "endpoint_sprite.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "textured_effect.h"

EndpointSprite::EndpointSprite() : TexturedEffect(), hd_trail(0)
{
    for (uint32 end = 0; end < 2; ++end)
        for (uint32 axis = 0; axis < 3; ++axis)
            endpoint[end].comp[axis] = 0;
    FUNCTION_MARKER(0x80034E4C, "SLUS_003.73");
    AddToList(effect_lists.endpoint_sprs);
}

uint32 EndpointSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80034E90, "SLUS_003.73");
    RemoveFromList(effect_lists.endpoint_sprs);
    uint32 result = TexturedEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
