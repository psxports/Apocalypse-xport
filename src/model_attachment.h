#ifndef MODEL_ATTACHMENT_H
#define MODEL_ATTACHMENT_H

#include "model_attachment_data.h"
#include "vector3.h"

class Character;

uint32 GetModelBonePos(Vector3 &output, Character &character, const ModelAttachmentPoint &point);
uint32 GetModelAttachment(Vector3 &output, Character &character, uint32 requested);

#endif
