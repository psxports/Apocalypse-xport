#ifndef CREDITS_PLAYER_H
#define CREDITS_PLAYER_H

#include "player.h"

// The original table at 800A0340 derives its player slots from Player
class CreditsPlayer : public Player
{
public:
    CreditsPlayer();
    virtual ~CreditsPlayer() {}
    static CreditsPlayer &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 SetPose(uint32 pose);
    uint32 ChoosePose();

private:
    // Native helper for the original inline adjusted weapon call
    void UpdateCreditsWeapon();
};

#endif
