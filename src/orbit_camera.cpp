extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "orbit_camera.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include <new>

static uint32 ReadOrbitWord(const uint32 *record, uint32 offset)
{
    uint32 address = GuestMemoryAddress(record);
    return address ? r_u32(address + offset) : xport_load_le32((const uint8 *)record + offset);
}

OrbitCamera &OrbitCamera::Attach(uint32 object)
{
    return *static_cast<OrbitCamera *>(FindNativeObjectStorage(object));
}

OrbitCamera::OrbitCamera(const uint32 *position, uint32 radius, uint32 height)
{
    FUNCTION_MARKER(0x80011904, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    w_u32(object, 0x800A0380);
    w_u32(object + 4, ReadOrbitWord(position, 0) << 12);
    w_u32(object + 8, ReadOrbitWord(position, 4) << 12);
    uint32 z = ReadOrbitWord(position, 8);
    w_u32(object + 16, radius);
    w_u32(object + 20, height);
    w_u32(object + 12, z << 12);
    Update();
    w_u32(0x800ED51C, 0);
    w_u16(0x800FFA98, 128);
    w_u16(0x800FFA96, 128);
    w_u16(0x800FFA94, 128);
}

uint32 OrbitCamera::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80011990, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    w_u32(object, 0x800A0390);
    if (reason & 1)
        return ReleaseHeapBlock(object);
    return 0x800A0390;
}

uint32 OrbitCamera::Advance()
{
    uint32 *record = (uint32 *)psx_addr(NativeObjectGuestAddress(this), 28);
    FUNCTION_MARKER(0x800119C4, "SLUS_003.73");
    uint32 phase = ReadOrbitWord(record, 24) + 4;
    uint32 address = GuestMemoryAddress(record);
    if (address)
        w_u32(address + 24, phase);
    else
        xport_store_le32((uint8 *)record + 24, phase);
    return Update();
}

uint32 OrbitCamera::Update()
{
    const uint32 *record = (const uint32 *)psx_addr(NativeObjectGuestAddress(this), 28);
    FUNCTION_MARKER(0x800119F0, "SLUS_003.73");
    // Preserve the original initial position reads before phase lookup
    (void)ReadOrbitWord(record, 4);
    (void)ReadOrbitWord(record, 8);
    (void)ReadOrbitWord(record, 12);
    uint32 phase = ReadOrbitWord(record, 24) & 4095;
    uint32 sine = (uint32)(sint32)(sint16)r_u16(0x800F863C + phase * 4);
    uint32 radius = ReadOrbitWord(record, 16);
    uint32 x = ReadOrbitWord(record, 4) + radius * sine;
    uint32 height = ReadOrbitWord(record, 20) << 12;
    uint32 y = ReadOrbitWord(record, 8) - height;
    phase = ReadOrbitWord(record, 24) & 4095;
    uint32 cosine = (uint32)(sint32)(sint16)r_u16(0x800F863C + phase * 4 + 2);
    radius = ReadOrbitWord(record, 16);
    uint32 z = ReadOrbitWord(record, 12);
    w_u32(0x800ED520, (uint32)((sint32)x >> 12));
    w_u32(0x800ED524, (uint32)((sint32)y >> 12));
    z += radius * cosine;
    w_u32(0x800ED528, (uint32)((sint32)z >> 12));
    height = ReadOrbitWord(record, 20) << 12;
    radius = ReadOrbitWord(record, 16);
    sint32 slope;
    if (!radius)
        slope = (sint32)height < 0 ? 1 : -1;
    else if (height == 0x80000000 && radius == 0xFFFFFFFF)
        slope = (sint32)height;
    else
        slope = (sint32)height / (sint32)radius;
    w_u16(0x800ED548, 0 - (uint32)apocalypse_angle_from_slope(slope));
    uint32 address = GuestMemoryAddress(record);
    uint32 heading = address ? r_u16(address + 24) : xport_load_le16((const uint8 *)record + 24);
    w_u16(0x800ED54C, 0);
    uint32 result = heading + 2048;
    w_u16(0x800ED54A, result);
    return result;
}

extern "C" uint32 sub_80011904(uint32 object, uint32 position, uint32 radius, uint32 height)
{
    FUNCTION_MARKER(0x80011904, "SLUS_003.73");
    const uint32 *source = (const uint32 *)psx_addr(position, 12);
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OrbitCamera));
    new (dram_addr) OrbitCamera(source, radius, height);
    return object;
}

extern "C" uint32 sub_80011990(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80011990, "SLUS_003.73");
    return OrbitCamera::Attach(object).OrbitCamera::Destroy(reason);
}

extern "C" uint32 sub_800119F0(uint32 record)
{
    FUNCTION_MARKER(0x800119F0, "SLUS_003.73");
    return OrbitCamera::Attach(record).Update();
}

extern "C" uint32 sub_800119C4(uint32 record)
{
    FUNCTION_MARKER(0x800119C4, "SLUS_003.73");
    return OrbitCamera::Attach(record).Advance();
}
