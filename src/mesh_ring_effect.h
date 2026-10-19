#ifndef MESH_RING_EFFECT_H
#define MESH_RING_EFFECT_H

#include "effect_object.h"
#include "texture_resources.h"

// TODO Recover the original name for the 0x800A1398 ring mesh family
class RingAttack;

class MeshRingEffect : public RenderableEffect
{
public:
    MeshRingEffect(uint32 count, uint32 resource);
    virtual ~MeshRingEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual uint32 Render();
    uint32 SetTextureOffsets(uint32 u, uint32 v, uint32 repeats);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetGeometry(const uint32 *position, uint32 radius, uint32 expansion);

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
    uint32 seg_count;
    Segment *segs;
    Projection *proj;
    TextureResource *tex;
    MeshRingEffect(const MeshRingEffect &);
    MeshRingEffect &operator=(const MeshRingEffect &);
};

#endif
