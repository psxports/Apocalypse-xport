#ifndef PANEL_PARTICLE_H
#define PANEL_PARTICLE_H

#include "effect_sprite.h"

// TODO Recover the original class name for the Panel resource particle
class PanelParticle : public Sprite
{
public:
    PanelParticle(const uint32 *position);
    virtual ~PanelParticle() {}
    static PanelParticle &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
