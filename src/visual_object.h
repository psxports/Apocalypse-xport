#ifndef VISUAL_OBJECT_H
#define VISUAL_OBJECT_H
#include "scaled_object.h"

void FlagObjectNearReference(uint32 object);
// Native adapter retains the exact original target and adjusted receiver
uint32 DispatchObjectFinish(uint32 target, uint32 receiver);

class VisualObject : public ScaledObject
{
public:
    virtual ~VisualObject() {}
    static VisualObject &Attach(uint32 object);
    uint32 AddToList(uint32 *head);
    uint32 RemoveFromList(uint32 *head);
    uint32 RestoreList();
    uint32 MoveToTemporaryList(uint32 *head);
    const uint8 *ReadPosition(const uint8 *data);
    const uint8 *ReadAngles(const uint8 *data);
    uint32 SetModelBank(const char *name);
    uint32 SetColor(sint32 duration, uint32 red, uint32 green, uint32 blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();
    uint32 IsFinished();
    uint32 AdvanceColor();
    uint32 UpdateEventLatch(uint32 active, uint32 mask);
    uint32 EnableShadow();
    uint32 RemoveShadow();
    uint32 UpdateShadow();

protected:
    VisualObject();
};

void SmoothObjectPositions(uint32 object);
uint32 SaveObjectPositions(uint32 object);
uint32 RestoreObjectPositions(uint32 object);
void UpdateVisualObjects(uint32 *head);

#endif
