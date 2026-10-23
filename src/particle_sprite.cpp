#include "sprite.h"
#include "particle_sprite.h"
#include "effect_lifecycle.h"
#include "global.h"

ParticleSprite::ParticleSprite(const Vector3 *pos, uint8 r, uint8 g, uint8 b, uint32 resource, uint16 size, bool semi_transparent, uint32 spread, uint32 acceleration, uint32 lifetime) : Sprite()
{
    uint32 random, range;
    FUNCTION_MARKER(0x800358B4, "SLUS_003.73");
    this->pos = *pos;
    SelectResource(resource);
    if (semi_transparent)
        EnableSemiTransparency();
    SetColors(r, g, b);
    range = (spread << 1) + 1;
    this->size = size;
    random = RandomRange(range);
    velocity.x = (random - spread) << 12;
    random = RandomRange(spread);
    velocity.y = -(random << 12);
    random = RandomRange(range);
    velocity.z = (random - spread) << 12;
    this->acceleration.y = acceleration;
    drag[0] = 3;
    drag[1] = 3;
    drag[2] = 3;
    this->lifetime = RandomRange(lifetime);
}

uint32 ParticleSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035A00, "SLUS_003.73");
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void ParticleSprite::Update()
{
    FUNCTION_MARKER(0x80035A54, "SLUS_003.73");
    pos.x = (uint32)pos.x + velocity.x;
    pos.y = (uint32)pos.y + velocity.y;
    pos.z = (uint32)pos.z + velocity.z;
    velocity.x = (uint32)velocity.x + acceleration.x;
    velocity.z = (uint32)velocity.z + acceleration.z;
    velocity.y = (uint32)velocity.y + acceleration.y;
    velocity.x = (uint32)velocity.x - (velocity.x >> (drag[0] & 31));
    velocity.z = (uint32)velocity.z - (velocity.z >> (drag[2] & 31));
    velocity.y = (uint32)velocity.y - (velocity.y >> (drag[1] & 31));
    // Frame and age comparisons use signed byte and halfword loads
    ++frame;
    if (frame >= frame_count)
        frame = frame_count - 1;
    ++age;
    if (age >= lifetime)
        Finish();
}
