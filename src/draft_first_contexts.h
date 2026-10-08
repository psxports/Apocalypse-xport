#ifndef A_DRAFT_FIRST_CONTEXTS_H
#define A_DRAFT_FIRST_CONTEXTS_H
#include "psx.h"

typedef struct xport_draft_rotation_inputs
{
    uint32 addresses[3];
} xport_draft_rotation_inputs;

typedef struct xport_draft_reverse_polygon_inputs
{
    uint32 vertices;
    uint32 stride;
    uint32 flags;
    uint32 clip_bias;
    uint32 clip_mask;
    uint32 destination;
    uint32 subdivisions;
    uint32 step;
} xport_draft_reverse_polygon_inputs;

typedef struct xport_draft_polygon_strip_context
{
    uint32 packet_cursor;
    uint32 packet_limit;
    uint32 packet_words[6];
    uint32 texture_high0;
    uint32 texture_high1;
    uint32 clipping_mask;
    uint32 frustum_mask;
    uint32 remaining_segments;
} xport_draft_polygon_strip_context;
#endif
