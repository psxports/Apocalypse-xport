#ifndef EXPANDING_RING_H
#define EXPANDING_RING_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}
#include "point_segments.h"

// Geometric XZ ring expands and fades without a texture resource
class ExpandingRing : public PointSegments
{
private:
    uint8 rgb[3];
    sint32 max_radius;
    sint32 radius;
    uint32 growth;
    uint16 phase;

public:
    ExpandingRing(const Vector3 *src_pos, sint32 max_radius, uint8 r, uint8 g, uint8 b, uint32 count, uint32 growth);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    static uint32 ProjectRadius(sint32 radius, sint16 trigonometric_val);
};

#endif
