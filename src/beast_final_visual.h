#ifndef BEAST_FINAL_VISUAL_H
#define BEAST_FINAL_VISUAL_H

#include "visual_object.h"

// TODO Recover the resource name for hash4E5B6ED6
class BeastFinalVisual : public VisualObject
{
public:
    BeastFinalVisual(const uint32 *position);
    virtual ~BeastFinalVisual() {}
    static BeastFinalVisual &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
