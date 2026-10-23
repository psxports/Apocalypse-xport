#ifndef MODEL_SUBDIVISION_H
#define MODEL_SUBDIVISION_H

#include "psx.h"

struct ModelProjectedVertex;

uint8 *SubdivColoredPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, bool quad, uint32 &last_stride);
uint8 *SubdivFlatTriangle(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, bool textured, uint32 &last_stride);
uint8 *SubdivColoredTexturedPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, bool quad, uint32 &last_stride);
uint8 *SubdivFlatTexturedQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 &last_stride);
uint8 *SubdivFlatQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 &last_stride);

#endif
