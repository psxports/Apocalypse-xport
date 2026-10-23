#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"
#include "game_diagnostics.h"
#include "effect_lifecycle.h"
#include "vector3.h"
#include "effect_object.h"
#include "global.h"

RibbonTrail::RibbonTrail(const Vector3 *pos, uint32 segs, uint32 r, uint32 g, uint32 b) : SpriteTrail(pos, segs, 2, "Ribbon", "Ribbon", 400, 1), fading(false)
{
    FUNCTION_MARKER(0x80036B00, "SLUS_003.73");
    uint32 r_step = DivideSignedWord(r, segment_count);
    uint32 g_step = DivideSignedWord(g, segment_count);
    uint32 b_step = DivideSignedWord(b, segment_count);
    for (sint32 i = 0; i < segment_count; ++i)
    {
        uint32 weight = segment_count - i - 1;
        children[i]->SetColors((r - weight * r_step) & 255, (g - weight * g_step) & 255, (b - weight * b_step) & 255);
        children[i]->SetColorStep(8);
    }
}

uint32 RibbonTrail::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80036C5C, "SLUS_003.73");
    uint32 result = SpriteTrail::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void RibbonTrail::Update()
{
    FUNCTION_MARKER(0x80036CB0, "SLUS_003.73");
    if (fading)
    {
        bool all_empty = true;
        for (sint32 i = 0; i < segment_count; ++i)
            if (children[i] && !children[i]->FadeColors(0))
                all_empty = false;
        if (all_empty)
            Finish();
    }
}
