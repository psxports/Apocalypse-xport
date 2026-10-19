#ifndef CREDITS_PLAYER_H
#define CREDITS_PLAYER_H

#include "player.h"

// The original table at 800A0340 derives its player slots from Player
class CreditsPlayer : public Player
{
public:
    CreditsPlayer();

    virtual ~CreditsPlayer()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    // Credits poses reuse Player::movement_state at original offset460
    void SetPose(uint32 pose);
    void ChoosePose();
    uint32 pose_ticks;
};

#endif
