#ifndef PARTICLE_BEAM_H
#define PARTICLE_BEAM_H

#include "object_reference.h"
#include "vector3.h"
#include "object.h"
#include "model_instance.h"
#include "weapon_type.h"
#include "weapon_node.h"

class EffectObject;
class RingEffect;
class BeamSegments;

class ParticleBeam : public WeaponNode
{
private:
    friend class Plague;
    ObjectReference<BeamSegments> beam[2];
    uint32 impact_count;
    uint32 seg_len;
    bool trail_mode;

    struct Color
    {
        uint8 r, g, b;
    } colors[4];

    ObjectReference<RingEffect> ring;
    uint32 ring_scale;
    sint32 sound_id;

    // Native helper retains original inline damage dispatch

public:
    ParticleBeam(ModelInstance **list, sint32 range, bool mode, uint32 effect_scale, sint32 sound);
    virtual uint32 Destroy(uint32 reason);
    void SetColors(uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, uint8 r3, uint8 g3, uint8 b3);

private:
    void ClearChildren();

public:
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
    virtual ~ParticleBeam();

private:
    void DamageTarget(ModelInstance *hit, const Vector3 &dir);
};

#endif
