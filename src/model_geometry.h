#ifndef MODEL_GEOMETRY_H
#define MODEL_GEOMETRY_H

#include "texture_resources.h"

struct Vector3;
struct ShortVector3;
struct CollisionVertex;
struct CollisionRay;
struct ModelInstance;
struct HD_BANK;
struct ModelPalette;

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
    TextureRegion texture_region;
    // Short reflection polygons borrow the following polygon color word
    const uint32 *window_alias;
    uint16 src_stride;
    ModelPolygon *next;
};

struct ModelGeometry
{
private:
    friend struct ModelInstance;

public:
    uint32 flags;
    uint32 radius;
    ModelBounds bounds[3];
    uint32 vertex_count;
    uint32 n_count;
    uint32 poly_count;
    // Vertex pad words retain original reference and interpolation tags
    const SVECTOR *vertices;
    const SVECTOR *normals;
    ModelPolygon *polys;

public:
    void RenderProjectionEffect(const MATRIX &matrix, const SVECTOR &vector, const Vector3 *pos, const ShortVector3 &dir_vector, uint32 color) const;
    uint32 RenderShadow(const MATRIX &matrix, uint32 rgb, uint32 scale, sint32 depth_bias, const TextureResource &tex) const;
    void RenderProj() const;
    void RenderOffsetScratch(const Vector3 *pos) const;
    void RenderTranslatedClipped() const;
    void RenderTranslated() const;
    void RenderScratch() const;
    void RenderOffset(const Vector3 *vector) const;
    void RenderUntranslatedClipped() const;

private:
    uint16 TransformColVertices(CollisionVertex *output, sint32 depth_limit, const Vector3 &translation) const;
    void IntersectColPolygons(const CollisionVertex *output, CollisionRay &rec, ModelInstance *object) const;
};

struct ModelGeometrySet
{
    uint32 count;
    ModelGeometry *models;
    HD_BANK *hd;

    HD_BANK *GetHdCache();
    void CaptureHdMaterials(const ModelPalette &palette);
};

bool DecodeModelGeometrySet(ModelGeometrySet &owner, const uint8 *data, uint32 bytes, const TextureReferences &refs);
void ReleaseModelGeometrySet(ModelGeometrySet &owner);
uint32 GetModelPolygonWindow(const ModelPolygon &poly);
uint32 PackModelPolygonUv(const ModelPolygon &poly, uint32 word);

#endif
