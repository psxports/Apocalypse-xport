#ifndef MESH_RING_EFFECT_H
#define MESH_RING_EFFECT_H

#include "vector3.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "texture_resources.h"

// TODO Recover the original name for the 0x800A1398 ring mesh family
class RingAttack;

class MeshRingEffect : public RenderableEffect
{
public:
    ShortVector3 angles;
    uint32 radius;
    uint32 expansion;
    uint32 u_scroll;
    uint32 v_scroll;
    uint32 seg_mask;
    uint16 forced_depth;

private:
    friend class RingAttack;

    struct Segment
    {
        POLY_GT4 poly[2];
        Vector3 v[3];
    };
    struct Projection;
    sint32 seg_count;
    Segment *segs;
    Projection *proj;
    const TextureResource *tex;
    MeshRingEffect &operator=(const MeshRingEffect &);

public:
    MeshRingEffect(sint32 count, uint32 resource);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetGeometry(const Vector3 *src_pos, uint32 radius, uint32 expansion);
    uint32 SetTexOffsets(uint32 u, uint32 v, uint32 repeats);
    uint32 SetColors(sint32 r, sint32 g, sint32 b);
    virtual uint32 Render();

private:
    MeshRingEffect(const MeshRingEffect &);
    static sint32 ReadCameraOffset(sint32 pos, uint32 axis);

public:
    virtual ~MeshRingEffect();
    virtual void Update();
};

#endif
