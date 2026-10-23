#ifndef PANEL_PARTICLE_H
#define PANEL_PARTICLE_H

#include "vector3.h"
#include "sprite.h"

// Fading particle using the Panel resource
class PanelParticle : public Sprite
{
public:
    virtual ~PanelParticle()
    {
    }

private:
    uint8 intensity;

public:
    PanelParticle(const Vector3 *pos);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
