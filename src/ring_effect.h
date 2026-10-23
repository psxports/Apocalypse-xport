#ifndef RING_EFFECT_H
#define RING_EFFECT_H

#include "vector3.h"
#include "effect_object.h"

struct RingPoint
{
    uint32 distance;
    uint32 color;
};

class RingEffect : public EffectObject
{
public:
    RingPoint *ring_points;
    RingPoint *row_points;
    uint32 point_count;
    uint32 row_count;
    uint32 center_color;
    uint16 angle_step;
    bool hide_center;
    uint16 angle;
    uint32 seg_mask;

private:
    // Two native projected rays share the geometry allocation and its lifetime
    XY *proj_rows[2];

    RingEffect &operator=(const RingEffect &);

private:
    friend void BuildSceneEffectPackets();

public:
    static RingEffect *SpawnProjectileTrail(const Vector3 *pos, uint8 r, uint8 g, uint8 b);
    RingEffect(uint32 segs, uint32 rows);
    RingEffect(const Vector3 *src_pos, uint32 radius, uint32 extent, uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetOddRadii(uint32 val);
    uint32 SetEvenRadii(uint32 val);
    uint32 SetRadii(uint32 val);
    uint32 SetRowExtent(uint32 row, uint32 val);
    uint32 SetCenterColor(uint8 r, uint8 g, uint8 b);
    uint32 SetRingColors(uint8 r, uint8 g, uint8 b);
    uint32 SetRowColors(uint32 row, uint8 r, uint8 g, uint8 b);
    void SetColumnColors(uint32 column, uint8 r, uint8 g, uint8 b);
    uint32 FadeCenter(sint32 amount);
    uint32 FadeRing(sint32 amount);
    virtual void Update();

private:
    RingEffect(const RingEffect &);

public:
    virtual ~RingEffect();
};

#endif
