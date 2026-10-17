#ifndef SPIRAL_RAY_WEAPON_H
#define SPIRAL_RAY_WEAPON_H
extern "C" {
#include "psx.h"
}
#include "weapon_nodes.h"

// Descriptive native name for a hitscan ray with spiralling particle graphics
class SpiralRayWeapon : public WeaponNode
{
public:
    SpiralRayWeapon(ModelInstance **target);
    static SpiralRayWeapon &Attach(uint32 object);
    virtual ~SpiralRayWeapon() {}
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    uint32 Configure(uint32 first, uint32 second, uint32 enabled, uint32 speed, uint32 count, uint32 radius);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);

private:
    static uint32 DivideBy256(uint32 value);
    static void CopyRotation(uint32 guest, const void *native, sint16 output[3]);
    static uint32 ReadWord(uint32 guest, const uint32 *native, uint32 index);
    static uint32 ReadMasked(uint32 address, uint32 mask);
    // Native helper shares guest and native byte output views
    static void StoreLight(uint8 *output, uint32 value);
};

#endif
