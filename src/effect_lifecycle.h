#ifndef EFFECT_LIFECYCLE_H
#define EFFECT_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"

class EffectObject;
struct GameHeapBlock;

struct EffectLists
{
    EffectObject *controllers;
    EffectObject *sprs;
    EffectObject *endpoint_sprs;
    EffectObject *particles;
    EffectObject *quads;
    // Three- and four-vertex effects share this list
    EffectObject *polygons;
    EffectObject *textured_triangles;
    EffectObject *rings;
    EffectObject *opaque_fragments;
    EffectObject *renderables;
    EffectObject *points;
    EffectObject *segments;
    EffectObject *lines;
};

extern EffectLists effect_lists;
extern bool use_small_effect_pool;
extern uint32 effect_count;

#endif
