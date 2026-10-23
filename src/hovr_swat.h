#ifndef HOVR_SWAT_H
#define HOVR_SWAT_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"
#include "sprite.h"

class Sprite;

class HovrSwat : public Enemy
{
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

public:
    HovrSwat(const RecordCursor &rec_data, uint16 idx);

private:
    uint32 Init();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const Vector3 *input_vector, uint32 kind);

private:
    void Fragment();
    void InitSmoke();
    void DestroySmoke();
    void UpdateSmoke();

public:
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint16 command);

private:
    uint32 SetDst(const Vector3 *pos);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
    uint32 Fire();
    void UpdateState();

public:
    virtual ~HovrSwat();
};

#endif
