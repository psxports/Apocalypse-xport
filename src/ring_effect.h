#ifndef RING_EFFECT_H
#define RING_EFFECT_H

#include "effect_object.h"

struct RingPoint
{
    uint32 distance;
    uint32 color;
};

class RingEffect : public EffectObject
{
public:
    RingEffect(uint32 segments, uint32 rows);
    RingEffect(const uint32 *position, uint32 radius, uint32 extent, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    virtual ~RingEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint32 SetOddRadii(uint32 value);
    uint32 SetEvenRadii(uint32 value);
    uint32 SetRadii(uint32 value);
    uint32 SetRowExtent(uint32 row, uint32 value);
    uint32 SetCenterColor(uint32 red, uint32 green, uint32 blue);
    uint32 SetRingColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetRowColors(uint32 row, uint32 red, uint32 green, uint32 blue);
    void SetColumnColors(uint32 column, uint32 red, uint32 green, uint32 blue);
    uint32 FadeCenter(uint32 amount);
    uint32 FadeRing(uint32 amount);

    RingPoint *ring_points;
    RingPoint *row_points;
    uint32 point_count;
    uint32 row_count;
    uint32 center_color;
    uint16 angle_step;
    uint8 hide_center;
    uint16 angle;
    uint32 seg_mask;

private:
    friend void BuildSceneEffectPackets();
    // Two native projected rays share the geometry allocation and its lifetime
    XY *proj_rows[2];

    RingEffect(const RingEffect &);
    RingEffect &operator=(const RingEffect &);
};

RingEffect *SpawnProjectileRingTrail(const uint32 *position, uint32 red, uint32 green, uint32 blue);

#endif
