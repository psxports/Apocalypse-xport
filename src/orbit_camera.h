#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H

extern "C"
{
#include "psx.h"
}

class OrbitCamera
{
public:
    OrbitCamera(const uint32 *position, uint32 radius, uint32 height);
    virtual ~OrbitCamera() {}
    static OrbitCamera &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 Advance();
    uint32 Update();
};

#endif
