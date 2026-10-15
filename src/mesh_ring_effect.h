#ifndef MESH_RING_EFFECT_H
#define MESH_RING_EFFECT_H

#include "effect_object.h"

// TODO Recover the original name for the 0x800A1398 ring mesh family
class MeshRingEffect : public RenderableEffect
{
public:
    MeshRingEffect(uint32 segments, uint32 resource);
    virtual ~MeshRingEffect() {}
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 Render();
    uint32 SetTextureOffsets(uint32 u, uint32 v, uint32 repeats);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetGeometry(const uint32 *position, uint32 radius, uint32 expansion);
    static MeshRingEffect &Attach(uint32 object);
};

#endif
