#ifndef CREDITS_PLAYER_H
#define CREDITS_PLAYER_H

#include "player.h"

// The original table at 800A0340 derives its player slots from Player
class CreditsPlayer : public Player
{
public:
    virtual ~CreditsPlayer()
    {
    }

private:
    // Credits poses reuse Player::movement_state at original offset460

private:
    uint8 pose_ticks;

public:
    CreditsPlayer();

private:
    void SetPose(uint32 pose);
    void ChoosePose();

public:
    virtual uint32 Update();
    static uint32 Run();
    virtual uint32 Destroy(uint32 reason);
};

#endif
