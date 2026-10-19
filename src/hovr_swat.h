#ifndef HOVR_SWAT_H
#define HOVR_SWAT_H

#include "vector3.h"
#include "enemy.h"

class Sprite;

class HovrSwat : public Enemy
{
public:
    HovrSwat(const RecordCursor &record_data, uint32 index);
    virtual ~HovrSwat();
    uint32 Init();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    uint32 Fragment();
    uint32 InitEffects();
    uint32 DestroyEffects();
    uint32 UpdateEffects();
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
    uint32 Fire();
    uint32 UpdateState();

private:
    Vector3 aim_pos;
    Vector3 aim_offset;
    ModelReference tracked_target;
    uint16 sound_timer;
    uint32 rotor_sound;
    uint32 sound_distance;
    ObjectReference<Sprite> smoke[8];
    uint8 debris_first;
    uint8 debris_count;
};

#endif
