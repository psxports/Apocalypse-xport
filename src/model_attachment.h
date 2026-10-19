#ifndef MODEL_ATTACHMENT_H
#define MODEL_ATTACHMENT_H

#include "model_attachment_data.h"

class Character;

uint32 GetModelBonePosition(uint32 *output, Character &character, const ModelAttachmentPoint &point);
uint32 GetModelAttachment(uint32 *output, Character &character, uint32 requested);

#endif
