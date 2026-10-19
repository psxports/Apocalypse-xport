#ifndef MODEL_SUBDIVISION_H
#define MODEL_SUBDIVISION_H

#include "psx.h"

struct ModelProjectedVertex;

uint8 *SubdivideColoredPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 quad, uint32 *last_stride);
uint8 *SubdivideFlatTriangle(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 textured, uint32 *last_stride);
uint8 *SubdivideColoredTexturedPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 quad, uint32 *last_stride);
uint8 *SubdivideFlatTexturedQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 *last_stride);
uint8 *SubdivideFlatQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 *last_stride);

#endif
