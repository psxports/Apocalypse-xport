#ifndef SCRIPTED_EFFECT_CONTROLLER_H
#define SCRIPTED_EFFECT_CONTROLLER_H

#include "object_reference.h"
#include "enemy.h"
#include "object_force.h"
#include "segment_effect.h"
#include "waving_beam.h"

// Script-only owner of environment effects, object forces and timed sound
class ScriptedEffectController : public Enemy
{
private:
    ObjectReference<ObjectForce> force;

    struct RainDrop
    {
        Vector3 velocity;
        ObjectReference<SegmentEffect> effect;
    };

    uint16 rain_count;
    RainDrop *rain;
    ObjectReference<WavingBeam> beam;
    uint32 sound;
    sint32 sound_period;

public:
    ScriptedEffectController(const RecordCursor &rec_data, uint16 idx);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual void SetParameters(uint16 command);
    virtual uint32 ExecuteCommand(uint16 command);
    virtual ~ScriptedEffectController();

private:
    void ReleaseChildren();
};

#endif
