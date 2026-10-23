#include <new>

#include "game_heap.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "plague_area.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "object_damage.h"
#include "panel_particle.h"
#include "textured_quad.h"
#include "effect_object.h"

// Recovered inline base construction and table assignment at 800464EC..80046504
PlagueArea::PlagueArea() : TexturedQuad(), age(0), lifetime(0), radius(0)
{
    center.x = center.y = center.z = 0;
}

void PlagueArea::Update()
{
    sint32 age;
    uint32 distance;
    uint32 radius;
    sint32 lifetime;
    uint32 count;
    sint32 random;
    GameHeapBlock *child;
    ModelInstance *player;
    Vector3 pos;
    ShortVector3 dir;
    FUNCTION_MARKER(0x80047844, "SLUS_003.73");
    player = visual_lists.players;
    ++this->age;
    if (player)
    {
        distance = center.HorizontalDistance(player->pos);
        radius = this->radius;
        radius = (uint32)((sint32)(radius << 7) >> 7);
        if (distance < radius)
        {
            player = visual_lists.players;
            if (static_cast<VisualObject *>(player->owner)->contact_flags & 2)
                static_cast<VisualObject *>(player->owner)->Damage(1, &zero_vector, 0);
        }
    }
    age = this->age;
    if (age < 38)
    {
        this->radius += 4;
        dir.z = 0;
        dir.x = 0;
        dir.y = -ONE;
        SetPos(&center, &dir, this->radius, this->radius);
    }
    age = this->age;
    lifetime = this->lifetime;
    if (age >= lifetime)
    {
        Finish();
        return;
    }
    if (age >= lifetime - 30)
    {
        radius = this->radius;
        if ((sint32)radius < 9)
        {
            Finish();
            return;
        }
        this->radius = radius - 6;
        dir.z = 0;
        dir.x = 0;
        dir.y = -ONE;
        SetPos(&center, &dir, this->radius, this->radius);
        return;
    }
    count = RandomRange(3);
    while (count)
    {
        pos = center;
        random = RandomRange(140);
        pos.x = (uint32)pos.x + ((random - 70) * ONE);
        random = RandomRange(140);
        pos.z = (uint32)pos.z + ((random - 70) * ONE);
        child = EffectObject::AllocateStorage(116);
        if (child)
        {
            new (child) PanelParticle(&pos);
        }
        count -= 1;
    }
}

uint32 PlagueArea::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80047A54, "SLUS_003.73");
    uint32 result = TexturedQuad::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}
