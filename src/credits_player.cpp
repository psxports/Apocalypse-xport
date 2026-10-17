#include "game_sdk_bindings.h"
#include "credits_player.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "game_random.h"
#include "model_attachment.h"
#include "machine_gun.h"
extern "C"
{
#include "draft_first_signatures.h"
}
#include <new>
#include <stdio.h>
#include <stdlib.h>

CreditsPlayer::CreditsPlayer() : Player()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80014818, "SLUS_003.73");
    w_u32(object + 68, 0x800A0340);
    SetPose(0);
    uint32 flags = this->model.flags;
    this->model.color = 0x404040;
    this->model.flags = flags | 4;
}

CreditsPlayer &CreditsPlayer::Attach(uint32 object)
{
    return *static_cast<CreditsPlayer *>(FindNativeObjectStorage(object));
}

uint32 CreditsPlayer::SetPose(uint32 pose)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80014870, "SLUS_003.73");
    w_u32(object + 460, pose);
    w_u32(object + 656, 120);
    switch (pose)
    {
        case 0:
        case 2:
        case 3:
            motion.angular_velocity[1] = 16;
            return 16;
        case 1:
        case 4:
            motion.angular_velocity[1] = 0;
            // The jump-table target remains the original result carrier
            return 0x800148B4;
        default:
            return 0x800A0000;
    }
}

uint32 CreditsPlayer::ChoosePose()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800148C0, "SLUS_003.73");
    uint32 pose = r_u32(object + 460);
    for (uint32 attempt = 0; attempt < 100; ++attempt)
    {
        pose = RandomRange(5);
        if (pose != r_u32(object + 460))
            break;
    }
    return SetPose(pose);
}

void CreditsPlayer::UpdateCreditsWeapon()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 weapon = r_u32(object + 612);
    uint32 table = r_u32(weapon);
    uint32 receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + 24);
    uint32 target = r_u32(table + 28);
    const sint32 *position = (const sint32 *)psx_addr(object + 416, 12);
    const sint16 *rotation = (const sint16 *)psx_addr(object + 16, 6);
    switch (target)
    {
        case 0x80022C90:
            MachineGun::Attach(receiver).MachineGun::Fire((const uint32 *)position, rotation);
            return;
        case 0x80023380:
            MachineGun::Attach(receiver).MachineGun::Update(position, rotation);
            return;
        default:
            // TODO Restore other original weapon targets if this object can select them
            GameDiagnostic("Missing credits weapon target %08X receiver=%08X\n", target, receiver);
            abort();
    }
}

uint32 CreditsPlayer::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80014920, "SLUS_003.73");
    uint32 timer = r_u32(object + 656);
    if (timer)
        w_u32(object + 656, timer - 1);
    switch (r_u32(object + 460))
    {
        case 0:
            if (this->model.animation != 1)
                StartAnimation(1, 1);
            if (!r_u32(object + 656))
                ChoosePose();
            break;
        case 1:
            if (this->model.animation)
                StartAnimation(0, 1);
            if (!r_u32(object + 656))
                ChoosePose();
            break;
        case 2:
        {
            uint32 animation = this->model.animation;
            if (animation != 9 && animation)
                SetAnimationRange(0, 0, (uint32)-1);
            if (r_u8(object + 303) && !this->model.animation)
                SetAnimationRange(9, 0, (uint32)-1);
            if (!r_u32(object + 656))
                ChoosePose();
            break;
        }
        case 3:
            if (this->model.animation != 5)
                SetAnimationRange(5, 0, (uint32)-1);
            if (!r_u32(object + 656))
            {
                uint32 frame_count = r_u8(object + 302);
                sint32 frame = (sint8)this->model.frame;
                if (frame == (sint32)frame_count - 1)
                    SetAnimationRange(5, (uint32)-1, 0);
                if (!r_u32(object + 656) && !this->model.frame)
                    SetPose(0);
            }
            break;
        case 4:
        {
            uint32 animation = this->model.animation;
            if (animation != 5 && animation != 12)
                SetAnimationRange(5, 0, (uint32)-1);
            if (r_u8(object + 303))
            {
                if (this->model.animation == 5)
                {
                    uint32 frame_count = r_u8(object + 302);
                    sint32 frame = (sint8)this->model.frame;
                    if (frame == (sint32)frame_count - 1)
                    {
                        sub_80069DF0(4, 0x2000, 0);
                        SetAnimationRange(12, 0, (uint32)-1);
                    }
                }
                if (r_u8(object + 303) && this->model.animation == 12)
                    SetAnimationRange(5, (uint32)-1, 0);
            }
            if (r_u8(object + 303) && (this->model.frame == 0 && this->model.animation == 5))
                SetPose(0);
            break;
        }
        default:
            break;
    }
    uint32 heading = this->model.angles[1];
    uint32 turn = motion.angular_velocity[1];
    this->model.angles[1] = heading + turn;
    GetCharacterAttachment((uint32 *)psx_addr(object + 416, 12), object, 0);
    UpdateCreditsWeapon();
    AdvanceAnimation();
    return UpdateAnimationModel(PlayerHierarchy);
}

uint32 CreditsPlayer::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80015148, "SLUS_003.73");
    w_u32(object + 68, 0x800A0340);
    uint32 result = Player::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}




extern "C" uint32 sub_80014920(uint32 object)
{
    FUNCTION_MARKER(0x80014920, "SLUS_003.73");
    return CreditsPlayer::Attach(object).CreditsPlayer::Update();
}
extern "C" uint32 sub_80015148(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80015148, "SLUS_003.73");
    return CreditsPlayer::Attach(object).CreditsPlayer::Destroy(reason);
}
