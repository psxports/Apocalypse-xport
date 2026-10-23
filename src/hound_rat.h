#ifndef HOUND_RAT_H
#define HOUND_RAT_H

#include "vector3.h"
#include "enemy.h"

// Hound and rat resources share movement and behavior methods
class HoundRat : public Enemy
{
public:
    HoundRat(const RecordCursor &rec_data, uint16 idx, bool hound);

private:
    uint32 Init();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void UpdateState();
    uint32 SetTarget(const Vector3 *target);

public:
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void BuildOrbitPoint(Vector3 &point, uint16 angle) const;
};

#endif
