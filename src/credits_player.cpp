#include "audio_session.h"
#include "game_diagnostics.h"
#include "credits_player.h"
#include "game_object_allocation.h"
#include "game_random.h"
#include "model_attachment.h"
#include "weapon_nodes.h"

CreditsPlayer::CreditsPlayer() : Player(), pose_ticks(0)
{
    FUNCTION_MARKER(0x80014818, "SLUS_003.73");
    SetPose(0);
    uint32 flags = this->model.flags;
    this->model.color = 0x404040;
    this->model.flags = flags | 4;
}

void CreditsPlayer::SetPose(uint32 pose)
{
    FUNCTION_MARKER(0x80014870, "SLUS_003.73");
    movement_state = pose;
    pose_ticks = 120;
    switch (pose)
    {
        case 0:
        case 2:
        case 3:
            motion.angular_velocity.words[1] = 16;
            break;
        case 1:
        case 4:
            motion.angular_velocity.words[1] = 0;
            break;
        default:
            break;
    }
}

void CreditsPlayer::ChoosePose()
{
    FUNCTION_MARKER(0x800148C0, "SLUS_003.73");
    uint32 pose = movement_state;
    for (uint32 attempt = 0; attempt < 100; ++attempt)
    {
        pose = RandomRange(5);
        if (pose != movement_state)
            break;
    }
    SetPose(pose);
}

uint32 CreditsPlayer::Update()
{
    FUNCTION_MARKER(0x80014920, "SLUS_003.73");
    uint32 timer = pose_ticks;
    if (timer)
        pose_ticks = timer - 1;
    switch (movement_state)
    {
        case 0:
            if (this->model.animation != 1)
                StartAnimation(1, 1);
            if (!pose_ticks)
                ChoosePose();
            break;
        case 1:
            if (this->model.animation)
                StartAnimation(0, 1);
            if (!pose_ticks)
                ChoosePose();
            break;
        case 2:
        {
            uint32 animation = this->model.animation;
            if (animation != 9 && animation)
                SetAnimationRange(0, 0, (uint32)-1);
            if (this->playback.complete && !this->model.animation)
                SetAnimationRange(9, 0, (uint32)-1);
            if (!pose_ticks)
                ChoosePose();
            break;
        }
        case 3:
            if (this->model.animation != 5)
                SetAnimationRange(5, 0, (uint32)-1);
            if (!pose_ticks)
            {
                uint32 frame_count = this->playback.frame_count;
                sint32 frame = (sint8)this->model.frame;
                if (frame == (sint32)frame_count - 1)
                    SetAnimationRange(5, (uint32)-1, 0);
                if (!pose_ticks && !this->model.frame)
                    SetPose(0);
            }
            break;
        case 4:
        {
            uint32 animation = this->model.animation;
            if (animation != 5 && animation != 12)
                SetAnimationRange(5, 0, (uint32)-1);
            if (this->playback.complete)
            {
                if (this->model.animation == 5)
                {
                    uint32 frame_count = this->playback.frame_count;
                    sint32 frame = (sint8)this->model.frame;
                    if (frame == (sint32)frame_count - 1)
                    {
                        PlaySoundEffect(4, 0x2000, 0);
                        SetAnimationRange(12, 0, (uint32)-1);
                    }
                }
                if (this->playback.complete && this->model.animation == 12)
                    SetAnimationRange(5, (uint32)-1, 0);
            }
            if (this->playback.complete && (this->model.frame == 0 && this->model.animation == 5))
                SetPose(0);
            break;
        }
        default:
            break;
    }
    uint32 heading = this->model.angles.words[1];
    uint32 turn = motion.angular_velocity.words[1];
    this->model.angles.words[1] = heading + turn;
    GetModelAttachment(muzzle_pos.words, *this, 0);
    weapons[0]->Fire(muzzle_pos.words, model.angles.components);
    AdvanceAnimation();
    return UpdateAnimationModel(PlayerHierarchy);
}

uint32 CreditsPlayer::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80015148, "SLUS_003.73");
    uint32 result = Player::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}
