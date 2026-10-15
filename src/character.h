#ifndef CHARACTER_H
#define CHARACTER_H
#include "visual_object.h"

class Character : public VisualObject
{
public:
    virtual ~Character() {}
    virtual uint32 Destroy(uint32 reason);
    uint32 SetAnimationRange(uint32 animation, uint32 first, uint32 last);
    uint32 StartAnimation(uint32 animation, uint32 direction);
    uint32 AdvanceAnimation();
    uint32 UpdateAnimationModel(uint32 model);
    uint32 DidAnimationCrossFrame(uint32 animation, uint32 target_frame);

protected:
    Character();
};

#endif
