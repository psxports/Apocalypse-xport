#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

#include "object.h"

class OrbitCamera : public Object
{
public:
    virtual ~OrbitCamera()
    {
    }

private:
    Vector3 center;
    uint32 radius;
    uint32 height;
    uint32 phase;

public:
    OrbitCamera(const Vector3 *pos, uint32 radius, uint32 height);
    virtual uint32 Destroy(uint32 reason);
    uint32 Advance();

private:
    uint32 Update();
};

#endif
