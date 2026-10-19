#ifndef EXPANDING_RING_H
#define EXPANDING_RING_H
extern "C"
{
#include "psx.h"
}
#include "point_segments.h"

// Geometric XZ ring expands and fades without a texture resource
class ExpandingRing : public PointSegments
{
public:
    ExpandingRing(const sint32 *position, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 tail);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint8 rgb[3];
    uint32 max_radius;
    uint32 radius;
    uint32 growth;
    uint32 phase;
    static uint32 Divide(sint32 numerator, sint32 denominator);
    static uint32 ProjectRadius(uint32 radius, sint16 trigonometric_value);
};

#endif
