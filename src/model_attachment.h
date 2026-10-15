#ifndef MODEL_ATTACHMENT_H
#define MODEL_ATTACHMENT_H

extern "C"
{
#include "psx.h"
}

uint32 GetModelBonePosition(uint32 *output, uint32 object, const sint16 *descriptor);
uint32 GetModelAttachment(uint32 *output, uint32 object, uint32 requested);
uint32 GetCharacterAttachment(uint32 *output, uint32 object, uint32 requested);

#endif
