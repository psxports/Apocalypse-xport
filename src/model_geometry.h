#ifndef MODEL_GEOMETRY_H
#define MODEL_GEOMETRY_H

#include "texture_resources.h"

struct ModelBounds
{
    sint16 max;
    sint16 min;
};

struct ModelPolygon
{
    uint16 flags;
    uint16 v[4];
    uint16 n;
    uint16 surface;
    uint32 color;
    // Packed UV, CLUT and TPage words are GPU values
    UV uv[4];
    uint16 clut;
    uint16 tpage;
    uint32 window;
    // Short reflection polygons borrow the following polygon color word
    const uint32 *window_alias;
    uint16 source_stride;
    ModelPolygon *next;
};

struct ModelGeometry
{
    uint32 flags;
    uint32 radius;
    ModelBounds bounds[3];
    uint32 vertex_count;
    uint32 n_count;
    uint32 poly_count;
    const SVECTOR *vertices;
    const SVECTOR *normals;
    ModelPolygon *polys;
};

struct ModelGeometrySet
{
    uint32 count;
    ModelGeometry *models;
};

bool DecodeModelGeometrySet(ModelGeometrySet &owner, const uint8 *data, uint32 bytes, const TextureReferences &refs);
void ReleaseModelGeometrySet(ModelGeometrySet &owner);
uint32 GetModelPolygonWindow(const ModelPolygon &poly);
uint32 PackModelPolygonUv(const ModelPolygon &poly, uint32 word);

#endif
