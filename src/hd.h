#ifndef HD_H
#define HD_H

#include "psx.h"
#include "vid.h"
#include "texture_region.h"

typedef struct HD_BANK HD_BANK;
typedef struct HD_MODEL_HISTORY HD_MODEL_HISTORY;
typedef struct HD_SPRITE_HISTORY HD_SPRITE_HISTORY;
typedef struct HD_LINE_HISTORY HD_LINE_HISTORY;
typedef struct HD_LINE_CHAIN HD_LINE_CHAIN;
typedef struct HD_TRAIL_HISTORY HD_TRAIL_HISTORY;
typedef struct HD_BEAM_HISTORY HD_BEAM_HISTORY;
typedef struct HD_RIBBON_HISTORY HD_RIBBON_HISTORY;
typedef struct HD_RING_HISTORY HD_RING_HISTORY;
typedef struct HD_POLYGON_CHAIN HD_POLYGON_CHAIN;

typedef struct HD_ATTACHMENT
{
    const HD_MODEL_HISTORY *owner;
    uint32 part;
    VECTORF point, reference;
} HD_ATTACHMENT;
typedef struct HD_MODEL_EFFECT_CHAIN HD_MODEL_EFFECT_CHAIN;

typedef struct HD_SOURCE_FACE
{
    uint16 flags;
    uint16 vertex[4];
    uint16 normal;
    uint16 uv[4];
    uint16 clut;
    uint16 tpage;
    uint32 color;
    uint32 window;
    TextureRegion texture_region;
} HD_SOURCE_FACE;

typedef struct HD_SOURCE_MODEL
{
    const SVECTOR *vertices;
    const SVECTOR *normals;
    const HD_SOURCE_FACE *faces;
    uint32 vertex_count;
    uint32 normal_count;
    uint32 face_count;
    uint32 flags;
} HD_SOURCE_MODEL;

typedef struct HD_POSE
{
    VECTORF pos;
    VECTORF scale;
    QUATF rotation;
} HD_POSE;

typedef struct HD_POSE_HISTORY
{
    HD_POSE previous;
    HD_POSE current;
    int initialized;
} HD_POSE_HISTORY;

typedef struct HD_MATRIX_HISTORY
{
    MATRIXF previous, current;
    QUATF previous_rotation, current_rotation;
    int initialized;
} HD_MATRIX_HISTORY;

typedef struct HD_CAMERA
{
    MATRIXF view;
    float focal_y;
    float near_depth;
    float far_depth;
    float center_x;
    float center_y;
    uint32 mode;
} HD_CAMERA;

typedef struct HD_CAMERA_HISTORY
{
    HD_POSE_HISTORY pose;
    HD_CAMERA previous, current;
} HD_CAMERA_HISTORY;

typedef struct HD_FRAME
{
    HD_CAMERA camera;
    HD_CAMERA_HISTORY camera_history, camera_logic_history;
    uint32 camera_logic_tick, camera_logic_epoch;
    int camera_logic_valid, camera_logic_seeded;
    uint32 camera_epoch, camera_serial;
    uint32 camera_revision, cached_revision, ui_revision;
    int cached, camera_captured;
    int selected;
    int open;
} HD_FRAME;

typedef struct HD_EFFECT_VERTEX
{
    VECTORF pos;
    float u, v;
    uint32 color;
} HD_EFFECT_VERTEX;

typedef struct HD_EFFECT_DEPTH
{
    VECTORF points[6];
    uint32 count, rule;
    sint32 offset;
    int clamp;
} HD_EFFECT_DEPTH;

typedef struct HD_EFFECT
{
    HD_EFFECT_VERTEX vertices[4];
    /* Billboard offsets use camera-plane units multiplied by focal_y */
    VECTORF offsets[4];
    uint32 space;
    uint32 sprite;
    uint32 line;
    uint32 strip;
    uint32 beam, beam_layer;
    uint32 ribbon, ribbon_layer;
    uint32 ring;
    uint32 polygon;
    uint32 model_effect;
    HD_EFFECT_DEPTH depth;
    uint32 count;
    uint32 window;
    uint16 page, clut, flags;
    TextureRegion texture_region;
    uint32 forced_depth;
    int cull_positive;
    int sort_endpoint;
} HD_EFFECT;

typedef struct HD_POLYGON_SAMPLE
{
    HD_EFFECT effect;
    uint32 bias, uv_period;
    int present, reject_zero_depth;
} HD_POLYGON_SAMPLE;

typedef struct HD_RING_POINT
{
    uint32 distance, color;
} HD_RING_POINT;

typedef struct HD_RING
{
    VECTORF center;
    HD_ATTACHMENT attachment;
    const HD_RING_POINT *points;
    uint32 count, rows, mask, color;
    float angle, step;
    uint16 page;
    int hide_center;
} HD_RING;

typedef struct HD_STRIP
{
    VECTORF center;
    VECTORF edges[2], saved[2];
    int connected;
} HD_STRIP;

typedef struct HD_BEAM_POINT
{
    VECTORF point;
    VECTORF history[2];
    float size;
    uint32 color;
} HD_BEAM_POINT;

typedef struct HD_RIBBON_POINT
{
    VECTORF point;
    uint32 center, edge;
    float width, height;
} HD_RIBBON_POINT;

typedef struct HD_RIBBON
{
    const HD_RIBBON_POINT *points;
    uint32 count;
    uint16 page, clut;
    TextureRegion texture_region;
    uint8 u, v;
    uint32 u_scroll, v_scroll, v_step, tick;
    int waving;
} HD_RIBBON;

typedef struct HD_LIGHTING
{
    MATRIXF directions;
    MATRIXF world_directions;
    int world_directions_valid;
    MATRIXF colors;
    /* PsyQ background/far arguments and signed IR constants retain their native units */
    VECTORF background;
    VECTORF far_color;
    VECTORF constant;
    uint32 draw_mask;
    float fog_start;
    sint32 fog_shift;
} HD_LIGHTING;

typedef struct HD_POSE_LINK
{
    uint32 part, parent;
} HD_POSE_LINK;

typedef struct HD_OBJECT_STATE
{
    HD_BANK *bank;
    MATRIXF transform;
    HD_LIGHTING lighting;
    uint32 model;
    uint32 part_count;
    uint32 hidden_parts;
    uint32 flags;
    uint32 near_bias;
    uint32 far_bias;
    float radius;
    float water_y;
    uint32 water_clip;
    uint32 reflection;
    uint16 reflection_page, reflection_clut, reflection_uv;
    TextureRegion reflection_region;
    /* Borrowed during collection; hd_scene_add copies the samples */
    const HD_MODEL_HISTORY *history;
    const HD_POSE_LINK *pose_links;
    uint32 pose_link_count;
} HD_OBJECT_STATE;

typedef struct HD_UI_IMAGE
{
    uint16 uv[4];
    uint16 page;
    uint16 clut;
    sint16 width, height;
    sint16 offset_x, offset_y;
    TextureRegion region;
    uint32 texel_flags;
} HD_UI_IMAGE;

typedef struct HD_SPRITE_SAMPLE
{
    VECTORF pos;
    HD_ATTACHMENT attachment;
    HD_UI_IMAGE image;
    uint32 color, flags, bias, forced;
    float size, scale_x, scale_y, angle, near_depth, far_depth;
    int flip, large, pixels;
} HD_SPRITE_SAMPLE;

typedef struct HD_LINE_SAMPLE
{
    VECTORF points[2];
    uint32 colors[2], flags, bias, forced;
    uint16 page;
    float near_depth, far_depth;
    int clip, sort_endpoint;
} HD_LINE_SAMPLE;

typedef struct HD_TRAIL_EDGE
{
    VECTORF previous[2], current[2];
    uint32 previous_segment, current_segment;
    /* Ranges address the source segment's material and UV interval */
    float previous_range[2], current_range[2];
} HD_TRAIL_EDGE;

typedef struct HD_TRAIL_SEGMENT
{
    VECTORF points[2];
    HD_UI_IMAGE image;
    uint32 color, flags, bias;
    float size, near_depth, far_depth;
    int present;
} HD_TRAIL_SEGMENT;

typedef struct HD_MODEL_EFFECT
{
    MATRIXF matrix;
    VECTORF minimum, maximum;
    VECTORF normal, plane, direction;
    HD_UI_IMAGE image;
    uint32 color;
    float scale;
    sint32 depth_bias;
    int projection;
} HD_MODEL_EFFECT;

typedef struct HD_UI_QUAD
{
    float x[4], y[4], u[4], v[4];
    uint32 color[4];
    uint16 page, clut;
    uint16 flags;
    uint16 depth;
    uint32 anchor;
} HD_UI_QUAD;

typedef struct HD_UI_TEXT_STYLE
{
    uint32 color, shadow;
    float scale;
    uint16 depth;
    uint8 alignment;
    uint8 flags;
    const sint32 *curve;
    int alternate;
} HD_UI_TEXT_STYLE;

typedef struct HD_UI_TEXT_HISTORY
{
    HD_UI_TEXT_STYLE style;
    sint32 curve[41];
    sint32 x, y;
    uint32 glow_color, glow_scale;
    uint32 revision, epoch;
    int initialized;
} HD_UI_TEXT_HISTORY;

typedef struct HD_UI_ARROW_HISTORY
{
    sint32 x, y;
    uint32 offset, anchor, revision, epoch;
    int initialized;
} HD_UI_ARROW_HISTORY;

enum
{
    HD_UI_CENTER,
    HD_UI_LEFT,
    HD_UI_RIGHT,
    HD_UI_STRETCH,
    HD_UI_EDGES
};

enum
{
    HD_TEXTURED = 1,
    HD_RAW_TEXTURE = 2,
    HD_SEMITRANSPARENT = 4,
    HD_FILTERED = 8,
    HD_OPAQUE_TEXELS_ONLY = 16,
    HD_STP_TEXELS_ONLY = 32
};

enum
{
    HD_EFFECT_CAMERA,
    HD_EFFECT_WORLD,
    HD_EFFECT_BILLBOARD
};

enum
{
    HD_DEPTH_CAPTURE,
    HD_DEPTH_MIN,
    HD_DEPTH_MAX,
    HD_DEPTH_MEAN
};

enum
{
    HD_TEXELS_ALL,
    HD_TEXELS_OPAQUE,
    HD_TEXELS_STP
};

/* Compact vertices carry page, CLUT and flags in tex[0..2] */
/* UV values use half texels; rect holds packed XY masks and offsets */
/* Cached source tags use n[3]; extra[0..1] retain face flags and vertex index */

#ifdef __cplusplus
extern "C"
{
#endif
    void hd_pose_from_matrix(HD_POSE *pose, const MATRIX *rotation, const VECTORF *pos, const VECTORF *scale);
    void hd_pose_seed(HD_POSE_HISTORY *history, const HD_POSE *pose);
    void hd_pose_capture(HD_POSE_HISTORY *history, const HD_POSE *pose);
    void hd_pose_matrix(const HD_POSE_HISTORY *history, float fract, MATRIXF *matrix);
    void hd_matrix_seed(HD_MATRIX_HISTORY *history, const MATRIXF *matrix);
    void hd_matrix_capture(HD_MATRIX_HISTORY *history, const MATRIXF *matrix);
    void hd_matrix_evaluate(const HD_MATRIX_HISTORY *history, float phase, MATRIXF *matrix);
    int hd_matrix_relative(const MATRIXF *parent, const MATRIXF *child, MATRIXF *local);
    void hd_projection(HD_CAMERA *camera, float distance, float width, float height, float center_x, float center_y, float near_depth, float far_depth);
    void hd_camera_view(HD_CAMERA *camera, const HD_POSE_HISTORY *history, float fract);
    void hd_camera_capture(HD_CAMERA_HISTORY *history, const HD_CAMERA *camera, int reset);
    void hd_camera_evaluate(const HD_CAMERA_HISTORY *history, float phase, HD_CAMERA *camera);
    void hd_camera_set_view(HD_CAMERA *camera, const QUATF *rotation, const VECTORF *position);
    int hd_sphere_visible(const HD_CAMERA *camera, float aspect, const VECTORF *center, float radius);
    void hd_ui_begin(void);
    void hd_ui_end(void);
    int hd_ui_collecting(void);
    void hd_ui_set_anchor(uint32 anchor);
    void hd_ui_set_scroll(float previous_offset);
    uint32 hd_ui_anchor(float x);
    void hd_ui_image(HD_UI_QUAD *quad, const HD_UI_IMAGE *image, uint32 color, uint32 flags, uint32 depth, uint32 anchor);
    void hd_ui_rect(HD_UI_QUAD *quad, float left, float top, float right, float bottom);
    void hd_ui_add(const HD_UI_QUAD *quad);
    void hd_ui_add_pair(const HD_UI_QUAD *current, const HD_UI_QUAD *previous);
    void hd_ui_arrows(HD_UI_ARROW_HISTORY *history, sint32 x, sint32 y, uint32 offset);
    int hd_ui_draw(void);
    uint32 hd_ui_text(sint32 x, sint32 y, const char *text, const HD_UI_TEXT_STYLE *style, int filter, uint32 glow_color, uint32 glow_scale, const HD_UI_IMAGE *glow, int (*glyph)(uint8 character, HD_UI_IMAGE *image), uint32 (*random)(uint32 limit));
    uint32 hd_ui_text_pair(sint32 x, sint32 y, const char *text, const HD_UI_TEXT_STYLE *style, const HD_UI_TEXT_STYLE *previous, int filter, uint32 glow_color, uint32 glow_scale, uint32 previous_glow_color, uint32 previous_glow_scale, const HD_UI_IMAGE *glow, int (*glyph)(uint8 character, HD_UI_IMAGE *image), uint32 (*random)(uint32 limit));
    uint32 hd_ui_text_history(HD_UI_TEXT_HISTORY *history, sint32 x, sint32 y, const char *text, const HD_UI_TEXT_STYLE *style, int filter, uint32 glow_color, uint32 glow_scale, const HD_UI_IMAGE *glow, int (*glyph)(uint8 character, HD_UI_IMAGE *image), uint32 (*random)(uint32 limit));
    int hd_init(void);
    void hd_reset(void);
    void hd_free(void);
    int hd_begin(const HD_CAMERA *camera, float r, float g, float b);
    void hd_pass(uint32 blend, uint32 texels, int depth_test, int depth_write, float alpha);
    int hd_submit(VID_GEOMETRY *mesh, VID_INDEX *indices, uint32 first, uint32 count, const MATRIXF *model);
    int hd_end(void);
    void hd_set_lighting(const HD_LIGHTING *lighting);
    void hd_scene_clear(void);
    void hd_scene_capture_begin(const HD_CAMERA *camera);
    void hd_scene_capture_end(void);
    int hd_scene_collecting(void);
    void hd_effect_point(VECTORF *point, float x, float y, float z);
    void hd_effect_offset(VECTORF *point, const VECTORF *center, float x, float y);
    int hd_effect_add(const HD_EFFECT *effect, float depth, uint32 bias, uint32 forced);
    int hd_effect_sort(HD_EFFECT *effect, const VECTORF *points, uint32 count, uint32 rule, sint32 offset, int clamp);
    int hd_sprite_tick(HD_SPRITE_HISTORY **owner, const HD_SPRITE_SAMPLE *sample);
    int hd_sprite_capture(HD_SPRITE_HISTORY **history, const HD_SPRITE_SAMPLE *sample);
    void hd_sprite_reset(HD_SPRITE_HISTORY *history);
    void hd_sprite_free(HD_SPRITE_HISTORY *history);
    int hd_line_tick(HD_LINE_HISTORY **owner, const HD_LINE_SAMPLE *sample);
    int hd_line_capture(HD_LINE_HISTORY **history, const HD_LINE_SAMPLE *sample);
    void hd_line_reset(HD_LINE_HISTORY *history);
    void hd_line_free(HD_LINE_HISTORY *history);
    HD_LINE_SAMPLE *hd_line_samples(uint32 count);
    int hd_line_chain_tick(HD_LINE_CHAIN **owner, const HD_LINE_SAMPLE *samples, uint32 count);
    int hd_line_chain_capture(HD_LINE_CHAIN **history, const HD_LINE_SAMPLE *samples, uint32 count);
    void hd_line_chain_reset(HD_LINE_CHAIN *history);
    void hd_line_chain_free(HD_LINE_CHAIN *history);
    /* Samples run oldest to newest; capacity is at least count * 2 - 1 */
    /* Step and advance count actual history insertions, not render frames */
    int hd_trail_correspond(const VECTORF *previous, const VECTORF *current, uint32 count, uint32 step, uint32 advance, HD_TRAIL_EDGE *edges, uint32 capacity, uint32 *used);
    HD_TRAIL_SEGMENT *hd_trail_segments(uint32 count);
    int hd_trail_captured(const HD_TRAIL_HISTORY *history);
    int hd_trail_capture(HD_TRAIL_HISTORY **history, const HD_TRAIL_SEGMENT *segments, uint32 count, uint32 step, uint64 insertions);
    int hd_trail_tick(HD_TRAIL_HISTORY **history, const HD_TRAIL_SEGMENT *segments, uint32 count, uint32 step, uint64 insertions);
    void hd_trail_reset(HD_TRAIL_HISTORY *history);
    void hd_trail_free(HD_TRAIL_HISTORY *history);
    HD_RING_POINT *hd_effect_ring_points(uint32 count);
    int hd_effect_ring(const HD_RING *ring);
    /* Capture a world-space center; hd_effect_ring consumes a camera-space center */
    int hd_ring_tick(HD_RING_HISTORY **owner, const HD_RING *ring);
    int hd_ring_capture(HD_RING_HISTORY **history, const HD_RING *ring);
    void hd_ring_reset(HD_RING_HISTORY *history);
    void hd_ring_free(HD_RING_HISTORY *history);
    /* Samples contain world vertices, original sort points and native byte UVs */
    HD_POLYGON_SAMPLE *hd_polygon_samples(uint32 count);
    int hd_polygon_chain_tick(HD_POLYGON_CHAIN **owner, const HD_POLYGON_SAMPLE *samples, uint32 count);
    int hd_polygon_chain_capture(HD_POLYGON_CHAIN **history, const HD_POLYGON_SAMPLE *samples, uint32 count);
    void hd_polygon_chain_reset(HD_POLYGON_CHAIN *history);
    void hd_polygon_chain_free(HD_POLYGON_CHAIN *history);
    int hd_effect_strip(HD_STRIP *strip, const HD_EFFECT *source, float size, float frame_width, uint32 flags, uint32 bias, float near_depth, float far_depth);
    int hd_effect_beam(HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page, float near_depth, float far_depth);
    /* Capture world positions; hd_effect_beam consumes camera positions */
    int hd_beam_capture(HD_BEAM_HISTORY **history, HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page, float near_depth, float far_depth);
    int hd_beam_tick(HD_BEAM_HISTORY **history, const HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page);
    void hd_beam_reset(HD_BEAM_HISTORY *history);
    void hd_beam_free(HD_BEAM_HISTORY *history);
    HD_RIBBON_POINT *hd_effect_ribbon_points(uint32 count);
    int hd_effect_ribbon(const HD_RIBBON *ribbon, float near_depth, float far_depth);
    /* Capture world positions; hd_effect_ribbon consumes camera positions */
    int hd_ribbon_capture(HD_RIBBON_HISTORY **history, const HD_RIBBON *ribbon, float near_depth, float far_depth);
    int hd_ribbon_tick(HD_RIBBON_HISTORY **history, const HD_RIBBON *ribbon, float near_depth, float far_depth);
    void hd_ribbon_reset(HD_RIBBON_HISTORY *history);
    void hd_ribbon_free(HD_RIBBON_HISTORY *history);
    int hd_effect_model(const HD_MODEL_EFFECT *source);
    int hd_model_effect_geometry(const HD_MODEL_EFFECT *source, const HD_CAMERA *camera, HD_POLYGON_SAMPLE *sample);
    int hd_model_effect_capture(HD_MODEL_EFFECT_CHAIN **history, uint32 slot, uint32 count, uint32 identity, const HD_MODEL_EFFECT *source);
    int hd_model_effect_tick(HD_MODEL_EFFECT_CHAIN **history, uint32 slot, uint32 count, uint32 identity, const HD_MODEL_EFFECT *source);
    void hd_model_effect_reset(HD_MODEL_EFFECT_CHAIN *history);
    void hd_model_effect_free(HD_MODEL_EFFECT_CHAIN *history);
    int hd_effect_line(const HD_EFFECT *line, float near_depth, float far_depth, int clip, uint32 bias, uint32 forced);
    int hd_effect_sprite(const VECTORF *world, const HD_UI_IMAGE *image, uint32 color, uint32 flags, float size, float scale_x, float scale_y, float angle, uint32 flip, uint32 bias, uint32 forced, float near_depth, float far_depth, int large);
    MATRIXF *hd_scene_pose_buffer(uint32 count);
    int hd_scene_add(const HD_OBJECT_STATE *state, const MATRIXF *poses, uint32 count);
    void hd_model_tick_begin(uint32 tick);
    int hd_model_history_tick(HD_MODEL_HISTORY **owner, const HD_OBJECT_STATE *state, const MATRIXF *poses, uint32 count);
    int hd_model_history_copy_poses(const HD_MODEL_HISTORY *history, MATRIXF *poses, uint32 count);
    int hd_model_history_point(const HD_MODEL_HISTORY *history, uint32 part, const VECTORF *local, VECTORF *world);
    int hd_model_history_capture(HD_MODEL_HISTORY **history, HD_OBJECT_STATE *state, const MATRIXF *poses, uint32 count);
    void hd_model_history_reset(HD_MODEL_HISTORY *history);
    void hd_model_history_free(HD_MODEL_HISTORY *history);
    int hd_scene_phase(float phase);
    int hd_scene_draw(void);
    int hd_scene_present(const HD_CAMERA *camera, float r, float g, float b);
    void hd_frame_camera_reset(HD_FRAME *frame);
    void hd_frame_camera_tick(HD_FRAME *frame, const HD_CAMERA *camera, uint32 tick);
    void hd_frame_begin(HD_FRAME *frame);
    int hd_frame_present(HD_FRAME *frame);
    int hd_frame_present_phase(HD_FRAME *frame, float phase);
    int hd_frame_can_redraw(const HD_FRAME *frame);
    int hd_frame_redraw(HD_FRAME *frame, float phase);
    int hd_image_draw(uint32 x, uint32 y, uint32 width, uint32 height, int rgb24);
    int hd_frame_image(HD_FRAME *frame, uint32 x, uint32 y, uint32 width, uint32 height, int rgb24);
    int hd_frame_active(void);
    int hd_frame_restoring_og(void);
    void hd_frame_interrupt(void);
    void hd_frame_abandon(HD_FRAME *frame);
    HD_BANK *hd_bank_create(uint32 models, uint32 faces, uint32 vertices, uint32 indices);
    int hd_bank_write_model(HD_BANK *bank, uint32 model, const HD_SOURCE_MODEL *source);
    int hd_bank_seal(HD_BANK *bank);
    void hd_bank_free(HD_BANK *bank);
    int hd_bank_submit(HD_BANK *bank, uint32 model, const MATRIXF *matrix);
    int hd_bank_select(HD_BANK *bank, const uint32 *faces, uint32 count);
    int hd_bank_submit_selected(HD_BANK *bank, const MATRIXF *matrix);
    int hd_bank_submit_part(HD_BANK *bank, uint32 part, const MATRIXF *poses, uint32 count);
    void hd_bank_set_palette(HD_BANK *bank, const uint32 *colors, uint32 count);
    void hd_bank_set_face(HD_BANK *bank, uint32 face, const uint16 uv[4], uint16 page, uint16 clut, uint32 window);
    void hd_bank_set_region(HD_BANK *bank, uint32 face, const TextureRegion *region);
    void hd_bank_set_flags(HD_BANK *bank, uint32 face, uint16 flags);
    void hd_bank_commit_materials(HD_BANK *bank);
    void hd_bank_pass(HD_BANK *bank, uint32 blend, uint32 texels, int depth_test, int depth_write, float alpha);
    void hd_bank_scene_pass(HD_BANK *bank, uint32 pass);
#ifdef __cplusplus
}
#endif

#endif
