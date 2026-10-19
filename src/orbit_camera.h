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
    OrbitCamera(const uint32 *position, uint32 radius, uint32 height);

    virtual ~OrbitCamera()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    uint32 Advance();
    uint32 Update();

private:
    Vector3 center;
    uint32 radius;
    uint32 height;
    uint32 phase;
};

#endif
