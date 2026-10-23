#include <new>

#include "game_heap.h"
#include "effect_object.h"
#include "object_fragments.h"
#include "game_diagnostics.h"
#include "textured_fragment.h"
#include "textured_triangle.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"

TexturedFragment::TexturedFragment(const ShortVector3 &v0, const ShortVector3 &v1, const ShortVector3 &v2, const Vector3 &pos) : TexturedTriangle(v0, v1, v2), lifetime(0), trail(0)
{
    GameHeapBlock *child;
    uint16 angle;
    FUNCTION_MARKER(0x80020FB4, "SLUS_003.73");
    if (RandomRange(2))
    {
        angle = RandomRange(160) + 80;
        angular_velocity.x = angle;
        angular_velocity.y = 0;
        angular_velocity.z = 0;
    }
    else
    {
        angle = RandomRange(160) + 80;
        angular_velocity.x = 0;
        angular_velocity.y = angle;
        angular_velocity.z = 0;
    }
    lifetime = RandomRange(30) + 45;
    InitializeVelocity(pos);
    TransformVertices();
    if (RandomRange(2) && !IsGameHeapUnderPressure())
    {
        child = EffectObject::AllocateStorage(100);
        if (child)
        {
            trail = new (child) RibbonTrail(&this->pos, 4, 128, 128, 128);
        }
        // TODO Original allocation failure still leaves no usable trail
        if (!trail)
        {
            GameDiagnostic(("Effect trail allocation failed\n"));
            abort();
        }
        trail->persistent = true;
        trail->SetWidth(1200);
        trail->SetColors(32, 32, 32);
    }
}

uint32 TexturedFragment::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800210D8, "SLUS_003.73");
    if (trail)
    {
        trail->Destroy(3);
        trail = 0;
    }
    TexturedTriangle::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

void TexturedFragment::InitializeVelocity(const Vector3 &src_pos)
{
    Vector3 difference, scaled;
    uint32 distance, factor;
    FUNCTION_MARKER(0x80021154, "SLUS_003.73");
    SetPos(&src_pos);
    difference.Subtract(src_pos, GetFragmentOrigin());
    distance = difference.Len();
    if (distance)
    {
        factor = RandomRange(48);
        scaled.Scale(factor, difference);
        velocity.Divide(scaled, distance);
    }
    else
        Finish();
    sint32 normal_scale = RandomRange(45);
    const ShortVector3 &n = GetFragmentNormal();
    for (uint32 axis = 0; axis < 3; ++axis)
        velocity.comp[axis] = (uint32)velocity.comp[axis] + (normal_scale * n.comp[axis]);
    uint32 rise = RandomRange(175);
    velocity.y = (uint32)velocity.y - (rise << 12);
    if (!RandomRange(20))
        velocity.ShiftLeft(1);
    if (velocity.y > 0)
        velocity.y = 0;
    if (trail)
        trail->SetPos(&pos);
}

void TexturedFragment::Update()
{
    FUNCTION_MARKER(0x800212C8, "SLUS_003.73");
    this->pos.Add(velocity);
    velocity.y = (uint32)velocity.y + (20500);
    angles.AddAngles(angular_velocity);
    if (trail)
        trail->SetPos(&pos);
    TransformVertices();
    if (!lifetime)
    {
        Finish();
        return;
    }
    --lifetime;
}

TexturedFragment::~TexturedFragment()
{
    if (trail)
        trail->Destroy(3);
}
