#ifndef SPRITE_H
#define SPRITE_H

#include "vector3.h"
#include "effect_object.h"
#include "textured_effect.h"

struct HD_SPRITE_HISTORY;

class Sprite : public TexturedEffect
{
public:
    sint16 age;
    uint16 lifetime;
    uint16 angle;
    uint16 angle_speed;
    uint16 angle_accel;
    uint16 scale[2];
    uint8 forced_depth;
    uint8 blend_mode;
    uint16 clut;
    mutable HD_SPRITE_HISTORY *hd_history;

    virtual ~Sprite();

public:
    Sprite();
    virtual uint32 Destroy(uint32 reason);
};

#endif
