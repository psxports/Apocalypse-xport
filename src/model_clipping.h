#ifndef MODEL_CLIPPING_H
#define MODEL_CLIPPING_H

#include "model_geometry.h"

void ResetModelPolygonQueue();
void ReleaseModelPolygonQueue();
const ModelPolygon *ReadQueuedModelPolygons(uint32 &count);
void QueueWaterPlanePolygon(const ModelPolygon *desc, uint32 flags, const uint32 depths[4]);

#endif
