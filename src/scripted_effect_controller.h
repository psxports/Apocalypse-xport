#ifndef SCRIPTED_EFFECT_CONTROLLER_H
#define SCRIPTED_EFFECT_CONTROLLER_H

#include "enemy.h"
#include "object_force.h"
#include "effect_segments.h"
#include "waving_beam.h"

// Script-only owner of environment effects, object forces and timed sound
class ScriptedEffectController : public Enemy
{
public:
    ScriptedEffectController(const RecordCursor &record_data, uint32 index);
    virtual ~ScriptedEffectController();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    virtual void SetParameters(uint32 command);

    ObjectReference<ObjectForce> force;

private:
    struct RainDrop
    {
        Vector3 velocity;
        ObjectReference<SegmentEffect> effect;
    };

    void ReleaseChildren();
    uint16 rain_count;
    RainDrop *rain;
    ObjectReference<WavingBeam> beam;
    uint32 sound;
    uint32 sound_period;
};

#endif
