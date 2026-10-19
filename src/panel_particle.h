#ifndef PANEL_PARTICLE_H
#define PANEL_PARTICLE_H

#include "effect_sprite.h"

// TODO Recover the original class name for the Panel resource particle
class PanelParticle : public Sprite
{
public:
    PanelParticle(const uint32 *position);

    virtual ~PanelParticle()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint8 intensity;
};

#endif
