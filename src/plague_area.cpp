#include <new>

#include "game_heap.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "plague_area.h"
#include "effect_lifecycle.h"
#include "vector_distance.h"
#include "object_damage.h"
#include "game_random.h"
#include "panel_particle.h"

// Recovered inline base construction and table assignment at 800464EC..80046504
PlagueArea::PlagueArea() : TexturedQuad(), age(0), lifetime(0), radius(0)
{
    center.x = center.y = center.z = 0;
}

uint32 PlagueArea::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80047A54, "SLUS_003.73");
    uint32 result = TexturedQuad::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

void PlagueArea::Update()
{
    uint32 age;
    uint32 distance;
    uint32 radius;
    uint32 lifetime;
    uint32 count;
    uint32 random;
    GameHeapBlock *child;
    ModelInstance *player;
    Vector3 position;
    ShortVector3 direction;
    FUNCTION_MARKER(0x80047844, "SLUS_003.73");
    age = this->age;
    player = visual_lists.players;
    this->age = age + 1;
    if (player)
    {
        distance = MeasureHorizontalDistance(center.words, player->pos.words);
        radius = this->radius;
        radius = (uint32)((sint32)(radius << 7) >> 7);
        if (distance < radius)
        {
            player = visual_lists.players;
            if (static_cast<VisualObject *>(player->owner)->contact_flags & 2)
                static_cast<VisualObject *>(player->owner)->Damage(1, zero_vector.components, 0);
        }
    }
    age = (uint32)(sint32)(sint16)this->age;
    if ((sint32)age < 38)
    {
        radius = this->radius;
        this->radius = radius + 4;
        direction.components[2] = 0;
        direction.components[0] = 0;
        direction.components[1] = -4096;
        SetPosition(center.words, direction.components, this->radius, this->radius);
    }
    age = (uint32)(sint32)(sint16)this->age;
    lifetime = this->lifetime;
    if ((sint32)age >= (sint32)lifetime)
    {
        Finish();
        return;
    }
    if ((sint32)age >= (sint32)(lifetime - 30))
    {
        radius = this->radius;
        if ((sint32)radius < 9)
        {
            Finish();
            return;
        }
        this->radius = radius - 6;
        direction.components[2] = 0;
        direction.components[0] = 0;
        direction.components[1] = -4096;
        SetPosition(center.words, direction.components, this->radius, this->radius);
        return;
    }
    count = RandomRange(3);
    while (count)
    {
        position.words[0] = center.x;
        position.words[1] = center.y;
        position.words[2] = center.z;
        random = RandomRange(140);
        position.words[0] += (random - 70) << 12;
        random = RandomRange(140);
        position.words[2] += (random - 70) << 12;
        child = AllocateEffectStorage(116);
        if (child)
        {
            new (child) PanelParticle(position.words);
        }
        count -= 1;
    }
}
