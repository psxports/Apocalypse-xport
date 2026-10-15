extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "effect_sprite.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "object_lifecycle.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for the original position and velocity loads

// Native constructors and legacy bridges share the original field operations

// Field adapters preserve legacy construction without changing native type

// TODO Recover the original property name for halfword92

// Legacy entry also serves effects awaiting native construction

// Legacy entry also serves effects awaiting native construction

// Legacy entries also serve effects awaiting native construction

// Legacy entries preserve memory access before all derived effects have native construction

static uint32 ReadSpritePositionWord(const uint32 *position, uint32 axis)
{
    uint32 guest = GuestMemoryAddress(position), value;
    if (guest)
        return r_u32(guest + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

Sprite &Sprite::Attach(uint32 object)
{
    return *static_cast<Sprite *>(FindNativeObjectStorage(object));
}

AnimatedSprite &AnimatedSprite::Attach(uint32 object)
{
    return *static_cast<AnimatedSprite *>(FindNativeObjectStorage(object));
}

MovingSprite &MovingSprite::Attach(uint32 object)
{
    return *static_cast<MovingSprite *>(FindNativeObjectStorage(object));
}

ParticleSprite &ParticleSprite::Attach(uint32 object)
{
    return *static_cast<ParticleSprite *>(FindNativeObjectStorage(object));
}

EndpointSprite &EndpointSprite::Attach(uint32 object)
{
    return *static_cast<EndpointSprite *>(FindNativeObjectStorage(object));
}

LoopingEndpointSprite &LoopingEndpointSprite::Attach(uint32 object)
{
    return *static_cast<LoopingEndpointSprite *>(FindNativeObjectStorage(object));
}

Sprite::Sprite() : TexturedEffect()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80034D88, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CD8);
    AddToList((uint32 *)psx_addr(0x800FF438, 4));
    w_u8(object + 109, 32);
    w_u16(object + 102, 1);
    w_u32(object + 104, 0x10001000);
}

uint32 Sprite::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80034DE8, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CD8);
    RemoveFromList((uint32 *)psx_addr(0x800FF438, 4));
    uint32 result = TexturedEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

EndpointSprite::EndpointSprite() : TexturedEffect()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80034E4C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CC0);
    AddToList((uint32 *)psx_addr(0x800FF43C, 4));
}

uint32 EndpointSprite::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80034E90, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CC0);
    RemoveFromList((uint32 *)psx_addr(0x800FF43C, 4));
    uint32 result = TexturedEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

AnimatedSprite::AnimatedSprite(const uint32 *position, const char *name, uint32 size, uint32 initialize, uint32 first, uint32 last)
    : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x800353A4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C78);
    x = ReadSpritePositionWord(position, 0);
    y = ReadSpritePositionWord(position, 1);
    z = ReadSpritePositionWord(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    SelectResource(name);
    w_u16(object + 94, size);
    if (initialize)
        EnableSemiTransparency();
    w_u32(object + 112, first);
    if (last == (uint32)-1)
        last = r_u8(object + 89) - 1;
    w_u32(object + 116, last);
}

AnimatedSprite::AnimatedSprite(const uint32 *position, uint32 resource, uint32 size, uint32 initialize, uint32 first, uint32 last)
    : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x80035478, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C78);
    x = ReadSpritePositionWord(position, 0);
    y = ReadSpritePositionWord(position, 1);
    z = ReadSpritePositionWord(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    SelectResource(resource);
    w_u16(object + 94, size);
    if (initialize)
        EnableSemiTransparency();
    w_u32(object + 112, first);
    if (last == (uint32)-1)
        last = r_u8(object + 89) - 1;
    w_u32(object + 116, last);
}

uint32 AnimatedSprite::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003554C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C78);
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 AnimatedSprite::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 looping, frame, limit, count;
    FUNCTION_MARKER(0x800355A0, "SLUS_003.73");
    AdvanceFrame();
    looping = r_u32(object + 112);
    if (looping)
    {
        frame = (uint32)(sint32)(sint8)r_u8(object + 90);
        limit = r_u32(object + 116);
        if ((sint32)frame < (sint32)limit)
            return 1;
        return Finish();
    }
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    count = r_u8(object + 89);
    if ((sint32)frame < (sint32)count)
        return (uint32)-2;
    limit = r_u32(object + 116);
    return SetFrame(limit == (uint32)-2 ? count - 1 : 0);
}

MovingSprite::MovingSprite(const uint32 *position, const uint32 *velocity, const char *name, uint32 size, uint32 lifetime, uint32 mode)
    : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x80035638, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C60);
    x = ReadSpritePositionWord(position, 0);
    y = ReadSpritePositionWord(position, 1);
    z = ReadSpritePositionWord(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    x = ReadSpritePositionWord(velocity, 0);
    y = ReadSpritePositionWord(velocity, 1);
    z = ReadSpritePositionWord(velocity, 2);
    w_u32(object + 36, x);
    w_u32(object + 40, y);
    w_u32(object + 44, z);
    SelectResource(name);
    EnableSemiTransparency();
    w_u16(object + 94, size);
    w_u16(object + 74, lifetime);
    w_u16(object + 72, mode);
}

uint32 MovingSprite::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80035704, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C60);
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 MovingSprite::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80035758, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    uint32 result = Shrink(1);
    if (!result)
    {
        result = FadeColors(1);
        if (!result)
            return AdvanceLoopingFrame();
    }
    return result;
}

ParticleSprite::ParticleSprite(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 resource, uint32 size, uint32 initialize, uint32 spread, uint32 acceleration, uint32 lifetime)
    : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, random, range;
    FUNCTION_MARKER(0x800358B4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C30);
    x = ReadSpritePositionWord(position, 0);
    y = ReadSpritePositionWord(position, 1);
    z = ReadSpritePositionWord(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    SelectResource(resource);
    if (initialize)
        EnableSemiTransparency();
    SetColors(red & 255, green & 255, blue & 255);
    range = (spread << 1) + 1;
    w_u16(object + 94, size);
    random = RandomRange(range);
    w_u32(object + 36, (random - spread) << 12);
    random = RandomRange(spread);
    w_u32(object + 40, 0 - (random << 12));
    random = RandomRange(range);
    w_u32(object + 44, (random - spread) << 12);
    w_u32(object + 52, acceleration);
    w_u8(object + 60, 3);
    w_u8(object + 61, 3);
    w_u8(object + 62, 3);
    w_u16(object + 10, RandomRange(lifetime));
}

uint32 ParticleSprite::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80035A00, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C30);
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 ParticleSprite::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, velocity_x, velocity_y, velocity_z, acceleration, shift_x, shift_y, shift_z, frame, count, age, lifetime;
    FUNCTION_MARKER(0x80035A54, "SLUS_003.73");
    x = r_u32(object + 24);
    velocity_x = r_u32(object + 36);
    y = r_u32(object + 28);
    velocity_y = r_u32(object + 40);
    shift_y = r_u8(object + 61);
    w_u32(object + 24, x + velocity_x);
    z = r_u32(object + 32);
    velocity_z = r_u32(object + 44);
    w_u32(object + 28, y + velocity_y);
    velocity_x = r_u32(object + 36);
    acceleration = r_u32(object + 48);
    w_u32(object + 32, z + velocity_z);
    velocity_y = r_u32(object + 40);
    y = r_u32(object + 52);
    w_u32(object + 36, velocity_x + acceleration);
    velocity_z = r_u32(object + 44);
    acceleration = r_u32(object + 56);
    velocity_y += y;
    w_u32(object + 44, velocity_z + acceleration);
    shift_x = r_u8(object + 60);
    frame = r_u8(object + 90);
    w_u32(object + 40, velocity_y);
    velocity_x = r_u32(object + 36);
    velocity_y = r_u32(object + 40);
    ++frame;
    velocity_x -= (uint32)((sint32)velocity_x >> (shift_x & 31));
    shift_z = r_u8(object + 62);
    velocity_y -= (uint32)((sint32)velocity_y >> (shift_y & 31));
    w_u32(object + 36, velocity_x);
    velocity_z = r_u32(object + 44);
    w_u8(object + 90, frame);
    velocity_z -= (uint32)((sint32)velocity_z >> (shift_z & 31));
    w_u32(object + 44, velocity_z);
    count = r_u8(object + 89);
    w_u32(object + 40, velocity_y);
    if ((sint8)frame >= (sint32)count)
        w_u8(object + 90, count - 1);
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    ++age;
    w_u16(object + 8, age);
    return (sint16)age < (sint32)lifetime ? 1 : Finish();
}

LoopingEndpointSprite::LoopingEndpointSprite() : EndpointSprite()
{
    FUNCTION_MARKER(0x8003658C, "SLUS_003.73");
    w_u32(NativeObjectGuestAddress(this) + 68, 0x800A1BE0);
}

uint32 LoopingEndpointSprite::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800365C0, "SLUS_003.73");
    w_u32(object + 68, 0x800A1BE0);
    uint32 result = EndpointSprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 LoopingEndpointSprite::Update()
{
    FUNCTION_MARKER(0x80036614, "SLUS_003.73");
    return AdvanceLoopingFrame();
}

extern "C" uint32 sub_80034D88(uint32 object)
{
    FUNCTION_MARKER(0x80034D88, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Sprite));
    new (dram_addr) Sprite();
    return object;
}

extern "C" uint32 sub_80034DE8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80034DE8, "SLUS_003.73");
    return Sprite::Attach(object).Sprite::Destroy(reason);
}

extern "C" uint32 sub_800353A4(uint32 object, uint32 position, uint32 name, uint32 size, uint32 initialize, uint32 first, uint32 last)
{
    FUNCTION_MARKER(0x800353A4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(AnimatedSprite));
    new (dram_addr) AnimatedSprite((const uint32 *)psx_addr(position, 12), (const char *)psx_addr(name, 1), size, initialize, first, last);
    return object;
}

extern "C" uint32 apocalypse_sprite_construct_native(uint32 object, const uint32 position[3], uint32 name, uint32 size, uint32 initialize, uint32 first, uint32 last)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(AnimatedSprite));
    new (dram_addr) AnimatedSprite(position, (const char *)psx_addr(name, 1), size, initialize, first, last);
    return object;
}

extern "C" uint32 sub_80035478(uint32 object, uint32 position, uint32 resource, uint32 size, uint32 initialize, uint32 first, uint32 last)
{
    FUNCTION_MARKER(0x80035478, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(AnimatedSprite));
    new (dram_addr) AnimatedSprite((const uint32 *)psx_addr(position, 12), resource, size, initialize, first, last);
    return object;
}

extern "C" uint32 apocalypse_effect_construct_35478(uint32 object, const void *position, uint32 resource, uint32 size, uint32 initialize, uint32 first, uint32 last)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(AnimatedSprite));
    new (dram_addr) AnimatedSprite((const uint32 *)position, resource, size, initialize, first, last);
    return object;
}

extern "C" uint32 sub_8003554C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003554C, "SLUS_003.73");
    return AnimatedSprite::Attach(object).AnimatedSprite::Destroy(reason);
}

extern "C" uint32 sub_800355A0(uint32 object)
{
    FUNCTION_MARKER(0x800355A0, "SLUS_003.73");
    return AnimatedSprite::Attach(object).AnimatedSprite::Update();
}

extern "C" uint32 sub_80035638(uint32 object, uint32 position, uint32 velocity, uint32 name, uint32 size, uint32 lifetime, uint32 mode)
{
    FUNCTION_MARKER(0x80035638, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(MovingSprite));
    new (dram_addr) MovingSprite((const uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(velocity, 12), (const char *)psx_addr(name, 1), size, lifetime, mode);
    return object;
}

extern "C" uint32 xport_draft_host_sub_80035638_p3(uint32 object, uint32 position, const void *velocity, uint32 name, uint32 size, uint32 lifetime, uint32 mode)
{
    FUNCTION_MARKER(0x80035638, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(MovingSprite));
    new (dram_addr) MovingSprite((const uint32 *)psx_addr(position, 12), (const uint32 *)velocity, (const char *)psx_addr(name, 1), size, lifetime, mode);
    return object;
}

extern "C" uint32 sub_80035704(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80035704, "SLUS_003.73");
    return MovingSprite::Attach(object).MovingSprite::Destroy(reason);
}

extern "C" uint32 sub_80035758(uint32 object)
{
    FUNCTION_MARKER(0x80035758, "SLUS_003.73");
    return MovingSprite::Attach(object).MovingSprite::Update();
}

extern "C" uint32 sub_800358B4(uint32 object, uint32 position, uint32 red, uint32 green, uint32 blue, uint32 resource, uint32 size, uint32 initialize, uint32 spread, uint32 acceleration, uint32 lifetime)
{
    FUNCTION_MARKER(0x800358B4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ParticleSprite));
    new (dram_addr) ParticleSprite((const uint32 *)psx_addr(position, 12), red, green, blue, resource, size, initialize, spread, acceleration, lifetime);
    return object;
}

extern "C" uint32 apocalypse_particle_construct_358B4(uint32 object, const void *position, uint32 red, uint32 green, uint32 blue, uint32 resource, uint32 size, uint32 initialize, uint32 spread, uint32 acceleration, uint32 lifetime)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ParticleSprite));
    new (dram_addr) ParticleSprite((const uint32 *)position, red, green, blue, resource, size, initialize, spread, acceleration, lifetime);
    return object;
}

extern "C" uint32 sub_80035A00(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80035A00, "SLUS_003.73");
    return ParticleSprite::Attach(object).ParticleSprite::Destroy(reason);
}

extern "C" uint32 sub_80035A54(uint32 object)
{
    FUNCTION_MARKER(0x80035A54, "SLUS_003.73");
    return ParticleSprite::Attach(object).ParticleSprite::Update();
}

extern "C" uint32 sub_80034E4C(uint32 object)
{
    FUNCTION_MARKER(0x80034E4C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(EndpointSprite));
    new (dram_addr) EndpointSprite();
    return object;
}

extern "C" uint32 sub_8003658C(uint32 object)
{
    FUNCTION_MARKER(0x8003658C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(LoopingEndpointSprite));
    new (dram_addr) LoopingEndpointSprite();
    return object;
}

extern "C" uint32 sub_80034E90(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80034E90, "SLUS_003.73");
    return EndpointSprite::Attach(object).EndpointSprite::Destroy(reason);
}

extern "C" uint32 sub_800365C0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800365C0, "SLUS_003.73");
    return LoopingEndpointSprite::Attach(object).LoopingEndpointSprite::Destroy(reason);
}

extern "C" uint32 sub_80036614(uint32 object)
{
    FUNCTION_MARKER(0x80036614, "SLUS_003.73");
    return LoopingEndpointSprite::Attach(object).LoopingEndpointSprite::Update();
}
