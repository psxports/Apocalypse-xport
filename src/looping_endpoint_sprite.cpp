#include "endpoint_sprite.h"
#include "looping_endpoint_sprite.h"
#include "effect_lifecycle.h"
#include "global.h"

LoopingEndpointSprite::LoopingEndpointSprite() : EndpointSprite()
{
    FUNCTION_MARKER(0x8003658C, "SLUS_003.73");
}

uint32 LoopingEndpointSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800365C0, "SLUS_003.73");
    uint32 result = EndpointSprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void LoopingEndpointSprite::Update()
{
    FUNCTION_MARKER(0x80036614, "SLUS_003.73");
    AdvanceLoopingFrame();
}
