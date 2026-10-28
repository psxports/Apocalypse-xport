#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "hd.h"
#include "psx_gpu.h"
#include "vid_psx_sampling.h"

static VID_SHADER *hd_world_shader;
static VID_SHADER *hd_ui_shader;
static VID_SHADER *hd_image_shader;
static VID_MATERIAL *hd_image_material;
static VID_GEOMETRY *hd_image_mesh;
static uint32 hd_image_source_width, hd_image_source_height;
static sint32 hd_image_drawable_width, hd_image_drawable_height;
static HD_FRAME *hd_current_frame;
static int hd_last_selected = -1;
static int hd_restore_og;
static VID_MATERIAL *hd_ui_materials[5];
static VID_GEOMETRY *hd_ui_mesh;
static VID_INDEX *hd_ui_indices;
static uint32 hd_ui_mesh_capacity;
static uint32 hd_ui_revision, hd_ui_uploaded_revision;
static sint32 hd_ui_drawable_width, hd_ui_drawable_height;
static int hd_ui_mesh_valid;
static int hd_ui_collect;
static int hd_ui_submitted;
static uint32 hd_ui_current_anchor;
static float hd_ui_previous_scroll;

typedef struct HD_UI_ITEM
{
    HD_UI_QUAD quad;
    float previous_scroll;
    uint32 previous;
    uint32 next;
} HD_UI_ITEM;

static HD_UI_ITEM *hd_ui_items;
static HD_UI_QUAD *hd_ui_previous;
static uint32 hd_ui_previous_count, hd_ui_previous_capacity;
static HD_UI_QUAD *hd_ui_text_staging;
static uint32 hd_ui_text_staging_count, hd_ui_text_staging_capacity, hd_ui_text_staging_cursor;
static int hd_ui_text_endpoint;
static uint32 hd_ui_count, hd_ui_capacity;
static uint32 hd_ui_heads[4096];
static VID_TEXTURE *hd_vram;
static VID_TEXTURE *hd_default_palette;
static VID_TEXTURE *hd_default_faces;
static VID_MATERIAL *hd_materials[4][3][2][2];
static uint32 hd_vram_version;
static int hd_vram_valid;
static float hd_draw[32][4];
static int hd_active;

#if defined(_DEBUG)
static int hd_debug_probe_enabled;
static VID_SAMPLE *hd_debug_probes[9];
static uint32 hd_debug_colors[9];
static uint32 hd_debug_frames;
static uint32 hd_debug_region_attempts;
static int hd_debug_scene_pending;
static struct
{
    uint64 queue_us, redraw_phase_us;
    uint32 queue_frames, redraw_phase_frames;
} hd_debug_timing;
#endif

typedef struct HD_MODEL_RANGE
{
    uint32 first;
    uint32 count;
    uint32 first_face;
    uint32 face_count;
    uint32 first_batch;
    uint32 batch_count;
} HD_MODEL_RANGE;

typedef struct HD_POSE_BATCH
{
    uint32 first;
    uint32 count;
    uint32 parts[4];
    uint32 part_count;
} HD_POSE_BATCH;

typedef struct HD_SOURCE_REFERENCE
{
    SVECTOR pos;
    uint32 part;
    int valid;
} HD_SOURCE_REFERENCE;

typedef struct HD_FACE_RANGE
{
    uint32 first;
    uint32 batch;
    uint32 vertex;
    uint32 count;
} HD_FACE_RANGE;

typedef struct HD_OPAQUE_GROUP
{
    VID_TEXTURE *faces;
    VID_MATERIAL *material;
    VID_INDEX *indices;
    uint32 height, face_revision, index_count;
} HD_OPAQUE_GROUP;

struct HD_BANK
{
    uint32 identity;
    VID_GEOMETRY *mesh;
    VID_INDEX *topology;
    VID_INDEX *selection;
    VID_TEXTURE *palette;
    VID_TEXTURE *face_materials;
    VID_MATERIAL *materials[4][3][2][2];
    int palette_changed;
    int faces_changed;
    HD_MODEL_RANGE *models;
    HD_FACE_RANGE *faces;
    HD_POSE_BATCH *batches;
    HD_SOURCE_REFERENCE *references;
    uint32 batch_count;
    uint32 reference_count;
    uint32 reference_capacity;
    uint32 model_capacity;
    uint32 face_capacity;
    uint32 vertex_capacity;
    uint32 index_capacity;
    uint32 model_count;
    uint32 face_count;
    uint32 vertex_count;
    uint32 index_count;
    uint32 selection_count;
    int sealed;
    HD_OPAQUE_GROUP *opaque_groups;
    uint32 opaque_group_count, opaque_group_capacity, opaque_group_cursor;
    uint32 *model_marks, model_stamp, face_revision;
};

typedef struct HD_SCENE_OBJECT
{
    HD_OBJECT_STATE state;
    const HD_MODEL_HISTORY *owner_history;
    HD_MATRIX_HISTORY transform;
    int interpolate, visible;
    float radius;
    uint32 pose_offset;
    uint32 pose_count;
    uint32 order;
    uint32 hierarchy_offset, hierarchy_count, hierarchy_source_count;
} HD_SCENE_OBJECT;

typedef struct HD_POSE_SAMPLE
{
    HD_MATRIX_HISTORY local;
    uint32 part, parent;
} HD_POSE_SAMPLE;

static HD_POSE_SAMPLE *hd_skeletal_samples;
static uint32 hd_skeletal_count, hd_skeletal_capacity;

typedef struct HD_SCENE_FACE
{
    uint32 object;
    uint32 face;
    uint32 bucket;
    uint32 order;
    uint32 pass;
} HD_SCENE_FACE;

static HD_SCENE_OBJECT *hd_objects;
static MATRIXF *hd_poses;
static HD_MATRIX_HISTORY *hd_pose_history;
static uint32 hd_pose_history_capacity;
static uint32 hd_bank_identity;
static uint32 hd_history_epoch = 1, hd_capture_serial;
static uint32 hd_scene_revision;
static uint32 hd_logic_tick;
static int hd_logic_tick_valid;
static MATRIXF *hd_pose_scratch;
static uint32 hd_pose_scratch_capacity;
static int hd_collecting;
static HD_CAMERA hd_capture_camera;
static HD_CAMERA hd_draw_camera;
static float hd_draw_aspect;
static MATRIXF hd_capture_inverse;
static float hd_capture_aspect;
static HD_SCENE_FACE *hd_alpha;
static uint32 hd_object_count, hd_object_capacity;
static uint32 hd_pose_count, hd_pose_capacity;
static uint32 hd_alpha_count, hd_alpha_capacity;
static VID_INDEX *hd_scene_indices;
static uint32 hd_scene_index_capacity, hd_scene_index_count;
static int hd_scene_submitted;

struct HD_MODEL_HISTORY
{
    HD_MATRIX_HISTORY *matrices;
    uint32 capacity, count, serial, epoch, bank, model;
    uint32 scene_revision, scene_offset;
    uint32 logic_tick;
    int logic_valid;
    float previous_radius, current_radius;
    int initialized;
};

typedef struct HD_EFFECT_ITEM
{
    HD_EFFECT effect;
    uint32 bias;
    uint32 bucket;
    uint32 order;
} HD_EFFECT_ITEM;

static HD_EFFECT_ITEM *hd_effects;
static uint32 hd_effect_count, hd_effect_capacity, hd_effect_mesh_capacity;
static uint32 hd_effect_version, hd_effect_upload_version;
static int hd_effect_upload_valid;
static float hd_render_phase = 1;
static VID_TEXTURE *hd_sprite_texture;
static uint32 hd_sprite_texture_height;

typedef struct HD_SAMPLE_TICK
{
    uint32 tick, published_tick;
    int valid, published;
} HD_SAMPLE_TICK;

enum
{
    HD_SAMPLE_DUPLICATE,
    HD_SAMPLE_SEED,
    HD_SAMPLE_ADVANCE,
    HD_SAMPLE_REUSE
};

static int hd_sample_step(HD_SAMPLE_TICK *clock, uint32 serial, uint32 epoch, int initialized, int changed, int logic)
{
    int seed;
    if (logic)
    {
        if (!hd_logic_tick_valid)
            return -1;
        if (!changed && initialized && epoch == hd_history_epoch && clock->valid && clock->tick == hd_logic_tick)
            return HD_SAMPLE_DUPLICATE;
        seed = changed || !initialized || epoch != hd_history_epoch || !clock->valid || clock->tick + 1 != hd_logic_tick;
        clock->tick = hd_logic_tick;
        clock->valid = 1;
        clock->published = 0;
        return seed ? HD_SAMPLE_SEED : HD_SAMPLE_ADVANCE;
    }
    clock->published_tick = hd_logic_tick;
    clock->published = 1;
    if (!changed && initialized && epoch == hd_history_epoch && clock->valid && hd_logic_tick_valid && clock->tick == hd_logic_tick)
        return HD_SAMPLE_REUSE;
    clock->tick = hd_logic_tick;
    clock->valid = hd_logic_tick_valid;
    if (changed || !initialized || epoch != hd_history_epoch || (serial != hd_capture_serial && serial + 1 != hd_capture_serial))
        return HD_SAMPLE_SEED;
    return serial == hd_capture_serial ? HD_SAMPLE_DUPLICATE : HD_SAMPLE_ADVANCE;
}

typedef struct HD_SPRITE_PAIR
{
    HD_SPRITE_SAMPLE previous, current;
} HD_SPRITE_PAIR;

struct HD_SPRITE_HISTORY
{
    HD_SPRITE_PAIR samples;
    HD_SAMPLE_TICK clock;
    uint32 serial, epoch;
    int initialized;
};

static HD_SPRITE_PAIR *hd_sprites;
static uint32 hd_sprite_count, hd_sprite_capacity;
typedef struct HD_LINE_PAIR
{
    HD_LINE_SAMPLE previous, current;
} HD_LINE_PAIR;

struct HD_LINE_HISTORY
{
    HD_LINE_PAIR samples;
    HD_SAMPLE_TICK clock;
    uint32 serial, epoch;
    int initialized;
};

struct HD_LINE_CHAIN
{
    HD_LINE_HISTORY *lines;
    uint32 count;
};

static HD_LINE_SAMPLE *hd_line_staging;
static uint32 hd_line_staging_capacity;

struct HD_TRAIL_HISTORY
{
    HD_TRAIL_SEGMENT *previous, *current;
    HD_SAMPLE_TICK clock;
    VECTORF *positions;
    HD_TRAIL_EDGE *edges;
    uint32 count, step, edge_count, serial, epoch;
    uint64 insertions;
    int initialized;
};

static HD_TRAIL_SEGMENT *hd_trail_staging;
static uint32 hd_trail_staging_capacity;

typedef struct HD_STRIP_DRAW
{
    HD_TRAIL_EDGE edge;
    HD_TRAIL_SEGMENT previous, current;
    uint32 begin;
} HD_STRIP_DRAW;

static HD_STRIP_DRAW *hd_strips;
static uint32 hd_strip_count, hd_strip_capacity;

struct HD_BEAM_HISTORY
{
    HD_SAMPLE_TICK clock;
    uint32 previous_mode, previous_bias;
    uint16 previous_page;
    int previous_valid;
    HD_BEAM_POINT *previous, *current;
    uint32 count, serial, epoch, mode, bias;
    uint16 page;
    int initialized, valid;
};

typedef struct HD_BEAM_SAMPLE
{
    HD_BEAM_POINT previous, current;
    uint32 begin, count, previous_mode, current_mode, previous_bias, current_bias;
    uint16 previous_page, current_page;
    float near_depth, far_depth;
    int previous_valid, current_valid;
} HD_BEAM_SAMPLE;

static HD_BEAM_SAMPLE *hd_beams;
static uint32 hd_beam_count, hd_beam_sample_capacity;

typedef struct HD_RIBBON_SAMPLE
{
    HD_RIBBON_POINT previous, current;
    HD_RIBBON previous_config, current_config;
    uint32 begin, count;
    float near_depth, far_depth;
} HD_RIBBON_SAMPLE;

static HD_RIBBON_SAMPLE *hd_ribbons;
static uint32 hd_ribbon_count, hd_ribbon_sample_capacity;

struct HD_RING_HISTORY
{
    HD_RING previous, current;
    HD_SAMPLE_TICK clock;
    HD_RING_POINT *previous_points, *current_points;
    uint32 count, rows, serial, epoch;
    int initialized;
};

typedef struct HD_RING_PRIMITIVE
{
    VECTORF center;
    HD_ATTACHMENT attachment;
    float angles[2], radii[4];
    uint32 colors[4], flags;
    uint16 page;
    int visible;
} HD_RING_PRIMITIVE;

typedef struct HD_RING_PAIR
{
    HD_RING_PRIMITIVE previous, current;
    uint32 count;
} HD_RING_PAIR;

static HD_RING_PAIR *hd_rings;
static uint32 hd_ring_count, hd_ring_sample_capacity;

struct HD_POLYGON_CHAIN
{
    HD_POLYGON_SAMPLE *previous, *current;
    HD_SAMPLE_TICK clock;
    uint32 count, serial, epoch;
    int initialized;
};

typedef struct HD_POLYGON_PAIR
{
    HD_POLYGON_SAMPLE previous, current;
} HD_POLYGON_PAIR;

static HD_POLYGON_PAIR *hd_polygons;
static uint32 hd_polygon_count, hd_polygon_capacity;
static HD_POLYGON_SAMPLE *hd_polygon_staging;
static uint32 hd_polygon_staging_capacity;

typedef struct HD_MODEL_EFFECT_ENTRY
{
    HD_MODEL_EFFECT previous, current;
    HD_SAMPLE_TICK clock;
    HD_MATRIX_HISTORY matrix;
    uint32 serial, epoch;
    int initialized;
} HD_MODEL_EFFECT_ENTRY;

static HD_MODEL_EFFECT_ENTRY *hd_model_effects;
static uint32 hd_model_effect_count, hd_model_effect_capacity;

static HD_LINE_PAIR *hd_lines;
static uint32 hd_line_count, hd_line_capacity, hd_line_texture_height;
static VID_TEXTURE *hd_line_texture;
static uint32 hd_capture_order;
static VID_SHADER *hd_effect_shader;
static VID_MATERIAL *hd_effect_materials[5][2];
static VID_TEXTURE *hd_effect_sprite_bindings[5][2];
static VID_TEXTURE *hd_effect_line_bindings[5][2];
static VID_GEOMETRY *hd_effect_mesh;
static HD_RING_POINT *hd_ring_points;
static uint32 hd_ring_capacity;

typedef struct HD_BEAM_DRAW
{
    VECTORF screen;
    VECTORF edges[2];
    float nx, ny;
    int valid;
} HD_BEAM_DRAW;

static HD_BEAM_DRAW *hd_beam_draw;
static uint32 hd_beam_capacity;
static HD_RIBBON_POINT *hd_ribbon_points;
static uint32 hd_ribbon_capacity;

/* The original 512-pixel GTE plane scales its horizontal camera axis by 8/5 */
static const float hd_effect_pixel_aspect = 320.0f / 512;

static const char hd_vertex_source[] =
    "layout(location=0) in vec4 aPos;\n"
    "layout(location=1) in vec2 aUV;\n"
    "layout(location=2) in vec4 aColor;\n"
    "layout(location=3) in vec4 aNormal;\n"
    "layout(location=4) in vec4 aExtra;\n"
    "layout(location=5) in uvec4 aTex;\n"
    "layout(location=6) in uvec2 aRect;\n"
    "uniform vec4 uDraw[32];vec4 drawData[32]; uniform sampler2D uAux0; uniform highp usampler2D uAux1;\n"
    "out vec2 vUV; out vec4 vColor; out float vDepth; out vec2 vFog;out float vWater;\n"
    "flat out uvec4 vRegion;flat out uint vRepeat;flat out uvec4 vTex; flat out uvec2 vRect; flat out vec2 vMaskAlpha;\n"
    "flat out uvec2 vPrimitive;flat out float vPass;\n"
    "vec3 worldPoint(vec3 local,int slot){vec4 v=vec4(local,1.0);int base=0;\n"
    "if(drawData[18].z>0.0&&slot>0)base=19+(slot-1)*3;\n"
    "return vec3(dot(drawData[base],v),dot(drawData[base+1],v),dot(drawData[base+2],v));}\n"
    "vec3 cameraPoint(vec3 local,int slot){vec4 w=vec4(worldPoint(local,slot),1.0);\n"
    "return vec3(dot(drawData[3],w),dot(drawData[4],w),dot(drawData[5],w));}\n"
    "uvec4 faceCell(int face,int cell){int at=face*8+cell;return texelFetch(uAux1,ivec2(at%1024,at/1024),0);}\n"
    "vec3 signedXYZ(uvec4 value){return vec3(ivec3(value.xyz^uvec3(32768u))-ivec3(32768));}\n"
    "float faceDepth(int face,uint flags){vec3 world[4];float z[4];float side[4];float depth=-1.0e30;\n"
    "bool water=(uint(drawData[28].x)&1u)!=0u;int count=(flags&16u)!=0u?3:4;\n"
    "for(int i=0;i<4;i++){uvec4 p=faceCell(face,i+2);world[i]=worldPoint(signedXYZ(p),int(p.w));\n"
    "z[i]=dot(drawData[5],vec4(world[i],1.0));side[i]=drawData[28].y-world[i].y;if(i<count&&(!water||side[i]>=0.0))depth=max(depth,z[i]);}\n"
    "if(water)for(int i=0;i<4;i++){if(i<count){int a=count==4?(i<2?i:5-i):i;int next=(i+1)%count;int b=count==4?(next<2?next:5-next):next;\n"
    "if((side[a]<0.0)!=(side[b]<0.0)){float phase=side[a]/(side[a]-side[b]);depth=max(depth,mix(z[a],z[b],phase));}}}\n"
    "return depth;}\n"
    "vec3 normalIR(vec3 normal){\n"
    "vec3 light=clamp(floor(vec3(dot(drawData[9].xyz,normal),dot(drawData[10].xyz,normal),dot(drawData[11].xyz,normal))),0.0,32767.0);\n"
    "vec3 color=clamp(floor(vec3(dot(drawData[12].xyz,light),dot(drawData[13].xyz,light),dot(drawData[14].xyz,light))+drawData[15].xyz*16.0),0.0,32767.0);\n"
    "return clamp(floor(color*255.0/256.0),0.0,32767.0);}\n"
    "vec3 wrap16(vec3 v){return mod(v+32768.0,65536.0)-32768.0;}\n"
    "vec2 reflectionUV(vec3 local,vec3 normal){\n"
    "vec3 eye=vec3(drawData[29].w,drawData[30].w,drawData[31].w);vec3 d=wrap16(eye-local);\n"
    "vec3 n=normal/4096.0;float factor=mod(clamp(dot(d,n),-32768.0,32767.0)*2.0+32768.0,65536.0)-32768.0;\n"
    "vec3 r=wrap16(clamp(n*factor,-32768.0,32767.0)-d);\n"
    "vec3 p=vec3(dot(drawData[29].xyz,r),dot(drawData[30].xyz,r),dot(drawData[31].xyz,r));\n"
    "float quotient=min(1024.0/max(clamp(p.z,0.0,65535.0),0.0001),2.0);\n"
    "vec2 screen=clamp(clamp(p.xy,-32768.0,32767.0)*quotient,vec2(-1024.0),vec2(1023.0));\n"
    "uint base=uint(drawData[28].w);return vec2(base&255u,base>>8)+mod(floor(screen/32.0)+32.0,64.0);}\n"
    "vec4 sceneCell(int at){uvec4 lo=texelFetch(uAux1,ivec2(at%1024,at/1024),0);at++;\n"
    "uvec4 hi=texelFetch(uAux1,ivec2(at%1024,at/1024),0);return uintBitsToFloat(lo|(hi<<16));}\n"
    "void main(){for(int i=0;i<32;i++)drawData[i]=uDraw[i];\n"
    "if(uDraw[18].z==2.0){int at=int(uDraw[31].w)*1024+int(aExtra.x)*64;\n"
    "for(int i=0;i<32;i++)drawData[i]=sceneCell(at+i*2);}\n"
    "vec3 p=cameraPoint(aPos.xyz,int(aPos.w));\n"
    "vWater=(uint(drawData[28].x)&1u)!=0u?drawData[28].y-worldPoint(aPos.xyz,int(aPos.w)).y:1.0;\n"
    "gl_Position=vec4(p.x*drawData[6].x,-p.y*drawData[6].y,drawData[6].z*p.z+drawData[6].w,p.z);\n"
    "gl_Position.xy+=drawData[7].zw*p.z;vMaskAlpha=drawData[7].xy;\n"
    "vRegion=uvec4(0u);vRepeat=0u;vUV=aUV*0.5;vColor=aColor/255.0;vDepth=p.z;vFog=vec2(0.0);vTex=aTex;vRect=aRect;\n"
    "vPrimitive=uvec2(0u);vPass=drawData[18].w;\n"
    "if(drawData[8].x>0.0){\n"
    "int face=int(aExtra.z);uvec4 uv=faceCell(face,0);\n"
    "uint coord=uv[int(aExtra.w)];vUV=vec2(coord&255u,coord>>8);\n"
    "uvec4 material=faceCell(face,1);\n"
    "vRegion=faceCell(face,7);vRepeat=vRegion.z>>9;vRegion.z&=511u;\n"
    "vTex.xy=material.xy;uint window=material.z|(material.w<<16);\n"
    "vRect=uvec2(((window&31u)<<3)|((window&992u)<<6),((window>>7)&248u)|((window>>4)&63488u));\n"
    "uint flags=faceCell(face,6).w;if(drawData[8].y>0.0)flags=(flags&uint(drawData[8].z))|uint(drawData[8].w);\n"
    "if(vPass>0.0){\n"
    "bool colored=(flags&2048u)!=0u||(flags&12u)==12u;bool textured=(flags&(colored?1u:3u))!=0u;\n"
    "vTex.z=(vTex.z&(~1u))|(textured?1u:0u);vTex.x|=(flags&384u)>>2;\n"
    "if(textured&&(flags&1u)==0u&&(uint(drawData[28].x)&2u)!=0u){uint texture=uint(drawData[28].z);\n"
    "vRegion=uvec4(drawData[9].w,drawData[10].w,drawData[11].w,drawData[12].w);vUV=reflectionUV(aPos.xyz,aNormal.xyz);vTex.xy=uvec2((texture&511u)|((flags&384u)>>2),texture>>9);}\n"
    "vPrimitive=uvec2(flags,textured?((vTex.x>>5)&3u):((flags>>7)&3u));\n"
    "vec3 p0=cameraPoint(signedXYZ(faceCell(face,2)),int(faceCell(face,2).w));\n"
    "vec3 p1=cameraPoint(signedXYZ(faceCell(face,3)),int(faceCell(face,3).w));\n"
    "vec3 p2=cameraPoint(signedXYZ(faceCell(face,4)),int(faceCell(face,4).w));\n"
    "vec3 p3=cameraPoint(signedXYZ(faceCell(face,5)),int(faceCell(face,5).w));\n"
    "bool front=determinant(mat3(p0,p1,p2))>0.0;\n"
    "if((flags&16u)==0u)front=front||determinant(mat3(p1,p3,p2))>0.0;\n"
    "if((flags&192u)==0u||(!front&&min(min(p0.z,p1.z),min(p2.z,p3.z))>0.0))gl_Position=vec4(2.0,2.0,2.0,1.0);}\n"
    "if((flags&2048u)!=0u)vColor=texelFetch(uAux0,ivec2(int(aTex.w),0),0);\n"
    "if(drawData[8].y>0.0){\n"
    "float depth=faceDepth(face,flags);\n"
    "vec3 ir=vec3(4096.0);uint mode=flags&12u;\n"
    "if(mode==4u)ir=normalIR(signedXYZ(faceCell(face,6)));\n"
    "else if(mode==12u)ir=normalIR(aNormal.xyz);else if(mode==8u)ir=drawData[16].xyz;\n"
    "vec3 base=round(vColor.rgb*255.0)*ir/256.0;\n"
    "int raw=int(floor(max(depth-drawData[18].x,0.0)*drawData[18].y));\n"
    "float phase=(float((raw&65535)^32768)-32768.0)/4096.0;\n"
    "vec3 delta=clamp(floor(drawData[17].xyz*16.0-base),-32768.0,32767.0);\n"
    "vColor.rgb=clamp(floor(floor(base+phase*delta)/16.0),0.0,255.0)/255.0;}\n"
    "}\n"
    "}\n";

static const char hd_fragment_source[] =
    "in float vWater;\n"
    "flat in uvec4 vRegion;flat in uint vRepeat;flat in uvec4 vTex; flat in uvec2 vRect; flat in vec2 vMaskAlpha;\n"
    "flat in uvec2 vPrimitive;flat in float vPass;\n"
    "uint vidPsxWord(ivec2 p){\n"
    "uvec4 c=uvec4(round(texelFetch(uTex,p&ivec2(1023,511),0)*vec4(31.0,31.0,31.0,1.0)));\n"
    "return c.r|(c.g<<5)|(c.b<<10)|(c.a<<15);}\n"
    "ivec2 hdTextureAddress(ivec2 pixel,ivec4 region,uint repeatAxes);\n"
    "#define VID_PSX_ADDRESS hdTextureAddress\n"
    VID_PSX_SAMPLING_GLSL
    "ivec2 hdTextureAddress(ivec2 pixel,ivec4 region,uint repeatAxes){\n"
    "if(vRect.x==0u)return vidPsxAddress(pixel,region,repeatAxes);\n"
    "uvec2 raw=uvec2(pixel+region.xy)&uvec2(255u);uvec2 mask=uvec2(vRect.x&255u,vRect.x>>8);\n"
    "ivec2 mapped=ivec2((raw&(~mask))|(uvec2(vRect.y&255u,vRect.y>>8)&mask));\n"
    "return clamp(mapped,region.xy,region.xy+region.zw-1); }\n"
    "void main(){\n"
    "if(vWater<0.0)discard;\n"
    "vec3 color=vColor.rgb;bool stp=true;\n"
    "if((vTex.z&1u)!=0u){\n"
    "ivec2 pixel=ivec2(floor(vUV));\n"
    "#ifdef HD_UI_TEXTURE\n"
    "pixel=clamp(pixel,ivec2(int(vRect.x&255u),int(vRect.y&255u)),ivec2(int(vRect.x>>8),int(vRect.y>>8)));\n"
    "uvec2 uv=uvec2(pixel)&uvec2(255u);\n"
    "#else\n"
    "uvec2 uv=uvec2(pixel)&uvec2(255u);uvec2 mask=uvec2(vRect.x&255u,vRect.x>>8);\n"
    "uv=(uv&(~mask))|(uvec2(vRect.y&255u,vRect.y>>8)&mask);\n"
    "#endif\n"
    "uint word=vidPsxDecode(vTex.x,vTex.y,ivec2(uv));if(word==0u)discard;vec3 texel;\n"
    "if(vRegion.z>0u&&vRegion.w>0u){uint cls=uint(vMaskAlpha.x);\n"
    "if(vPass>0.0&&(vPrimitive.x&64u)!=0u)cls=vPass==1.0?1u:2u;\n"
    "if(vidPsxColor(word,cls,false).a==0.0)discard;\n"
    "vec4 filtered=vidPsxFilter(vUV-vec2(vRegion.xy),vTex.x,vTex.y,ivec4(vRegion),vRepeat,cls,false);\n"
    "if(filtered.a==0.0)discard;texel=filtered.rgb;}\n"
    "else{texel=vidPsxColor(word,0u,false).rgb;}\n"
    "stp=(word&32768u)!=0u;\n"
    "color=texel*((vTex.z&2u)!=0u?vec3(1.0):vColor.rgb*2.0);}\n"
    "if(vPass>0.0){bool semi=(vPrimitive.x&64u)!=0u;bool textured=(vTex.z&1u)!=0u;\n"
    "if(vPass==1.0){if(semi&&(!textured||stp))discard;}\n"
    "else if(!semi||uint(vPass-2.0)!=vPrimitive.y||(textured&&!stp))discard;}\n"
    "if((vMaskAlpha.x==1.0&&stp)||(vMaskAlpha.x==2.0&&!stp))discard;\n"
    "outColor=vec4(clamp(color,0.0,1.0),vMaskAlpha.y);\n"
    "}\n";

static const char hd_ui_vertex_source[] =
    // UI reuses the float layout for current/previous coordinates and colors
    "layout(location=0) in vec4 aPos;layout(location=1) in vec2 aUV;layout(location=2) in vec4 aColor;\n"
    "layout(location=3) in vec4 aNormal;layout(location=4) in vec4 aExtra;uniform vec4 uDraw[1];\n"
    "out vec2 vUV;out vec4 vColor;out float vDepth;out vec2 vFog;out float vWater;\n"
    "flat out uvec4 vRegion;flat out uint vRepeat;flat out uvec4 vTex;flat out uvec2 vRect;flat out vec2 vMaskAlpha;flat out uvec2 vPrimitive;flat out float vPass;\n"
    "void main(){gl_Position=vec4(mix(aPos.zw,aPos.xy,uDraw[0].w),0.0,1.0);\n"
    "vUV=aUV;vColor=mix(aNormal,aColor,uDraw[0].w)/255.0;vDepth=0.0;vFog=vec2(0.0);vWater=1.0;\n"
    "vRegion=uvec4(0u);vRepeat=0u;uint texture=uint(aExtra.x);vTex=uvec4(texture&511u,texture>>9,uint(aExtra.y),0u);vRect=uvec2(aExtra.zw);\n"
    "vMaskAlpha=uDraw[0].xy;vPass=uDraw[0].z;\n"
    "vPrimitive=uvec2((vTex.z&4u)!=0u?64u:128u,(vTex.x>>5)&3u);}";

static const char hd_image_fragment_source[] =
    "uniform vec4 uDraw[2];\n"
    "uint vidPsxWord(ivec2 p){uvec4 c=uvec4(round(texelFetch(uTex,p&ivec2(1023,511),0)*vec4(31.0,31.0,31.0,1.0)));\n"
    "return c.r|(c.g<<5)|(c.b<<10)|(c.a<<15); }\n"
    VID_PSX_SAMPLING_GLSL
    "uint imageByte(int x,int y){return (vidPsxWord(ivec2(x>>1,y))>>((x&1)*8))&255u;}\n"
    "vec3 imageColor(ivec2 pixel){ivec2 p=clamp(pixel,ivec2(0),ivec2(uDraw[1].xy)-1);int y=int(uDraw[0].y)+p.y;\n"
    "if(uDraw[0].z>0.0){int x=int(uDraw[0].x)*2+p.x*3;\n"
    "return vec3(imageByte(x,y),imageByte(x+1,y),imageByte(x+2,y))/255.0;}\n"
    "return vidPsxColor(vidPsxWord(ivec2(int(uDraw[0].x)+p.x,y)),0u,true).rgb;}\n"
    "void main(){vec2 p=vUV-0.5;ivec2 base=ivec2(floor(p));vec2 f=fract(p);\n"
    "vec3 a=imageColor(base),b=imageColor(base+ivec2(1,0)),c=imageColor(base+ivec2(0,1)),d=imageColor(base+ivec2(1,1));\n"
    "outColor=vec4(mix(mix(a,b,f.x),mix(c,d,f.x),f.y),1.0); }\n";

static const char hd_effect_vertex_source[] =
    "layout(location=0) in vec4 aPos;layout(location=1) in vec2 aUV;layout(location=2) in vec4 aColor;\n"
    "layout(location=3) in vec4 aNormal;layout(location=4) in vec4 aExtra;\n"
    "uniform vec4 uDraw[11];uniform highp usampler2D uAux0;uniform highp usampler2D uAux1;\n"
    "out vec2 vUV;out vec4 vColor;out float vDepth;out vec2 vFog;out float vWater;\n"
    "flat out uvec4 vRegion;flat out uint vRepeat;flat out uvec4 vTex;flat out uvec2 vRect;flat out vec2 vMaskAlpha;flat out uvec2 vPrimitive;flat out float vPass;\n"
    "vec4 spriteCell(int sprite,int cell){int at=(sprite*7+cell)*2;\n"
    "uvec4 lo=texelFetch(uAux0,ivec2(at%1024,at/1024),0);at++;\n"
    "uvec4 hi=texelFetch(uAux0,ivec2(at%1024,at/1024),0);return uintBitsToFloat(lo|(hi<<16));}\n"
    "vec4 lineWord(int at){\n"
    "uvec4 lo=texelFetch(uAux1,ivec2(at%1024,at/1024),0);at++;\n"
    "uvec4 hi=texelFetch(uAux1,ivec2(at%1024,at/1024),0);return uintBitsToFloat(lo|(hi<<16));}\n"
    "vec4 lineCell(int line,int cell){return lineWord((line*14+cell)*2);}\n"
    "vec3 spriteColor(float value){uint c=uint(value);return vec3(c&255u,(c>>8)&255u,(c>>16)&255u)/255.0;}\n"
    "vec3 stripView(vec3 point){vec4 p=vec4(point,1.0);return vec3(dot(uDraw[3],p),dot(uDraw[4],p),dot(uDraw[5],p));}\n"
    "vec2 stripUV(vec4 words,uint corner){uint uv=uint(words[int(corner)]);\n"
    "return vec2((uvec2(uv&255u,uv>>8)-uvec2(corner&1u,corner>>1))&uvec2(255u));}\n"
    "void stripVertex(int id,uint corner,out vec3 position,out vec2 uv,out vec3 color,out uvec3 texture,out bool visible){\n"
    "float phase=uDraw[2].z;bool old=phase<=0.0;int begin=int(lineCell(id,9).w);\n"
    "bool connected=false;vec3 center=vec3(0.0),edge0=vec3(0.0),edge1=vec3(0.0),saved0=vec3(0.0),saved1=vec3(0.0);\n"
    "position=vec3(0.0);uv=vec2(0.0);color=vec3(0.0);texture=uvec3(0u);visible=false;\n"
    "for(int at=begin;at<=id;at++){vec4 a0=lineCell(at,0),a1=lineCell(at,1),b0=lineCell(at,2),b1=lineCell(at,3);\n"
    "vec4 cfg=lineCell(at,4),size=lineCell(at,5),ranges=lineCell(at,6),restart=lineCell(at,11);\n"
    "if((old&&ranges.x==ranges.y)||(phase>=1.0&&ranges.z==ranges.w))continue;\n"
    "vec3 a=stripView(mix(a0.xyz,a1.xyz,phase)),b=stripView(mix(b0.xyz,b1.xyz,phase));\n"
    "bool start=!connected||(old?restart.x:restart.y)>0.0;uint flags=uint(old?size.z:size.w);if(!start)a=center;connected=false;\n"
    "vec2 clip=old?cfg.xy:cfg.zw;if(a.z<clip.x||a.z>clip.y||b.z<clip.x||b.z>clip.y)continue;\n"
    "vec2 dir=vec2((b.x/b.z-a.x/a.z)/0.625,b.y/b.z-a.y/a.z);float len=length(dir);if(len<=0.0)continue;dir/=len;\n"
    "float width=mix(size.x,size.y,phase);vec2 normal=vec2(dir.y*0.625,-dir.x)/(uDraw[2].y*120.0);\n"
    "vec3 offsetA=vec3(normal*max(width/(2.0*a.z),2.0)*a.z,0.0),offsetB=vec3(normal*max(width/(2.0*b.z),2.0)*b.z,0.0);\n"
    "vec3 first0=start?a+offsetA:edge0,first1=start?a-offsetA:edge1;\n"
    "if(start&&(flags&16u)!=0u){saved0=first0;saved1=first1;}\n"
    "vec3 last0=(flags&32u)!=0u?saved0:b+offsetB,last1=(flags&32u)!=0u?saved1:b-offsetB;\n"
    "center=b;edge0=last0;edge1=last1;connected=true;if(at!=id)continue;\n"
    "uint edge=(corner>>((flags&2u)!=0u?0u:1u))&1u;bool end=((corner>>((flags&2u)!=0u?1u:0u))&1u)!=0u;\n"
    "if(((flags&1u)!=0u)!=((flags&2u)!=0u))edge=1u-edge;if((flags&2u)!=0u)end=!end;\n"
    "position=end?(edge!=0u?last1:last0):(edge!=0u?first1:first0);\n"
    "vec4 words=lineCell(at,old?7:8);uint other=corner^((flags&2u)!=0u?2u:1u);\n"
    "vec2 from=stripUV(words,end?other:corner),to=stripUV(words,end?corner:other);\n"
    "vec2 range=mix(ranges.xy,ranges.zw,phase);uv=mix(from,to,end?range.y:range.x);\n"
    "color=mix(spriteColor(a0.w),spriteColor(a1.w),phase);texture=uvec3(lineCell(at,old?9:10).xyz);visible=true;}}\n"
    "vec3 beamPoint(int id){return stripView(mix(lineCell(id,0).xyz,lineCell(id,1).xyz,uDraw[2].z));}\n"
    "vec2 beamDirection(int a,int b,vec2 clip){vec3 p=beamPoint(a),q=beamPoint(b);\n"
    "if(p.z<clip.x||p.z>clip.y||q.z<clip.x||q.z>clip.y)return vec2(0.0);\n"
    "vec2 d=(q.xy/q.z-p.xy/p.z)*(uDraw[2].y*120.0);float x=abs(d.x),y=abs(d.y);\n"
    "float norm=max(x,y)+min(x,y)*0.5;if(norm<2.0||norm<=0.0)return vec2(0.0);return vec2(d.y/(norm*0.625),-d.x/norm);}\n"
    "vec2 beamNormal(int id,int first,int last,vec2 clip){if(id==first)return beamDirection(id,id+1,clip);\n"
    "if(id==last)return beamDirection(id-1,id,clip);return (beamDirection(id-1,id,clip)+beamDirection(id,id+1,clip))*0.5;}\n"
    "void beamVertex(int id,int layer,uint corner,out vec3 p,out vec3 color,out uint page,out bool visible){\n"
    "vec4 cfg=lineCell(id,4),mode=lineCell(id,5);int first=int(cfg.z),last=first+int(cfg.w)-1;float phase=uDraw[2].z;\n"
    "vec2 previous=beamNormal(id-1,first,last,cfg.xy),current=beamNormal(id,first,last,cfg.xy);\n"
    "visible=any(notEqual(previous,vec2(0.0)))&&any(notEqual(current,vec2(0.0)));\n"
    "if(layer==0)visible=visible&&(phase<=0.0?(mode.x>=3.0&&mode.z>0.0):(mode.y>=3.0&&mode.w>0.0));\n"
    "int point=(corner&1u)!=0u?id-1:id;vec4 a=lineCell(point,0),b=lineCell(point,1),old=lineCell(point,2),now=lineCell(point,3);\n"
    "p=beamPoint(point);color=corner<2u?vec3(0.0):mix(spriteColor(old.z),spriteColor(now.z),phase);page=uint(phase<=0.0?old.w:now.w);\n"
    "if(corner<2u){if(layer==0){vec2 screen=mix(old.xy,now.xy,phase);float scale=p.z/(uDraw[2].y*120.0);\n"
    "p.xy=vec2((screen.x-256.0-uDraw[1].x*256.0)*0.625,screen.y-120.0+uDraw[1].y*120.0)*scale;}\n"
    "else{vec2 normal=(corner&1u)!=0u?previous:current;float width=400.0*mix(a.w,b.w,phase)/(uDraw[2].y*120.0);\n"
    "p.xy+=vec2(normal.x*0.625,normal.y)*width*(layer==1?1.0:-1.0);}}}\n"
    "bool ribbonClip(inout vec3 a,inout vec3 b,vec2 clip){\n"
    "if((a.z<clip.x&&b.z<clip.x)||(a.z>clip.y&&b.z>clip.y))return false;\n"
    "float start=0.0,end=1.0,dz=b.z-a.z;if(dz!=0.0){\n"
    "if(a.z<clip.x)start=(clip.x-a.z)/dz;if(b.z<clip.x)end=(clip.x-a.z)/dz;\n"
    "if(a.z>clip.y)start=(clip.y-a.z)/dz;if(b.z>clip.y)end=(clip.y-a.z)/dz;}\n"
    "vec3 old=a;a=mix(old,b,start);b=mix(old,b,end);return true;}\n"
    "vec3 ribbonEdge(vec3 p,vec2 normal,float extent,int edge){\n"
    "p.xy+=vec2(normal.x*0.625,normal.y)*(400.0*extent/(uDraw[2].y*120.0))*(edge%2==0?1.0:-1.0);return p;}\n"
    "vec2 ribbonWave(uint tick){float rad=0.0015339807878856412;\n"
    "return vec2(sin(float((tick*200u+130u)&4095u)*rad)*2.0,\n"
    "sin(float(((tick*1000u)&16376u)>>2)*rad)*3.0+sin(float((tick*150u+100u)&4095u)*rad)*2.0);}\n"
    "float ribbonUV(float a,float b,float phase){if(phase<=0.0)return a;if(phase>=1.0)return b;return a+(mod(b-a+32.0,64.0)-32.0)*phase;}\n"
    "void ribbonVertex(int id,int layer,uint corner,out vec3 p,out vec2 uv,out vec3 color,out uvec3 texture,out uvec2 rect,out bool visible){\n"
    "float phase=uDraw[2].z;bool old=phase<=0.0,start=true;vec4 group=lineCell(id,3);int first=int(group.z);uint column=0u;\n"
    "vec2 normal=vec2(0.0);vec3 previous[4];for(int i=0;i<4;i++)previous[i]=vec3(0.0);\n"
    "p=vec3(0.0);uv=vec2(0.0);color=vec3(0.0);texture=uvec3(0u);rect=uvec2(0u);visible=false;\n"
    "for(int at=first+1;at<=id;at++){vec3 a=beamPoint(at-1),b=beamPoint(at);vec4 cfg=lineCell(at,4);\n"
    "if(!ribbonClip(a,b,cfg.xy)){normal=vec2(0.0);start=true;continue;}\n"
    "vec2 d=(b.xy/b.z-a.xy/a.z)*(uDraw[2].y*120.0);float norm=max(abs(d.x),abs(d.y))+min(abs(d.x),abs(d.y))*0.5;\n"
    "vec2 next=norm>=3.0?vec2(d.y/(norm*0.625),-d.x/norm):vec2(0.0),initial=normal;\n"
    "if(any(notEqual(normal,vec2(0.0)))){if(any(notEqual(next,vec2(0.0))))normal=next;}\n"
    "else{if(all(equal(next,vec2(0.0)))){start=true;continue;}initial=normal=next;}\n"
    "vec4 sizeA=lineCell(at-1,2),sizeB=lineCell(at,2);\n"
    "float widthA=mix(lineCell(at-1,0).w,lineCell(at-1,1).w,phase),widthB=mix(lineCell(at,0).w,lineCell(at,1).w,phase);\n"
    "vec3 edges[4];for(int edge=0;edge<4;edge++){if(start)previous[edge]=ribbonEdge(a,initial,edge<2?widthA:mix(sizeA.x,sizeA.y,phase),edge);\n"
    "edges[edge]=ribbonEdge(b,normal,edge<2?widthB:mix(sizeB.x,sizeB.y,phase),edge);}\n"
    "vec4 state=lineCell(at,7);if(at-first-1>=int(group.w)/2){\n"
    "vec2 wave=mix(ribbonWave(uint(state.z))*state.x,ribbonWave(uint(state.w))*state.y,phase);\n"
    "edges[0].y+=wave.x*b.z/(uDraw[2].y*120.0);edges[1].y+=wave.y*b.z/(uDraw[2].y*120.0);}\n"
    "if(at==id){bool end=(corner&1u)==0u,center=(corner&2u)!=0u;int edge=center?max(layer-2,0):layer;\n"
    "p=center&&layer<2?(end?b:a):(end?edges[edge]:previous[edge]);int point=end?at:at-1;\n"
    "vec4 colors=lineCell(point,2),outer=lineCell(point,3);color=center?(layer<2?mix(spriteColor(colors.z),spriteColor(colors.w),phase):mix(spriteColor(outer.x),spriteColor(outer.y),phase)):(layer<2?mix(spriteColor(outer.x),spriteColor(outer.y),phase):vec3(0.0));\n"
    "vec4 base=lineCell(at,5),scroll=lineCell(at,6);uint oldBase=uint(base.x),newBase=uint(base.y);\n"
    "uint offset=center?(layer<2?32u:(layer==2?16u:48u)):(layer==0?16u:(layer==1?48u:(layer==2?0u:64u)));\n"
    "uvec2 uv0=uvec2((oldBase&255u)+uint(base.z)+offset,(oldBase>>8)+((uint(scroll.x)-column*uint(scroll.z))&63u)+(end?0u:uint(scroll.z)))&uvec2(255u);\n"
    "uvec2 uv1=uvec2((newBase&255u)+uint(base.w)+offset,(newBase>>8)+((uint(scroll.y)-column*uint(scroll.w))&63u)+(end?0u:uint(scroll.w)))&uvec2(255u);\n"
    "uv=vec2(ribbonUV(float(uv0.x),float(uv1.x),phase),ribbonUV(float(uv0.y),float(uv1.y),phase));\n"
    "uint material=uint(old?cfg.z:cfg.w),origin=old?oldBase:newBase;texture=uvec3(material&511u,material>>9,13u);\n"
    "uint window=792u|(((origin&255u)>>3)<<10)|((origin>>11)<<15);\n"
    "rect=uvec2(((window&31u)<<3)|((window&992u)<<6),((window>>7)&248u)|((window>>4)&63488u));visible=true;}\n"
    "for(int edge=0;edge<4;edge++)previous[edge]=edges[edge];start=false;column++;}}\n"
    "void ringVertex(int id,uint corner,out vec3 p,out vec3 color,out uvec3 texture,out bool visible){\n"
    "float phase=uDraw[2].z;bool old=phase<=0.0;vec4 a=lineCell(id,0),b=lineCell(id,1),angles=lineCell(id,2),cfg=lineCell(id,7);\n"
    "p=stripView(mix(a.xyz,b.xyz,phase))+uDraw[10].xyz;visible=p.z>0.0&&p.z<=20000.0&&(old?cfg.z:cfg.w)>0.0;\n"
    "texture=uvec3(uint(old?a.w:b.w),0u,uint(old?cfg.x:cfg.y));bool fan=lineCell(id,8).x==3.0;\n"
    "vec2 rotation=angles.xy+(mod(angles.zw-angles.xy+3.14159265359,6.28318530718)-3.14159265359)*phase;\n"
    "if(phase<=0.0)rotation=angles.xy;else if(phase>=1.0)rotation=angles.zw;\n"
    "vec4 radii=mix(lineCell(id,3),lineCell(id,4),phase);\n"
    "if(!fan||corner>0u){uint ray=fan?corner-1u:corner&1u;float radius=radii[int(fan?corner+1u:corner)],angle=rotation[int(ray)];\n"
    "p.xy+=vec2(sin(angle)*625.0*0.625,-cos(angle)*400.0)*radius/(uDraw[2].y*120.0);}\n"
    "vec3 rgb=mix(spriteColor(lineCell(id,5)[int(corner)]),spriteColor(lineCell(id,6)[int(corner)]),phase)*255.0;\n"
    "float fade=p.z<700.0?floor(255.0*max(p.z,0.0)/700.0)+1.0:256.0;color=floor(rgb*fade/256.0)/255.0;}\n"
    "void polygonVertex(int id,uint corner,out vec3 p,out vec2 uv,out vec3 color,out uvec3 texture,out uvec2 rect,out bool visible){\n"
    "float phase=uDraw[2].z;bool old=phase<=0.0;vec4 a=lineCell(id,int(corner)),b=lineCell(id,int(corner)+4),cfg=lineCell(id,10),window=lineCell(id,11);\n"
    "p=stripView(mix(a.xyz,b.xyz,phase));color=mix(spriteColor(a.w),spriteColor(b.w),phase);\n"
    "uint uv0=uint(lineCell(id,8)[int(corner)]),uv1=uint(lineCell(id,9)[int(corner)]);vec2 from=vec2(uv0&255u,uv0>>8),to=vec2(uv1&255u,uv1>>8);\n"
    "float period=old?window.z:window.w;uv=old?from:to;if(phase>0.0&&phase<1.0&&period>0.0)uv=from+(mod(to-from+period*0.5,period)-period*0.5)*phase;\n"
    "uint material=uint(old?cfg.x:cfg.y),flags=uint(old?cfg.z:cfg.w),mask=uint(old?window.x:window.y);\n"
    "texture=uvec3(material&511u,material>>9,flags&255u);visible=(flags&256u)!=0u;\n"
    "if((flags&512u)!=0u){vec3 p0=stripView(mix(lineCell(id,0).xyz,lineCell(id,4).xyz,phase));\n"
    "vec3 p1=stripView(mix(lineCell(id,1).xyz,lineCell(id,5).xyz,phase)),p2=stripView(mix(lineCell(id,2).xyz,lineCell(id,6).xyz,phase));\n"
    "visible=visible&&dot(p0,cross(p1,p2))<=0.0;}\n"
    "rect=uvec2(((mask&31u)<<3)|((mask&992u)<<6),((mask>>7)&248u)|((mask>>4)&63488u));}\n"
    "vec3 modelUnit(vec3 v){float n=length(v);return n>0.0?v/n:vec3(0.0);}\n"
    "void modelVertex(int id,uint corner,out vec3 p,out vec2 uv,out vec3 color,out uvec3 texture,out bool visible){\n"
    "p=vec3(0.0);uv=vec2(0.0);color=vec3(0.0);texture=uvec3(0u);visible=false;float phase=uDraw[2].z;bool old=phase<=0.0;\n"
    "vec4 min0=lineCell(id,0),max0=lineCell(id,1),min1=lineCell(id,2),max1=lineCell(id,3);\n"
    "vec3 low=mix(min0.xyz,min1.xyz,phase),high=mix(max0.xyz,max1.xyz,phase),extent=(high-low)*0.5;bool project=(old?max0.w:max1.w)>0.0;\n"
    "extent*=project?vec3(1.25,1.375,1.25):vec3(mix(min0.w,min1.w,phase));if(any(lessThanEqual(extent,vec3(0.0))))return;\n"
    "int pose=int(uDraw[6].x)*28+int(lineCell(id,8).w)*6;vec4 m0=lineWord(pose),m1=lineWord(pose+2),m2=lineWord(pose+4);\n"
    "mat3 matrix=transpose(mat3(m0.xyz,m1.xyz,m2.xyz));vec3 center=matrix*((low+high)*0.5)+vec3(m0.w,m1.w,m2.w);\n"
    "vec4 dir0=lineCell(id,4),dir1=lineCell(id,5),normal0=lineCell(id,6),normal1=lineCell(id,7);\n"
    "vec3 direction=mix(dir0.xyz,dir1.xyz,phase);if(!project){mat3 view=transpose(mat3(uDraw[3].xyz,uDraw[4].xyz,uDraw[5].xyz));\n"
    "direction=modelUnit(center+transpose(view)*vec3(uDraw[3].w,uDraw[4].w,uDraw[5].w));}\n"
    "float average=(extent.x+extent.y+extent.z)/3.0;vec3 diagonal=vec3(average)/extent;\n"
    "vec3 normal=modelUnit(transpose(matrix)*direction),n=diagonal*normal,axis=vec3(0.0);float size=length(n);\n"
    "if(abs(n.x)<=abs(n.y)&&abs(n.x)<=abs(n.z))axis.x=1.0;else if(abs(n.y)<=abs(n.x)&&abs(n.y)<=abs(n.z))axis.y=1.0;else axis.z=1.0;\n"
    "vec3 tangent=cross(axis,n),basis0=cross(direction,matrix*(diagonal*modelUnit(tangent)));\n"
    "vec3 basis1=cross(direction,matrix*(diagonal*modelUnit(cross(tangent,n))));\n"
    "float factor=extent.x*extent.y*extent.z*size/(average*average);basis0*=factor;basis1*=factor;\n"
    "if(project){normal=mix(normal0.xyz,normal1.xyz,phase);vec3 plane=mix(lineCell(id,8).xyz,lineCell(id,9).xyz,phase)-center;\n"
    "float alignment=dot(direction,normal),distance=dot(plane,normal);if(alignment<0.25||distance>0.0)return;\n"
    "center+=direction*(distance/alignment);center.y-=0.5;basis0-=direction*(dot(basis0,normal)/alignment);basis1-=direction*(dot(basis1,normal)/alignment);}\n"
    "p=stripView(center+((corner&1u)!=0u?1.0:-1.0)*basis0+((corner&2u)!=0u?1.0:-1.0)*basis1);\n"
    "uint word=uint(lineCell(id,old?10:11)[int(corner)]);uv=vec2((uvec2(word&255u,word>>8)-uvec2(corner&1u,corner>>1))&uvec2(255u));\n"
    "color=mix(spriteColor(dir0.w),spriteColor(dir1.w),phase);uint material=uint(old?normal0.w:normal1.w);texture=uvec3(material&511u,material>>9,13u);visible=true;}\n"
    "void main(){vec4 world=vec4(aPos.xyz,1.0);vec3 p=vec3(dot(uDraw[3],world),dot(uDraw[4],world),dot(uDraw[5],world));\n"
    "vec2 uv=aUV;vec3 color=aColor.rgb/255.0;bool visible=true;uvec3 stripTexture=uvec3(0u);uint beamPage=0u;\n"
    "uvec2 ribbonRect=uvec2(0u);bool model=aNormal.w<0.0&&aNormal.y>=11.0,polygon=aNormal.w<0.0&&aNormal.y==10.0,ring=aNormal.w<0.0&&aNormal.y==9.0,ribbon=aNormal.w<0.0&&aNormal.y>=5.0&&aNormal.y<9.0;\n"
    "bool beam=aNormal.w<0.0&&aNormal.y>=2.0&&aNormal.y<5.0,strip=aNormal.w<0.0&&aNormal.y==1.0;\n"
    "if(model){modelVertex(int(-aNormal.w)-1,uint(aNormal.x),p,uv,color,stripTexture,visible);}\n"
    "else if(polygon){polygonVertex(int(-aNormal.w)-1,uint(aNormal.x),p,uv,color,stripTexture,ribbonRect,visible);}\n"
    "else if(ring){ringVertex(int(-aNormal.w)-1,uint(aNormal.x),p,color,stripTexture,visible);}\n"
    "else if(ribbon){ribbonVertex(int(-aNormal.w)-1,int(aNormal.y)-5,uint(aNormal.x),p,uv,color,stripTexture,ribbonRect,visible);}\n"
    "else if(beam){beamVertex(int(-aNormal.w)-1,int(aNormal.y)-2,uint(aNormal.x),p,color,beamPage,visible);}\n"
    "else if(strip){stripVertex(int(-aNormal.w)-1,uint(aNormal.x),p,uv,color,stripTexture,visible);}\n"
    "else if(aNormal.w<0.0){int id=int(-aNormal.w)-1;float phase=uDraw[2].z;\n"
    "vec4 a0=lineCell(id,0),a1=lineCell(id,1),b0=lineCell(id,2),b1=lineCell(id,3),cfg=lineCell(id,4);\n"
    "vec4 wa=vec4(mix(a0.xyz,a1.xyz,phase),1.0),wb=vec4(mix(b0.xyz,b1.xyz,phase),1.0);\n"
    "vec3 a=vec3(dot(uDraw[3],wa),dot(uDraw[4],wa),dot(uDraw[5],wa));\n"
    "vec3 b=vec3(dot(uDraw[3],wb),dot(uDraw[4],wb),dot(uDraw[5],wb));\n"
    "visible=!((a.z<cfg.x&&b.z<cfg.x)||(a.z>cfg.y&&b.z>cfg.y));\n"
    "if(cfg.w==2.0)visible=visible&&b.z>=cfg.x&&b.z<=cfg.y;\n"
    "float start=0.0,end=1.0,dz=b.z-a.z;\n"
    "if(cfg.z>0.0){if(dz!=0.0){if(a.z<cfg.x)start=(cfg.x-a.z)/dz;if(b.z<cfg.x)end=(cfg.x-a.z)/dz;\n"
    "if(a.z>cfg.y)start=(cfg.y-a.z)/dz;if(b.z>cfg.y)end=(cfg.y-a.z)/dz;}}\n"
    "else visible=visible&&a.z>=cfg.x&&b.z>=cfg.x&&a.z<=cfg.y&&b.z<=cfg.y;\n"
    "vec3 pa=mix(a,b,start),pb=mix(a,b,end);float za=max(pa.z,0.0001),zb=max(pb.z,0.0001);\n"
    "vec2 dir=vec2((pb.x/zb-pa.x/za)/0.625,pb.y/zb-pa.y/za);float len=length(dir);visible=visible&&len>0.0;\n"
    "dir/=len>0.0?len*2.0:1.0;uint corner=uint(aNormal.x);p=(corner&1u)!=0u?pb:pa;\n"
    "vec2 offset=vec2(dir.y*0.625,-dir.x)*p.z/(uDraw[2].y*120.0);if((corner&2u)!=0u)offset=-offset;p.xy+=offset;\n"
    "color=(corner&1u)!=0u?mix(spriteColor(b0.w),spriteColor(b1.w),phase):mix(spriteColor(a0.w),spriteColor(a1.w),phase);\n"
    "}else if(aNormal.w>0.0){int id=int(aNormal.w)-1;float phase=uDraw[2].z;\n"
    "vec4 a=spriteCell(id,0),b=spriteCell(id,1),angles=spriteCell(id,2),scale=spriteCell(id,3),image=spriteCell(id,4),colors=spriteCell(id,5);\n"
    "world=vec4(mix(a.xyz,b.xyz,phase),1.0);p=vec3(dot(uDraw[3],world),dot(uDraw[4],world),dot(uDraw[5],world))+uDraw[10].xyz;\n"
    "float size=mix(a.w,b.w,phase),distance=uDraw[2].y*120.0,z=max(p.z,0.0001);\n"
    "float angle=angles.z+(mod(angles.w-angles.z+3.14159265359,6.28318530718)-3.14159265359)*phase;\n"
    "if(phase<=0.0)angle=angles.z;else if(phase>=1.0)angle=angles.w;\n"
    "vec2 factors=mix(angles.xy,scale.xy,phase);vec2 left=image.zw*size/distance,right=left+image.xy*size/distance;\n"
    "if(colors.x>0.0){left=vec2(0.0);right=factors*z/distance;factors=vec2(1.0);angle=0.0;}\n"
    "else if(angle==0.0){if(image.x*size/z<5.0)right.x=left.x+2.0*z/distance;\n"
    "if(image.y*size/z<5.0)right.y=left.y+z/distance;factors=vec2(1.0);}\n"
    "vec2 corner=mix(left,right,aPos.xy);float sine=sin(angle),cosine=cos(angle);\n"
    "p.xy+=vec2((corner.x*cosine-corner.y*sine)*0.625,(corner.x*sine+corner.y*cosine))*factors;\n"
    "visible=p.z>=scale.z&&p.z<=scale.w&&(colors.x>0.0||colors.w>0.0||size*128.0/z<513.0);\n"
    "if(size*128.0/z>=256.0||angle!=0.0){uint cornerIndex=uint(aNormal.x);uv=vec2((uvec2(aUV)-uvec2(cornerIndex&1u,cornerIndex>>1))&uvec2(255u));}\n"
    "color=mix(spriteColor(colors.y),spriteColor(colors.z),phase);\n"
    "}else if((uint(aExtra.y)&256u)!=0u)p+=aNormal.xyz/uDraw[2].y;\n"
    "gl_Position=vec4(p.x*uDraw[0].x,-p.y*uDraw[0].y,uDraw[0].z*p.z+uDraw[0].w,p.z);\n"
    "float forced=polygon&&uDraw[2].z<=0.0?aNormal.z:aPos.w;\n"
    "if(forced>0.0)gl_Position.z=clamp(uDraw[0].z+uDraw[0].w/forced,-1.0,1.0)*p.z;\n"
    "gl_Position.xy+=uDraw[1].xy*p.z;if(!visible)gl_Position=vec4(2.0,2.0,2.0,1.0);\n"
    "vUV=uv;vColor=vec4(color,1.0);vDepth=p.z;vFog=vec2(0.0);vWater=1.0;\n"
    "vRegion=uvec4(0u);vRepeat=0u;uint texture=uint(aExtra.x);vTex=uvec4(texture&511u,texture>>9,uint(aExtra.y)&255u,0u);vRect=uvec2(aExtra.zw);\n"
    "if(aNormal.w==0.0&&(vTex.z&8u)!=0u)vRegion=uvec4(uDraw[9]);\n"
    "if(aNormal.w>0.0&&(vTex.z&8u)!=0u)vRegion=uvec4(spriteCell(int(aNormal.w)-1,6));\n"
    "if(strip)vTex.xyz=stripTexture;\n"
    "if(ring)vTex.xyz=stripTexture;\n"
    "if(model)vTex.xyz=stripTexture;\n"
    "if(ribbon||polygon){vTex.xyz=stripTexture;vRect=ribbonRect;}\n"
    "if(beam)vTex.x=beamPage;\n"
    "if((polygon||strip||model||ribbon)&&(vTex.z&8u)!=0u)vRegion=uvec4(lineCell(int(-aNormal.w)-1,uDraw[2].z<=0.0?12:13));\n"
    "vMaskAlpha=uDraw[1].zw;vPass=uDraw[2].x;\n"
    "vPrimitive=uvec2((vTex.z&4u)!=0u?64u:128u,(vTex.x>>5)&3u);}";

void hd_pose_from_matrix(HD_POSE *pose, const MATRIX *rotation, const VECTORF *pos, const VECTORF *scale)
{
    MATRIXF matrix;
    uint32 row, column;
    IdentityMatrixf(&matrix);
    for (row = 0; row < 3; ++row)
        for (column = 0; column < 3; ++column)
            matrix.m[row][column] = rotation->m[row][column] / 4096.0f;
    pose->pos = *pos;
    pose->scale = *scale;
    MatrixQuatf(&matrix, &pose->rotation);
}

void hd_pose_seed(HD_POSE_HISTORY *history, const HD_POSE *pose)
{
    history->previous = *pose;
    history->current = *pose;
    history->initialized = 1;
}

void hd_pose_capture(HD_POSE_HISTORY *history, const HD_POSE *pose)
{
    if (!history->initialized)
        hd_pose_seed(history, pose);
    else
    {
        history->previous = history->current;
        history->current = *pose;
    }
}

void hd_pose_matrix(const HD_POSE_HISTORY *history, float fract, MATRIXF *matrix)
{
    QUATF rotation;
    VECTORF pos, scale;
    const HD_POSE *a = &history->previous, *b = &history->current;
    float phase = fract < 0 ? 0 : (fract > 1 ? 1 : fract);
    pos.vx = a->pos.vx + (b->pos.vx - a->pos.vx) * phase;
    pos.vy = a->pos.vy + (b->pos.vy - a->pos.vy) * phase;
    pos.vz = a->pos.vz + (b->pos.vz - a->pos.vz) * phase;
    scale.vx = a->scale.vx + (b->scale.vx - a->scale.vx) * phase;
    scale.vy = a->scale.vy + (b->scale.vy - a->scale.vy) * phase;
    scale.vz = a->scale.vz + (b->scale.vz - a->scale.vz) * phase;
    SlerpQuatf(&a->rotation, &b->rotation, phase, &rotation);
    QuatMatrixf(&rotation, matrix);
    ScaleMatrixf(matrix, &scale);
    TransMatrixf(matrix, &pos);
}

static void hd_matrix_rotation(const MATRIXF *matrix, QUATF *rotation)
{
    VECTORF x = {matrix->m[0][0], matrix->m[1][0], matrix->m[2][0]};
    VECTORF y = {matrix->m[0][1], matrix->m[1][1], matrix->m[2][1]}, z;
    MATRIXF basis;
    float dot;
    if (NormalizeVectorf(&x, &x) < 0.000001f)
        return;
    dot = x.vx * y.vx + x.vy * y.vy + x.vz * y.vz;
    y.vx -= dot * x.vx;
    y.vy -= dot * x.vy;
    y.vz -= dot * x.vz;
    if (NormalizeVectorf(&y, &y) < 0.000001f)
        return;
    OuterProductf(&x, &y, &z);
    IdentityMatrixf(&basis);
    basis.m[0][0] = x.vx; basis.m[1][0] = x.vy; basis.m[2][0] = x.vz;
    basis.m[0][1] = y.vx; basis.m[1][1] = y.vy; basis.m[2][1] = y.vz;
    basis.m[0][2] = z.vx; basis.m[1][2] = z.vy; basis.m[2][2] = z.vz;
    MatrixQuatf(&basis, rotation);
}

void hd_matrix_seed(HD_MATRIX_HISTORY *history, const MATRIXF *matrix)
{
    history->current = history->previous = *matrix;
    history->current_rotation.x = history->current_rotation.y = history->current_rotation.z = 0;
    history->current_rotation.w = 1;
    hd_matrix_rotation(matrix, &history->current_rotation);
    history->previous_rotation = history->current_rotation;
    history->initialized = 1;
}

void hd_matrix_capture(HD_MATRIX_HISTORY *history, const MATRIXF *matrix)
{
    if (!history->initialized)
    {
        hd_matrix_seed(history, matrix);
        return;
    }
    history->previous = history->current;
    history->previous_rotation = history->current_rotation;
    history->current = *matrix;
    hd_matrix_rotation(matrix, &history->current_rotation);
}

void hd_matrix_evaluate(const HD_MATRIX_HISTORY *history, float phase, MATRIXF *matrix)
{
    MATRIXF a, b, residual_a, residual_b, rotation;
    QUATF quaternion;
    uint32 row, column;
    if (phase <= 0)
    {
        *matrix = history->previous;
        return;
    }
    if (phase >= 1)
    {
        *matrix = history->current;
        return;
    }
    QuatMatrixf(&history->previous_rotation, &a);
    QuatMatrixf(&history->current_rotation, &b);
    TransposeMatrixf(&a, &a);
    TransposeMatrixf(&b, &b);
    MulMatrix0f(&a, &history->previous, &residual_a);
    MulMatrix0f(&b, &history->current, &residual_b);
    // Retain signed scale and shear instead of discarding the affine residual
    for (row = 0; row < 3; ++row)
        for (column = 0; column < 3; ++column)
            residual_a.m[row][column] += (residual_b.m[row][column] - residual_a.m[row][column]) * phase;
    SlerpQuatf(&history->previous_rotation, &history->current_rotation, phase, &quaternion);
    QuatMatrixf(&quaternion, &rotation);
    MulMatrix0f(&rotation, &residual_a, matrix);
    for (row = 0; row < 3; ++row)
        matrix->t[row] = history->previous.t[row] + (history->current.t[row] - history->previous.t[row]) * phase;
}

int hd_matrix_relative(const MATRIXF *parent, const MATRIXF *child, MATRIXF *local)
{
    MATRIXF inverse;
    VECTORF delta, translation;
    const float (*m)[3] = parent->m;
    float determinant = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    if (determinant == 0)
        return 0;
    IdentityMatrixf(&inverse);
    inverse.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) / determinant;
    inverse.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) / determinant;
    inverse.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) / determinant;
    inverse.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) / determinant;
    inverse.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) / determinant;
    inverse.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) / determinant;
    inverse.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) / determinant;
    inverse.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) / determinant;
    inverse.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) / determinant;
    MulMatrix0f(&inverse, child, local);
    delta.vx = child->t[0] - parent->t[0];
    delta.vy = child->t[1] - parent->t[1];
    delta.vz = child->t[2] - parent->t[2];
    ApplyMatrixf(&inverse, &delta, &translation);
    local->t[0] = translation.vx;
    local->t[1] = translation.vy;
    local->t[2] = translation.vz;
    return 1;
}

void hd_projection(HD_CAMERA *camera, float distance, float width, float height, float center_x, float center_y, float near_depth, float far_depth)
{
    camera->mode = 0;
    camera->focal_y = 2 * distance / height;
    camera->center_x = 2 * center_x / width - 1;
    camera->center_y = 1 - 2 * center_y / height;
    camera->near_depth = near_depth;
    camera->far_depth = far_depth;
}

void hd_camera_view(HD_CAMERA *camera, const HD_POSE_HISTORY *history, float fract)
{
    MATRIXF world;
    VECTORF pos, translated;
    QUATF rotation;
    const HD_POSE *a = &history->previous, *b = &history->current;
    float phase = fract < 0 ? 0 : (fract > 1 ? 1 : fract);
    SlerpQuatf(&a->rotation, &b->rotation, phase, &rotation);
    QuatMatrixf(&rotation, &world);
    TransposeMatrixf(&world, &camera->view);
    pos.vx = -a->pos.vx - (b->pos.vx - a->pos.vx) * phase;
    pos.vy = -a->pos.vy - (b->pos.vy - a->pos.vy) * phase;
    pos.vz = -a->pos.vz - (b->pos.vz - a->pos.vz) * phase;
    ApplyMatrixf(&camera->view, &pos, &translated);
    TransMatrixf(&camera->view, &translated);
}

void hd_camera_set_view(HD_CAMERA *camera, const QUATF *rotation, const VECTORF *position)
{
    VECTORF negative = {-position->vx, -position->vy, -position->vz}, translated;
    QuatMatrixf(rotation, &camera->view);
    ApplyMatrixf(&camera->view, &negative, &translated);
    TransMatrixf(&camera->view, &translated);
}

void hd_camera_capture(HD_CAMERA_HISTORY *history, const HD_CAMERA *camera, int reset)
{
    HD_POSE pose;
    MATRIXF world;
    VECTORF translation = {-camera->view.t[0], -camera->view.t[1], -camera->view.t[2]};
    IdentityMatrixf(&world);
    TransposeMatrixf(&camera->view, &world);
    ApplyMatrixf(&world, &translation, &pose.pos);
    MatrixQuatf(&world, &pose.rotation);
    pose.scale.vx = pose.scale.vy = pose.scale.vz = 1;
    if (reset || !history->pose.initialized || history->current.mode != camera->mode || history->current.near_depth != camera->near_depth || history->current.far_depth != camera->far_depth)
    {
        hd_pose_seed(&history->pose, &pose);
        history->previous = *camera;
    }
    else
    {
        hd_pose_capture(&history->pose, &pose);
        history->previous = history->current;
    }
    history->current = *camera;
}

void hd_camera_evaluate(const HD_CAMERA_HISTORY *history, float phase, HD_CAMERA *camera)
{
    if (phase <= 0)
    {
        *camera = history->previous;
        return;
    }
    if (phase >= 1)
    {
        *camera = history->current;
        return;
    }
    *camera = history->current;
    hd_camera_view(camera, &history->pose, phase);
    camera->focal_y = history->previous.focal_y + (history->current.focal_y - history->previous.focal_y) * phase;
    camera->center_x = history->previous.center_x + (history->current.center_x - history->previous.center_x) * phase;
    camera->center_y = history->previous.center_y + (history->current.center_y - history->previous.center_y) * phase;
}

int hd_sphere_visible(const HD_CAMERA *camera, float aspect, const VECTORF *center, float radius)
{
    VECTORF pos;
    float x = camera->focal_y / aspect, y = camera->focal_y;
    float left = 1 + camera->center_x, right = 1 - camera->center_x;
    float top = 1 - camera->center_y, bottom = 1 + camera->center_y;
    RotTransf(&camera->view, center, &pos);
    if (pos.vz + radius < camera->near_depth || pos.vz - radius > camera->far_depth)
        return 0;
    if (x * pos.vx + left * pos.vz < -radius * sqrtf(x * x + left * left))
        return 0;
    if (-x * pos.vx + right * pos.vz < -radius * sqrtf(x * x + right * right))
        return 0;
    if (y * pos.vy + top * pos.vz < -radius * sqrtf(y * y + top * top))
        return 0;
    if (-y * pos.vy + bottom * pos.vz < -radius * sqrtf(y * y + bottom * bottom))
        return 0;
    return 1;
}

static void hd_matrix_rows(float rows[3][4], const MATRIXF *matrix)
{
    uint32 row, column;
    for (row = 0; row < 3; ++row)
    {
        for (column = 0; column < 3; ++column)
            rows[row][column] = matrix->m[row][column];
        rows[row][3] = matrix->t[row];
    }
}

int hd_init(void)
{
    static const char prefix[] = "#define HD_UI_TEXTURE\n";
    char ui_source[sizeof(prefix) + sizeof(hd_fragment_source)];
    if (hd_world_shader)
        return 1;
    hd_world_shader = vid_shader_create(hd_vertex_source, hd_fragment_source);
    memcpy(ui_source, prefix, sizeof(prefix) - 1);
    memcpy(ui_source + sizeof(prefix) - 1, hd_fragment_source, sizeof(hd_fragment_source));
    hd_ui_shader = vid_shader_create(hd_ui_vertex_source, ui_source);
    hd_image_shader = vid_shader_create(0, hd_image_fragment_source);
    hd_effect_shader = vid_shader_create(hd_effect_vertex_source, hd_fragment_source);
    hd_vram = vid_tex_create_fmt(1024, 512, VID_RGBA5551);
    hd_default_palette = vid_tex_create(1, 1);
    hd_default_faces = vid_tex_create_fmt(1, 1, VID_RGBA16UI);
    if (!hd_world_shader || !hd_ui_shader || !hd_image_shader || !hd_effect_shader || !hd_vram || !hd_default_palette || !hd_default_faces)
    {
        hd_free();
        return 0;
    }
    vid_tex_commit(hd_default_palette);
    vid_tex_commit(hd_default_faces);
    return 1;
}

void hd_reset(void)
{
    hd_logic_tick_valid = 0;
    ++hd_history_epoch;
    if (hd_active)
        vid_cancel();
    hd_active = 0;
    hd_collecting = 0;
    hd_ui_collect = 0;
    hd_ui_count = 0;
    hd_ui_mesh_valid = 0;
    hd_current_frame = 0;
    hd_vram_valid = 0;
    hd_scene_clear();
}

void hd_free(void)
{
    uint32 blend, texels, test, write;
    hd_reset();
#if defined(_DEBUG)
    for (blend = 0; blend < 9; ++blend)
    {
        vid_sample_free(hd_debug_probes[blend]);
        hd_debug_probes[blend] = 0;
    }
#endif
    for (blend = 0; blend < 5; ++blend)
    {
        vid_mat_free(hd_ui_materials[blend]);
        hd_ui_materials[blend] = 0;
        for (test = 0; test < 2; ++test)
        {
            vid_mat_free(hd_effect_materials[blend][test]);
            hd_effect_materials[blend][test] = 0;
            hd_effect_sprite_bindings[blend][test] = 0;
            hd_effect_line_bindings[blend][test] = 0;
        }
    }
    vid_shader_free(hd_effect_shader);
    vid_mesh_free(hd_effect_mesh);
    free(hd_effects);
    free(hd_ring_points);
    hd_ring_points = 0;
    hd_ring_capacity = 0;
    free(hd_beam_draw);
    hd_beam_draw = 0;
    hd_beam_capacity = 0;
    free(hd_ribbon_points);
    hd_ribbon_points = 0;
    hd_ribbon_capacity = 0;
    hd_effect_shader = 0;
    hd_effect_mesh = 0;
    hd_effects = 0;
    hd_effect_capacity = hd_effect_mesh_capacity = 0;
    hd_effect_upload_valid = 0;
    vid_tex_free(hd_sprite_texture);
    free(hd_sprites);
    hd_sprite_texture = 0;
    hd_sprites = 0;
    hd_sprite_count = hd_sprite_capacity = hd_sprite_texture_height = 0;
    vid_tex_free(hd_line_texture);
    free(hd_lines);
    hd_line_texture = 0;
    hd_lines = 0;
    hd_line_count = hd_line_capacity = hd_line_texture_height = 0;
    free(hd_line_staging);
    hd_line_staging = 0;
    hd_line_staging_capacity = 0;
    free(hd_trail_staging);
    hd_trail_staging = 0;
    hd_trail_staging_capacity = 0;
    free(hd_strips);
    hd_strips = 0;
    hd_strip_count = hd_strip_capacity = 0;
    free(hd_beams);
    hd_beams = 0;
    hd_beam_count = hd_beam_sample_capacity = 0;
    free(hd_ribbons);
    hd_ribbons = 0;
    hd_ribbon_count = hd_ribbon_sample_capacity = 0;
    free(hd_rings);
    hd_rings = 0;
    hd_ring_count = hd_ring_sample_capacity = 0;
    free(hd_polygons);
    hd_polygons = 0;
    hd_polygon_count = hd_polygon_capacity = 0;
    free(hd_polygon_staging);
    hd_polygon_staging = 0;
    hd_polygon_staging_capacity = 0;
    free(hd_model_effects);
    hd_model_effects = 0;
    hd_model_effect_count = hd_model_effect_capacity = 0;
    vid_shader_free(hd_ui_shader);
    vid_mat_free(hd_image_material);
    vid_shader_free(hd_image_shader);
    vid_mesh_free(hd_image_mesh);
    hd_image_material = 0;
    hd_image_shader = 0;
    hd_image_mesh = 0;
    hd_image_source_width = hd_image_source_height = 0;
    hd_image_drawable_width = hd_image_drawable_height = 0;
    vid_mesh_free(hd_ui_mesh);
    vid_index_free(hd_ui_indices);
    free(hd_ui_items);
    free(hd_ui_previous);
    free(hd_ui_text_staging);
    hd_ui_previous = hd_ui_text_staging = 0;
    hd_ui_previous_count = hd_ui_previous_capacity = 0;
    hd_ui_text_staging_count = hd_ui_text_staging_capacity = hd_ui_text_staging_cursor = 0;
    hd_ui_text_endpoint = 0;
    hd_ui_shader = 0;
    hd_ui_mesh = 0;
    hd_ui_indices = 0;
    hd_ui_items = 0;
    hd_ui_mesh_capacity = hd_ui_capacity = 0;
    free(hd_objects);
    free(hd_poses);
    free(hd_pose_history);
    free(hd_pose_scratch);
    free(hd_skeletal_samples);
    free(hd_alpha);
    vid_index_free(hd_scene_indices);
    hd_objects = 0;
    hd_poses = 0;
    hd_pose_history = 0;
    hd_pose_history_capacity = 0;
    hd_pose_scratch = 0;
    hd_pose_scratch_capacity = 0;
    hd_skeletal_samples = 0;
    hd_skeletal_count = hd_skeletal_capacity = 0;
    hd_alpha = 0;
    hd_scene_indices = 0;
    hd_object_capacity = hd_pose_capacity = hd_alpha_capacity = hd_scene_index_capacity = 0;
    hd_scene_index_count = 0;
    for (blend = 0; blend < 4; ++blend)
        for (texels = 0; texels < 3; ++texels)
            for (test = 0; test < 2; ++test)
                for (write = 0; write < 2; ++write)
                {
                    vid_mat_free(hd_materials[blend][texels][test][write]);
                    hd_materials[blend][texels][test][write] = 0;
                }
    vid_shader_free(hd_world_shader);
    vid_tex_free(hd_vram);
    vid_tex_free(hd_default_palette);
    vid_tex_free(hd_default_faces);
    hd_world_shader = 0;
    hd_vram = 0;
    hd_default_palette = 0;
    hd_default_faces = 0;
}

int hd_begin(const HD_CAMERA *camera, float r, float g, float b)
{
    sint32 width = 0, height = 0;
    float range = camera->far_depth - camera->near_depth;
    if (hd_active || range <= 0 || camera->near_depth <= 0 || !hd_init())
        return 0;
    vid_resize(&width, &height);
    if (width <= 0 || height <= 0 || !vid_begin(r, g, b))
        return 0;
    hd_active = 1;
    hd_draw_camera = *camera;
    hd_draw_aspect = (float)width / height;
#if defined(_DEBUG)
    hd_debug_scene_pending = 0;
#endif
    hd_ui_submitted = 0;
    hd_scene_submitted = 0;
    hd_matrix_rows(hd_draw + 3, &camera->view);
    hd_draw[6][0] = camera->focal_y * height / width;
    hd_draw[6][1] = camera->focal_y;
    hd_draw[6][2] = (camera->far_depth + camera->near_depth) / range;
    hd_draw[6][3] = -2 * camera->near_depth * camera->far_depth / range;
    hd_draw[7][2] = camera->center_x;
    hd_draw[7][3] = camera->center_y;
    hd_draw[8][0] = 0;
    hd_draw[8][1] = 0;
    hd_draw[18][2] = 0;
    hd_draw[18][3] = 0;
    memset(hd_draw + 28, 0, 4 * sizeof(hd_draw[0]));
    return 1;
}

static void hd_record_pass(VID_MATERIAL **slot, VID_TEXTURE *palette, VID_TEXTURE *faces, int bank, uint32 blend, uint32 texels, int depth_test, int depth_write, float alpha)
{
    VID_MATERIAL_DATA *data;
    if (!*slot)
    {
        *slot = vid_mat_create();
        data = vid_mat_data(*slot);
        data->shader = hd_world_shader;
        data->texture = hd_vram;
        data->aux[0] = palette;
        data->aux[1] = faces;
        data->blend = blend;
        data->depth_test = depth_test != 0;
        data->depth_write = depth_write != 0;
        vid_mat_commit(*slot);
    }
    hd_draw[7][0] = (float)texels;
    hd_draw[7][1] = alpha;
    hd_draw[8][0] = (float)bank;
    vid_uniforms(&hd_draw[0][0], 32);
    vid_pass(*slot);
}

void hd_pass(uint32 blend, uint32 texels, int depth_test, int depth_write, float alpha)
{
    if (!hd_active || blend > VID_BLEND_SUBTRACT || texels > HD_TEXELS_STP)
        return;
    hd_draw[18][3] = 0;
    hd_record_pass(&hd_materials[blend][texels][!!depth_test][!!depth_write], hd_default_palette, hd_default_faces, 0, blend, texels, depth_test, depth_write, alpha);
}

int hd_submit(VID_GEOMETRY *mesh, VID_INDEX *indices, uint32 first, uint32 count, const MATRIXF *model)
{
    if (!hd_active)
        return 0;
    hd_matrix_rows(hd_draw, model);
    vid_uniforms(&hd_draw[0][0], 32);
    return vid_submit(mesh, indices, first, count);
}

int hd_end(void)
{
    uint32 idx, version;
    uint16 *pixels;
    if (!hd_active)
        return 0;
    version = gpu_vram_version();
    if (!hd_vram_valid || version != hd_vram_version)
    {
        pixels = (uint16 *)vid_tex_data(hd_vram);
        for (idx = 0; idx < 1024 * 512; ++idx)
        {
            uint32 word = VRAM[idx];
            pixels[idx] = ((word & 31) << 11) | ((word & 992) << 1) | ((word & 31744) >> 9) | (word >> 15);
        }
        vid_tex_commit(hd_vram);
        hd_vram_version = version;
        hd_vram_valid = 1;
    }
    hd_active = 0;
    {
        int result = vid_end();
#if defined(_DEBUG)
        if (result && hd_debug_probe_enabled && hd_debug_scene_pending)
        {
            for (idx = 0; idx < 9; ++idx)
                hd_debug_colors[idx] = vid_sample_color(hd_debug_probes[idx]);
            ++hd_debug_frames;
        }
#endif
        return result;
    }
}

void hd_set_lighting(const HD_LIGHTING *lighting)
{
    uint32 row, column, shift, scale = 1;
    if (!lighting)
    {
        hd_draw[8][1] = 0;
        return;
    }
    hd_draw[8][1] = 1;
    hd_draw[8][2] = (float)(lighting->draw_mask >> 16);
    hd_draw[8][3] = (float)(lighting->draw_mask & 65535);
    for (row = 0; row < 3; ++row)
        for (column = 0; column < 3; ++column)
        {
            hd_draw[9 + row][column] = lighting->directions.m[row][column];
            hd_draw[12 + row][column] = lighting->colors.m[row][column];
        }
    hd_draw[15][0] = lighting->background.vx;
    hd_draw[15][1] = lighting->background.vy;
    hd_draw[15][2] = lighting->background.vz;
    hd_draw[16][0] = lighting->constant.vx;
    hd_draw[16][1] = lighting->constant.vy;
    hd_draw[16][2] = lighting->constant.vz;
    hd_draw[17][0] = lighting->far_color.vx;
    hd_draw[17][1] = lighting->far_color.vy;
    hd_draw[17][2] = lighting->far_color.vz;
    hd_draw[18][0] = lighting->fog_start;
    shift = (uint32)lighting->fog_shift;
    if (shift & 0x80000000)
    {
        scale <<= (0 - shift) & 31;
        hd_draw[18][1] = 1.0f / scale;
    }
    else
    {
        scale <<= shift & 31;
        hd_draw[18][1] = (float)scale;
    }
}

void hd_bank_free(HD_BANK *bank)
{
    uint32 blend, texels, test, write;
    if (!bank)
        return;
    hd_scene_clear();
    for (blend = 0; blend < bank->opaque_group_count; ++blend)
    {
        vid_mat_free(bank->opaque_groups[blend].material);
        vid_tex_free(bank->opaque_groups[blend].faces);
        vid_index_free(bank->opaque_groups[blend].indices);
    }
    free(bank->opaque_groups);
    free(bank->model_marks);
    vid_mesh_free(bank->mesh);
    vid_index_free(bank->topology);
    vid_index_free(bank->selection);
    for (blend = 0; blend < 4; ++blend)
        for (texels = 0; texels < 3; ++texels)
            for (test = 0; test < 2; ++test)
                for (write = 0; write < 2; ++write)
                    vid_mat_free(bank->materials[blend][texels][test][write]);
    vid_tex_free(bank->palette);
    vid_tex_free(bank->face_materials);
    free(bank->models);
    free(bank->faces);
    free(bank->batches);
    free(bank->references);
    free(bank);
}

HD_BANK *hd_bank_create(uint32 models, uint32 faces, uint32 vertices, uint32 indices)
{
    HD_BANK *bank;
    if (!models || !faces || !vertices || !indices || faces > 16777216 || models > (size_t)-1 / sizeof(HD_MODEL_RANGE) || faces > (size_t)-1 / sizeof(HD_POSE_BATCH))
        return 0;
    bank = (HD_BANK *)calloc(1, sizeof(HD_BANK));
    if (!bank)
        return 0;
    bank->identity = ++hd_bank_identity;
    bank->mesh = vid_mesh_create_16(vertices);
    bank->topology = vid_index_create(indices);
    bank->selection = vid_index_create(indices);
    bank->palette = vid_tex_create(256, 1);
    bank->face_materials = vid_tex_create_fmt(1024, (faces * 8 + 1023) / 1024, VID_RGBA16UI);
    bank->models = (HD_MODEL_RANGE *)calloc(models, sizeof(HD_MODEL_RANGE));
    bank->faces = (HD_FACE_RANGE *)calloc(faces, sizeof(HD_FACE_RANGE));
    bank->batches = (HD_POSE_BATCH *)calloc(faces, sizeof(HD_POSE_BATCH));
    bank->model_marks = (uint32 *)calloc(models, sizeof(uint32));
    if (!bank->mesh || !bank->topology || !bank->selection || !bank->palette || !bank->face_materials || !bank->models || !bank->faces || !bank->batches || !bank->model_marks)
    {
        hd_bank_free(bank);
        return 0;
    }
    bank->model_capacity = models;
    bank->face_capacity = faces;
    bank->vertex_capacity = vertices;
    bank->index_capacity = indices;
    bank->palette_changed = 1;
    bank->faces_changed = 1;
    return bank;
}

static uint32 hd_face_indices(uint32 *indices, uint32 first, uint32 vertices)
{
    indices[0] = first;
    indices[1] = first + 1;
    indices[2] = first + 2;
    if (vertices == 3)
        return 3;
    indices[3] = first + 1;
    indices[4] = first + 3;
    indices[5] = first + 2;
    return 6;
}

static int hd_resolve_model(HD_BANK *bank, uint32 model, const HD_SOURCE_MODEL *source, HD_SOURCE_REFERENCE **output)
{
    uint32 vertex;
    HD_SOURCE_REFERENCE *resolved;
    if (source->vertex_count > (size_t)-1 / sizeof(HD_SOURCE_REFERENCE))
        return 0;
    resolved = (HD_SOURCE_REFERENCE *)calloc(source->vertex_count, sizeof(HD_SOURCE_REFERENCE));
    if (source->vertex_count && !resolved)
        return 0;
    for (vertex = 0; vertex < source->vertex_count; ++vertex)
    {
        const SVECTOR *pos = source->vertices + vertex;
        HD_SOURCE_REFERENCE *value = resolved + vertex;
        value->pos = *pos;
        value->part = model;
        value->valid = 1;
        if (pos->pad & 2)
        {
            uint32 index = ((uint16)pos->vx | ((uint32)(uint16)pos->vy << 16)) >> 3;
            if (index < bank->reference_count)
                *value = bank->references[index];
            else
                value->valid = 0;
        }
        else if (pos->pad & 1)
        {
            if (bank->reference_count == bank->reference_capacity)
            {
                uint32 capacity = bank->reference_capacity ? bank->reference_capacity * 2 : 128;
                HD_SOURCE_REFERENCE *references;
                if (capacity < bank->reference_capacity || capacity > (size_t)-1 / sizeof(HD_SOURCE_REFERENCE))
                {
                    free(resolved);
                    return 0;
                }
                references = (HD_SOURCE_REFERENCE *)realloc(bank->references, capacity * sizeof(HD_SOURCE_REFERENCE));
                if (!references)
                {
                    free(resolved);
                    return 0;
                }
                bank->references = references;
                bank->reference_capacity = capacity;
            }
            bank->references[bank->reference_count++] = *value;
        }
    }
    *output = resolved;
    return 1;
}

static uint32 hd_pose_slot(HD_POSE_BATCH *batch, uint32 part)
{
    uint32 slot;
    for (slot = 0; slot < batch->part_count; ++slot)
        if (batch->parts[slot] == part)
            return slot;
    batch->parts[batch->part_count++] = part;
    return slot;
}

static uint32 hd_source_normal(const HD_SOURCE_MODEL *source, const HD_SOURCE_FACE *face, uint32 corner)
{
    return (source->flags & 1) || (face->flags & 12) == 12 ? face->vertex[corner] : face->normal;
}

int hd_bank_write_model(HD_BANK *bank, uint32 model, const HD_SOURCE_MODEL *source)
{
    uint32 face, corner, required_vertices = 0, required_indices = 0, reference_count;
    VID_VERTEX16 *vertices;
    uint32 *indices;
    HD_SOURCE_REFERENCE *resolved = 0;
    HD_POSE_BATCH *batch = 0;
    if (!bank || bank->sealed || model != bank->model_count || model >= bank->model_capacity || source->face_count > bank->face_capacity - bank->face_count)
        return 0;
    for (face = 0; face < source->face_count; ++face)
    {
        const HD_SOURCE_FACE *src = source->faces + face;
        uint32 count = src->flags & 16 ? 3 : 4;
        if (src->normal >= source->normal_count)
            return 0;
        if (required_vertices > bank->vertex_capacity - bank->vertex_count || count > bank->vertex_capacity - bank->vertex_count - required_vertices)
            return 0;
        required_vertices += count;
        if (required_indices > bank->index_capacity - bank->index_count || (count - 2) * 3 > bank->index_capacity - bank->index_count - required_indices)
            return 0;
        required_indices += (count - 2) * 3;
        for (corner = 0; corner < count; ++corner)
        {
            uint32 normal = hd_source_normal(source, src, corner);
            if (src->vertex[corner] >= source->vertex_count || normal >= source->normal_count)
                return 0;
        }
    }
    reference_count = bank->reference_count;
    if (!hd_resolve_model(bank, model, source, &resolved))
    {
        bank->reference_count = reference_count;
        return 0;
    }
    for (face = 0; face < source->face_count; ++face)
    {
        const HD_SOURCE_FACE *src = source->faces + face;
        uint32 count = src->flags & 16 ? 3 : 4;
        for (corner = 0; corner < count; ++corner)
            if (!resolved[src->vertex[corner]].valid)
            {
                bank->reference_count = reference_count;
                free(resolved);
                return 0;
            }
    }
    vertices = vid_mesh_data_16(bank->mesh);
    indices = vid_index_data(bank->topology);
    bank->models[model].first_face = bank->face_count;
    bank->models[model].face_count = source->face_count;
    bank->models[model].first = bank->index_count;
    bank->models[model].count = required_indices;
    bank->models[model].first_batch = bank->batch_count;
    for (face = 0; face < source->face_count; ++face)
    {
        const HD_SOURCE_FACE *src = source->faces + face;
        uint32 count = src->flags & 16 ? 3 : 4;
        uint32 parts[8], part_count = batch ? batch->part_count : 0, slots[4], i;
        HD_FACE_RANGE *range = bank->faces + bank->face_count++;
        if (batch)
            memcpy(parts, batch->parts, part_count * sizeof(uint32));
        for (corner = 0; corner < count; ++corner)
        {
            uint32 part = resolved[src->vertex[corner]].part;
            for (i = 0; i < part_count && parts[i] != part; ++i)
            {
            }
            if (i == part_count)
                parts[part_count++] = part;
        }
        if (!batch || part_count > 4)
        {
            batch = bank->batches + bank->batch_count++;
            batch->first = bank->index_count;
        }
        for (corner = 0; corner < count; ++corner)
            slots[corner] = hd_pose_slot(batch, resolved[src->vertex[corner]].part);
        range->first = bank->index_count;
        range->batch = bank->batch_count - 1;
        range->vertex = bank->vertex_count;
        range->count = count;
        hd_bank_set_face(bank, bank->face_count - 1, src->uv, src->tpage, src->clut, src->flags & 32 ? src->window : 0);
        hd_bank_set_region(bank, bank->face_count - 1, &src->texture_region);
        {
            uint16 *data = (uint16 *)vid_tex_data(bank->face_materials) + (bank->face_count - 1) * 32 + 8;
            const SVECTOR *normal = source->normals + src->normal;
            for (corner = 0; corner < 4; ++corner)
            {
                uint32 selected = corner < count ? corner : count - 1;
                const SVECTOR *pos = &resolved[src->vertex[selected]].pos;
                data[corner * 4] = pos->vx;
                data[corner * 4 + 1] = pos->vy;
                data[corner * 4 + 2] = pos->vz;
                data[corner * 4 + 3] = slots[selected];
            }
            data[16] = normal->vx;
            data[17] = normal->vy;
            data[18] = normal->vz;
            data[19] = src->flags;
        }
        for (corner = 0; corner < count; ++corner)
        {
            uint32 normal = hd_source_normal(source, src, corner);
            const SVECTOR *pos = &resolved[src->vertex[corner]].pos;
            const SVECTOR *n = source->normals + normal;
            VID_VERTEX16 *dst = vertices + bank->vertex_count++;
            memset(dst, 0, sizeof(*dst));
            dst->pos[0] = pos->vx;
            dst->pos[1] = pos->vy;
            dst->pos[2] = pos->vz;
            dst->pos[3] = slots[corner];
            dst->n[0] = n->vx;
            dst->n[1] = n->vy;
            dst->n[2] = n->vz;
            dst->n[3] = source->vertices[src->vertex[corner]].pad;
            dst->uv[0] = (src->uv[corner] & 255) * 2;
            dst->uv[1] = (src->uv[corner] >> 8) * 2;
            dst->color[0] = src->color;
            dst->color[1] = src->color >> 8;
            dst->color[2] = src->color >> 16;
            dst->color[3] = 255;
            dst->tex[0] = src->tpage;
            dst->tex[1] = src->clut;
            dst->tex[2] = src->flags & 3 ? HD_TEXTURED : 0;
            dst->tex[3] = src->flags & 2048 ? (src->color >> (corner * 8)) & 255 : 0;
            dst->rect[0] = ((src->window & 31) << 3) | ((src->window & 992) << 6);
            dst->rect[1] = ((src->window >> 7) & 248) | ((src->window >> 4) & 63488);
            dst->extra[0] = (float)(bank->batch_count - 1);
            dst->extra[1] = (float)src->vertex[corner];
            dst->extra[2] = (float)(bank->face_count - 1);
            dst->extra[3] = (float)corner;
        }
        i = hd_face_indices(indices + bank->index_count, range->vertex, count);
        bank->index_count += i;
        batch->count += i;
    }
    bank->models[model].batch_count = bank->batch_count - bank->models[model].first_batch;
    free(resolved);
    ++bank->model_count;
    return 1;
}

int hd_bank_seal(HD_BANK *bank)
{
    if (!bank || bank->sealed || bank->model_count != bank->model_capacity || bank->face_count != bank->face_capacity || bank->vertex_count != bank->vertex_capacity || bank->index_count != bank->index_capacity)
        return 0;
    vid_mesh_commit(bank->mesh);
    vid_index_commit(bank->topology, bank->index_count);
    vid_index_static(bank->topology);
    hd_bank_commit_materials(bank);
    free(bank->references);
    bank->references = 0;
    bank->reference_count = 0;
    bank->reference_capacity = 0;
    bank->sealed = 1;
    return 1;
}

int hd_bank_submit(HD_BANK *bank, uint32 model, const MATRIXF *matrix)
{
    if (!bank || !bank->sealed || model >= bank->model_count)
        return 0;
    return hd_submit(bank->mesh, bank->topology, bank->models[model].first, bank->models[model].count, matrix);
}

int hd_bank_submit_part(HD_BANK *bank, uint32 part, const MATRIXF *poses, uint32 count)
{
    HD_MODEL_RANGE *range;
    uint32 i, slot;
    int result = 1;
    if (!bank || !bank->sealed || !hd_active || part >= bank->model_count)
        return 0;
    range = bank->models + part;
    for (i = 0; i < range->batch_count; ++i)
    {
        const HD_POSE_BATCH *batch = bank->batches + range->first_batch + i;
        for (slot = 0; slot < batch->part_count; ++slot)
            if (batch->parts[slot] >= count)
                return 0;
    }
    hd_draw[18][2] = 1;
    for (i = 0; i < range->batch_count; ++i)
    {
        const HD_POSE_BATCH *batch = bank->batches + range->first_batch + i;
        for (slot = 1; slot < batch->part_count; ++slot)
            hd_matrix_rows(hd_draw + 19 + (slot - 1) * 3, poses + batch->parts[slot]);
        if (!hd_submit(bank->mesh, bank->topology, batch->first, batch->count, poses + batch->parts[0]))
            result = 0;
    }
    hd_draw[18][2] = 0;
    return result;
}

int hd_bank_select(HD_BANK *bank, const uint32 *faces, uint32 count)
{
    uint32 face, used = 0, changed, *indices;
    if (!bank || !bank->sealed || count > bank->face_count || hd_active)
        return 0;
    for (face = 0; face < count; ++face)
    {
        uint32 n;
        if (faces[face] >= bank->face_count)
            return 0;
        n = (bank->faces[faces[face]].count - 2) * 3;
        if (n > bank->index_capacity - used)
            return 0;
        used += n;
    }
    indices = vid_index_data(bank->selection);
    used = 0;
    changed = 0;
    for (face = 0; face < count; ++face)
    {
        uint32 values[6], n, i;
        const HD_FACE_RANGE *range;
        range = bank->faces + faces[face];
        n = hd_face_indices(values, range->vertex, range->count);
        for (i = 0; i < n; ++i)
        {
            if (indices[used] != values[i])
                changed = 1;
            indices[used++] = values[i];
        }
    }
    if (changed || used != bank->selection_count)
        vid_index_commit(bank->selection, used);
    bank->selection_count = used;
    return 1;
}

int hd_bank_submit_selected(HD_BANK *bank, const MATRIXF *matrix)
{
    if (!bank || !bank->sealed)
        return 0;
    return hd_submit(bank->mesh, bank->selection, 0, bank->selection_count, matrix);
}

void hd_bank_set_palette(HD_BANK *bank, const uint32 *colors, uint32 count)
{
    uint8 *pixels = vid_tex_data(bank->palette);
    uint32 color;
    if (count > 256)
        count = 256;
    for (color = 0; color < 256; ++color)
    {
        uint32 value = color < count ? colors[color] : 0;
        uint8 rgba[4];
        rgba[0] = value;
        rgba[1] = value >> 8;
        rgba[2] = value >> 16;
        rgba[3] = 255;
        if (memcmp(pixels + color * 4, rgba, 4))
        {
            memcpy(pixels + color * 4, rgba, 4);
            bank->palette_changed = 1;
        }
    }
}

void hd_bank_set_face(HD_BANK *bank, uint32 face, const uint16 uv[4], uint16 page, uint16 clut, uint32 window)
{
    uint16 values[8], *pixels;
    if (face >= bank->face_capacity)
        return;
    memcpy(values, uv, 4 * sizeof(uint16));
    values[4] = page;
    values[5] = clut;
    values[6] = window;
    values[7] = window >> 16;
    pixels = (uint16 *)vid_tex_data(bank->face_materials) + face * 32;
    if (memcmp(pixels, values, sizeof(values)))
    {
        memcpy(pixels, values, sizeof(values));
        bank->faces_changed = 1;
    }
}

void hd_bank_set_region(HD_BANK *bank, uint32 face, const TextureRegion *region)
{
    uint16 values[4] = {0}, *pixels;
    uint32 x, y, width, height;
    if (face >= bank->face_count)
        return;
    pixels = (uint16 *)vid_tex_data(bank->face_materials) + face * 32;
    x = region->x;
    y = region->y;
    width = region->width;
    height = region->height;
    if (width && height && x + width <= 256 && y + height <= 256)
    {
#ifdef _DEBUG
        ++hd_debug_region_attempts;
#endif
        values[0] = x;
        values[1] = y;
        values[2] = width;
        values[3] = height;
    }
    if (memcmp(pixels + 28, values, sizeof(values)))
    {
        memcpy(pixels + 28, values, sizeof(values));
        bank->faces_changed = 1;
    }
}

void hd_bank_commit_materials(HD_BANK *bank)
{
    if (bank->palette_changed)
    {
        vid_tex_commit(bank->palette);
        bank->palette_changed = 0;
    }
    if (bank->faces_changed)
    {
        vid_tex_commit(bank->face_materials);
        bank->faces_changed = 0;
        ++bank->face_revision;
    }
}

void hd_bank_set_flags(HD_BANK *bank, uint32 face, uint16 flags)
{
    uint16 *data;
    if (face >= bank->face_count)
        return;
    data = (uint16 *)vid_tex_data(bank->face_materials) + face * 32 + 27;
    if (*data != flags)
    {
        *data = flags;
        bank->faces_changed = 1;
    }
}

void hd_bank_pass(HD_BANK *bank, uint32 blend, uint32 texels, int depth_test, int depth_write, float alpha)
{
    if (!bank || !bank->sealed || !hd_active || blend > VID_BLEND_SUBTRACT || texels > HD_TEXELS_STP)
        return;
    hd_draw[18][3] = 0;
    hd_record_pass(&bank->materials[blend][texels][!!depth_test][!!depth_write], bank->palette, bank->face_materials, 1, blend, texels, depth_test, depth_write, alpha);
}

void hd_bank_scene_pass(HD_BANK *bank, uint32 pass)
{
    static const uint32 blend[5] = {VID_BLEND_NONE, VID_BLEND_ALPHA, VID_BLEND_ADD, VID_BLEND_SUBTRACT, VID_BLEND_ADD};
    static const float alpha[5] = {1, 0.5f, 1, 1, 0.25f};
    uint32 write = pass == 0;
    if (!bank || !bank->sealed || !hd_active || pass >= 5)
        return;
    hd_draw[18][3] = (float)(pass + 1);
    hd_record_pass(&bank->materials[blend[pass]][HD_TEXELS_ALL][1][write], bank->palette, bank->face_materials, 1, blend[pass], HD_TEXELS_ALL, 1, write, alpha[pass]);
}

void hd_scene_clear(void)
{
    hd_beam_count = 0;
    hd_ribbon_count = 0;
    hd_ring_count = 0;
    hd_polygon_count = 0;
    hd_model_effect_count = 0;
    hd_strip_count = 0;
    hd_line_count = 0;
    hd_sprite_count = 0;
    hd_render_phase = 1;
    ++hd_effect_version;
    ++hd_scene_revision;
    hd_object_count = 0;
    hd_pose_count = 0;
    hd_skeletal_count = 0;
    hd_alpha_count = 0;
    hd_effect_count = 0;
    hd_capture_order = 0;
}

static int hd_reserve(void **data, uint32 *capacity, uint32 required, size_t stride)
{
    uint32 next = *capacity ? *capacity : 64;
    void *replacement;
    if (required <= *capacity)
        return 1;
    while (next < required)
    {
        if (next > 0x7FFFFFFF)
        {
            next = required;
            break;
        }
        next *= 2;
    }
    if (next > (size_t)-1 / stride)
        return 0;
    replacement = realloc(*data, next * stride);
    if (!replacement)
        return 0;
    *data = replacement;
    *capacity = next;
    return 1;
}

void hd_effect_point(VECTORF *point, float x, float y, float z)
{
    VECTORF world = {x, y, z};
    RotTransf(&hd_capture_camera.view, &world, point);
}

void hd_effect_offset(VECTORF *point, const VECTORF *center, float x, float y)
{
    float scale = center->vz / (hd_capture_camera.focal_y * 120);
    point->vx = center->vx + x * scale * hd_effect_pixel_aspect;
    point->vy = center->vy + y * scale;
    point->vz = center->vz;
}

int hd_effect_sort(HD_EFFECT *effect, const VECTORF *points, uint32 count, uint32 rule, sint32 offset, int clamp)
{
    if (!count || count > 6 || !points || rule < HD_DEPTH_MIN || rule > HD_DEPTH_MEAN)
        return 0;
    memcpy(effect->depth.points, points, count * sizeof(*points));
    effect->depth.count = count;
    effect->depth.rule = rule;
    effect->depth.offset = offset;
    effect->depth.clamp = clamp;
    return 1;
}

int hd_effect_add(const HD_EFFECT *effect, float depth, uint32 bias, uint32 forced)
{
    HD_EFFECT_ITEM *item;
    uint32 corner;
    if (effect->beam > hd_beam_count || (effect->beam && effect->beam_layer > 2))
        return 0;
    if (effect->ribbon > hd_ribbon_count || (effect->ribbon && effect->ribbon_layer > 3))
        return 0;
    if (effect->ring > hd_ring_count)
        return 0;
    if (effect->polygon > hd_polygon_count)
        return 0;
    if (effect->model_effect > hd_model_effect_count)
        return 0;
    if (effect->count < 3 || effect->count > 4 || effect->sprite > hd_sprite_count || effect->line > hd_line_count || effect->strip > hd_strip_count || effect->depth.count > 6 || (effect->depth.count && (effect->depth.rule < HD_DEPTH_MIN || effect->depth.rule > HD_DEPTH_MEAN)) || effect->space > HD_EFFECT_BILLBOARD || forced > 4095 || hd_effect_count == 0xFFFFFFFF)
        return 0;
    if (effect->cull_positive)
    {
        VECTORF points[3];
        const VECTORF *a, *b, *c;
        for (corner = 0; corner < 3; ++corner)
        {
            points[corner] = effect->vertices[corner].pos;
            if (effect->space != HD_EFFECT_CAMERA)
            {
                RotTransf(&hd_capture_camera.view, &points[corner], &points[corner]);
                if (effect->space == HD_EFFECT_BILLBOARD)
                {
                    points[corner].vx += effect->offsets[corner].vx / hd_capture_camera.focal_y;
                    points[corner].vy += effect->offsets[corner].vy / hd_capture_camera.focal_y;
                    points[corner].vz += effect->offsets[corner].vz / hd_capture_camera.focal_y;
                }
            }
        }
        a = points; b = points + 1; c = points + 2;
        float area = a->vx * (b->vy * c->vz - b->vz * c->vy) + a->vy * (b->vz * c->vx - b->vx * c->vz) + a->vz * (b->vx * c->vy - b->vy * c->vx);
        if (area > 0)
            return 1;
    }
    if (!hd_reserve((void **)&hd_effects, &hd_effect_capacity, hd_effect_count + 1, sizeof(*hd_effects)))
        return 0;
    item = hd_effects + hd_effect_count++;
    item->effect = *effect;
    if (effect->space == HD_EFFECT_CAMERA)
    {
        for (corner = 0; corner < effect->count; ++corner)
            RotTransf(&hd_capture_inverse, &effect->vertices[corner].pos, &item->effect.vertices[corner].pos);
        for (corner = 0; corner < effect->depth.count; ++corner)
            RotTransf(&hd_capture_inverse, effect->depth.points + corner, item->effect.depth.points + corner);
        item->effect.space = HD_EFFECT_WORLD;
    }
    item->effect.forced_depth = forced;
    item->bias = bias;
    item->order = hd_capture_order++;
    item->bucket = forced ? forced : (((uint32)(sint32)depth - bias) & 0x3FFC) >> 2;
    ++hd_effect_version;
    return 1;
}

HD_RING_POINT *hd_effect_ring_points(uint32 count)
{
    return hd_reserve((void **)&hd_ring_points, &hd_ring_capacity, count, sizeof(*hd_ring_points)) ? hd_ring_points : 0;
}

static uint32 hd_effect_fade(uint32 color, uint32 fade)
{
    return (color & 0xFF000000) | (((color & 255) * fade) >> 8) | (((((color >> 8) & 255) * fade) >> 8) << 8) | (((((color >> 16) & 255) * fade) >> 8) << 16);
}

static VECTORF hd_effect_ring_point(const VECTORF *center, float radius, float angle)
{
    VECTORF point = *center;
    float distance = hd_capture_camera.focal_y * 120;
    point.vx += radius * sinf(angle) * 625 * hd_effect_pixel_aspect / distance;
    point.vy -= radius * cosf(angle) * 400 / distance;
    return point;
}

int hd_effect_ring(const HD_RING *ring)
{
    uint32 ray, fade;
    float z = ring->center.vz;
    if (z <= 0 || z > 20000 || !ring->count)
        return 1;
    fade = z < 700 ? (uint32)(255 * z / 700) + 1 : 256;
    for (ray = 0; ray < ring->count; ++ray)
    {
        HD_EFFECT effect = {0};
        uint32 previous = ray ? ray - 1 : ring->count - 1, row;
        uint32 r0 = ring->points[previous].distance, r1 = ring->points[ray].distance;
        float a0 = ray ? ring->angle + ray * ring->step : ring->angle;
        float a1 = ray + 1 == ring->count ? ring->angle : ring->angle + (ray + 1) * ring->step;
        VECTORF inner[2];
        uint32 inner_color = ring->points[ray].color;
        if (!((ring->mask >> (ray & 31)) & 1))
            continue;
        inner[0] = hd_effect_ring_point(&ring->center, (float)r0, a0);
        inner[1] = hd_effect_ring_point(&ring->center, (float)r1, a1);
        effect.page = ring->page;
        if (!hd_effect_sort(&effect, &ring->center, 1, HD_DEPTH_MIN, 0, 0))
            return 0;
        if (!ring->hide_center)
        {
            effect.count = 3;
            effect.flags = ring->color & 0x2000000 ? HD_SEMITRANSPARENT : 0;
            effect.vertices[0].pos = ring->center;
            effect.vertices[1].pos = inner[0];
            effect.vertices[2].pos = inner[1];
            effect.vertices[0].color = hd_effect_fade(ring->color, fade);
            effect.vertices[1].color = effect.vertices[2].color = hd_effect_fade(inner_color, fade);
            if (!hd_effect_add(&effect, z, 0, 0))
                return 0;
        }
        for (row = 0; row < ring->rows; ++row)
        {
            const HD_RING_POINT *points = ring->points + (row + 1) * ring->count;
            uint32 outer_color = points[ray].color;
            r0 += points[previous].distance;
            r1 += points[ray].distance;
            effect.count = 4;
            effect.flags = outer_color & 0x2000000 ? HD_SEMITRANSPARENT : 0;
            effect.vertices[0].pos = hd_effect_ring_point(&ring->center, (float)(sint32)r0, a0);
            effect.vertices[1].pos = hd_effect_ring_point(&ring->center, (float)(sint32)r1, a1);
            effect.vertices[2].pos = inner[0];
            effect.vertices[3].pos = inner[1];
            effect.vertices[0].color = effect.vertices[1].color = hd_effect_fade(outer_color, fade);
            effect.vertices[2].color = effect.vertices[3].color = hd_effect_fade(inner_color, fade);
            if (!hd_effect_add(&effect, z, 0, 0))
                return 0;
            inner[0] = effect.vertices[0].pos;
            inner[1] = effect.vertices[1].pos;
            inner_color = outer_color;
        }
    }
    return 1;
}

void hd_ring_reset(HD_RING_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_ring_free(HD_RING_HISTORY *history)
{
    if (history)
    {
        free(history->previous_points);
        free(history->current_points);
        free(history);
    }
}

static void hd_ring_sample(HD_RING_PRIMITIVE *sample, const HD_RING *ring, uint32 ray, uint32 row, const uint32 radii[2], const float inner[2])
{
    uint32 color = ring->points[row * ring->count + ray].color;
    uint32 inner_color = row ? ring->points[(row - 1) * ring->count + ray].color : ring->color;
    sample->center = ring->center;
    sample->attachment = ring->attachment;
    sample->page = ring->page;
    sample->angles[0] = ray ? ring->angle + ray * ring->step : ring->angle;
    sample->angles[1] = ray + 1 == ring->count ? ring->angle : ring->angle + (ray + 1) * ring->step;
    sample->radii[0] = (float)(sint32)radii[0];
    sample->radii[1] = (float)(sint32)radii[1];
    sample->radii[2] = inner[0];
    sample->radii[3] = inner[1];
    sample->colors[0] = row ? color : ring->color;
    sample->colors[1] = row ? color : ring->points[ray].color;
    sample->colors[2] = row ? inner_color : ring->points[ray].color;
    sample->colors[3] = inner_color;
    sample->flags = (row ? color : ring->color) & 0x2000000 ? HD_SEMITRANSPARENT : 0;
    sample->visible = ((ring->mask >> (ray & 31)) & 1) && (row || !ring->hide_center);
}

static int hd_ring_record(HD_RING_HISTORY **owner, const HD_RING *ring, int logic)
{
    HD_RING_HISTORY *history = *owner;
    uint64 total = (uint64)ring->count * ((uint64)ring->rows + 1);
    uint32 ray, row, count;
    int step;
    if (!ring->count)
    {
        hd_ring_reset(history);
        return 1;
    }
    if (hd_active || !ring->points || total > (size_t)-1 / sizeof(*ring->points) || total > 16777215 - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count - hd_polygon_count - hd_model_effect_count)
        return 0;
    if (!logic && history && history->initialized && history->epoch == hd_history_epoch && history->serial == hd_capture_serial && (!history->clock.valid || (history->clock.published && history->clock.published_tick == hd_logic_tick)))
        return 1;
    count = (uint32)total;
    if (!history || history->count != ring->count || history->rows != ring->rows)
    {
        HD_RING_HISTORY *replacement = (HD_RING_HISTORY *)calloc(1, sizeof(*replacement));
        if (!replacement)
            return 0;
        replacement->previous_points = (HD_RING_POINT *)malloc(count * sizeof(*ring->points));
        replacement->current_points = (HD_RING_POINT *)malloc(count * sizeof(*ring->points));
        if (!replacement->previous_points || !replacement->current_points)
        {
            hd_ring_free(replacement);
            return 0;
        }
        replacement->count = ring->count;
        replacement->rows = ring->rows;
        hd_ring_free(history);
        *owner = history = replacement;
    }
    step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, 0, logic);
    if (step < 0) return 0;
    if (step == HD_SAMPLE_SEED || step == HD_SAMPLE_ADVANCE)
    {
        history->previous = step == HD_SAMPLE_SEED ? *ring : history->current;
        memcpy(history->previous_points, step == HD_SAMPLE_SEED ? ring->points : history->current_points, count * sizeof(*ring->points));
        memcpy(history->current_points, ring->points, count * sizeof(*ring->points));
        history->current = *ring;
        history->previous.points = history->previous_points;
        history->current.points = history->current_points;
    }
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic) return 1;
    ring = &history->current;
    if (!hd_reserve((void **)&hd_rings, &hd_ring_sample_capacity, hd_ring_count + count, sizeof(*hd_rings)))
        return 0;
    for (ray = 0; ray < ring->count; ++ray)
    {
        uint32 previous = ray ? ray - 1 : ring->count - 1;
        uint32 radii[2][2] = {{history->previous_points[previous].distance, history->previous_points[ray].distance}, {history->current_points[previous].distance, history->current_points[ray].distance}};
        float inner[2][2] = {{(float)radii[0][0], (float)radii[0][1]}, {(float)radii[1][0], (float)radii[1][1]}};
        for (row = 0; row <= ring->rows; ++row)
        {
            HD_RING_PAIR *pair = hd_rings + hd_ring_count;
            HD_EFFECT effect = {0};
            VECTORF center;
            if (row)
            {
                radii[0][0] += history->previous_points[row * ring->count + previous].distance;
                radii[0][1] += history->previous_points[row * ring->count + ray].distance;
                radii[1][0] += history->current_points[row * ring->count + previous].distance;
                radii[1][1] += history->current_points[row * ring->count + ray].distance;
            }
            hd_ring_sample(&pair->previous, &history->previous, ray, row, radii[0], inner[0]);
            hd_ring_sample(&pair->current, &history->current, ray, row, radii[1], inner[1]);
            pair->count = row ? 4 : 3;
            effect.count = pair->count;
            effect.page = ring->page;
            effect.flags = pair->current.flags;
            effect.space = HD_EFFECT_WORLD;
            effect.ring = ++hd_ring_count;
            RotTransf(&hd_capture_camera.view, &ring->center, &center);
            if (!hd_effect_add(&effect, center.vz, 0, 0))
                return 0;
            if (row)
            {
                inner[0][0] = (float)(sint32)radii[0][0];
                inner[0][1] = (float)(sint32)radii[0][1];
                inner[1][0] = (float)(sint32)radii[1][0];
                inner[1][1] = (float)(sint32)radii[1][1];
            }
        }
    }
    history->serial = hd_capture_serial;
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    return 1;
}

int hd_ring_capture(HD_RING_HISTORY **owner, const HD_RING *ring)
{
    return hd_ring_record(owner, ring, 0);
}

int hd_ring_tick(HD_RING_HISTORY **owner, const HD_RING *ring)
{
    return hd_ring_record(owner, ring, 1);
}

HD_POLYGON_SAMPLE *hd_polygon_samples(uint32 count)
{
    return hd_reserve((void **)&hd_polygon_staging, &hd_polygon_staging_capacity, count, sizeof(*hd_polygon_staging)) ? hd_polygon_staging : 0;
}

void hd_polygon_chain_reset(HD_POLYGON_CHAIN *history)
{
    if (history)
        history->initialized = 0;
}

void hd_polygon_chain_free(HD_POLYGON_CHAIN *history)
{
    if (history)
    {
        free(history->previous);
        free(history->current);
        free(history);
    }
}

static float hd_polygon_depth(const HD_POLYGON_PAIR *pair, float phase, const MATRIXF *view)
{
    const HD_EFFECT *a = &pair->previous.effect, *b = &pair->current.effect;
    const HD_EFFECT_DEPTH *sort = phase <= 0 ? &a->depth : &b->depth;
    uint32 index, count = sort->count ? sort->count : b->count;
    uint32 rule = sort->count ? sort->rule : HD_DEPTH_MIN;
    float depth = rule == HD_DEPTH_MIN ? 1.0e30f : (rule == HD_DEPTH_MAX ? -1.0e30f : 0);
    for (index = 0; index < count; ++index)
    {
        const VECTORF *p = sort->count ? a->depth.points + index : &a->vertices[index].pos;
        const VECTORF *q = sort->count ? b->depth.points + index : &b->vertices[index].pos;
        VECTORF world = {p->vx + (q->vx - p->vx) * phase, p->vy + (q->vy - p->vy) * phase, p->vz + (q->vz - p->vz) * phase};
        float z = view->m[2][0] * world.vx + view->m[2][1] * world.vy + view->m[2][2] * world.vz + view->t[2];
        if (sort->clamp)
            z = z < 0 ? 0 : (z > 65535 ? 65535 : z);
        if (rule == HD_DEPTH_MEAN) depth += z;
        else if ((rule == HD_DEPTH_MIN && z < depth) || (rule == HD_DEPTH_MAX && z > depth)) depth = z;
    }
    if (rule == HD_DEPTH_MEAN) depth /= count;
    depth += sort->offset;
    if (sort->clamp && (depth < 0 || depth > 16380)) depth = 16380;
    return depth;
}

static int hd_polygon_chain_record(HD_POLYGON_CHAIN **owner, const HD_POLYGON_SAMPLE *samples, uint32 count, int logic)
{
    HD_POLYGON_CHAIN *history = *owner;
    uint32 index;
    int step, changed = 0;
    if (!count) { hd_polygon_chain_reset(history); return 1; }
    if (hd_active || !samples || count > (size_t)-1 / sizeof(*samples) || count > 16777215 - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count - hd_polygon_count - hd_model_effect_count)
        return 0;
    if (!logic && history && history->initialized && history->epoch == hd_history_epoch && history->serial == hd_capture_serial && (!history->clock.valid || (history->clock.published && history->clock.published_tick == hd_logic_tick)))
        return 1;
    for (index = 0; index < count; ++index)
    {
        const HD_EFFECT *effect = &samples[index].effect;
        if (effect->space != HD_EFFECT_WORLD || effect->count < 3 || effect->count > 4 || effect->depth.count > 6 || effect->forced_depth > 4095 || samples[index].uv_period > 256 || effect->sprite || effect->line || effect->strip || effect->beam || effect->ribbon || effect->ring || effect->polygon || effect->model_effect)
            return 0;
    }
    if (!history || history->count != count)
    {
        HD_POLYGON_CHAIN *replacement = (HD_POLYGON_CHAIN *)calloc(1, sizeof(*replacement));
        if (!replacement) return 0;
        replacement->previous = (HD_POLYGON_SAMPLE *)malloc(count * sizeof(*samples));
        replacement->current = (HD_POLYGON_SAMPLE *)malloc(count * sizeof(*samples));
        if (!replacement->previous || !replacement->current) { hd_polygon_chain_free(replacement); return 0; }
        replacement->count = count;
        hd_polygon_chain_free(history);
        *owner = history = replacement;
    }
    if (history->initialized)
        for (index = 0; index < count; ++index)
            changed |= samples[index].effect.count != history->current[index].effect.count || samples[index].effect.depth.count != history->current[index].effect.depth.count;
    step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, changed, logic);
    if (step < 0) return 0;
    if (step == HD_SAMPLE_SEED || step == HD_SAMPLE_ADVANCE)
    {
        memcpy(history->previous, step == HD_SAMPLE_SEED ? samples : history->current, count * sizeof(*samples));
        memcpy(history->current, samples, count * sizeof(*samples));
    }
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic) return 1;
    samples = history->current;
    if (!hd_reserve((void **)&hd_polygons, &hd_polygon_capacity, hd_polygon_count + count, sizeof(*hd_polygons))) return 0;
    for (index = 0; index < count; ++index)
    {
        HD_POLYGON_PAIR *pair = hd_polygons + hd_polygon_count;
        HD_EFFECT effect = samples[index].effect;
        pair->previous = history->previous[index];
        pair->current = history->current[index];
        effect.polygon = ++hd_polygon_count;
        effect.cull_positive = 0;
        if (!pair->previous.present && !pair->current.present) continue;
        if (!hd_effect_add(&effect, hd_polygon_depth(pair, 1, &hd_capture_camera.view), samples[index].bias, effect.forced_depth)) return 0;
    }
    history->serial = hd_capture_serial;
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    return 1;
}

int hd_polygon_chain_capture(HD_POLYGON_CHAIN **owner, const HD_POLYGON_SAMPLE *samples, uint32 count)
{
    return hd_polygon_chain_record(owner, samples, count, 0);
}

int hd_polygon_chain_tick(HD_POLYGON_CHAIN **owner, const HD_POLYGON_SAMPLE *samples, uint32 count)
{
    return hd_polygon_chain_record(owner, samples, count, 1);
}

int hd_effect_strip(HD_STRIP *strip, const HD_EFFECT *source, float size, float frame_width, uint32 flags, uint32 bias, float near_depth, float far_depth)
{
    HD_EFFECT effect = *source;
    VECTORF a = source->vertices[0].pos, b = source->vertices[1].pos, start[2], end[2];
    float width_a, width_b, dx, dy, length, distance = hd_capture_camera.focal_y * 120;
    uint32 corner;
    int new_start = (flags & 0x10) || !strip->connected;
    if (!new_start)
        a = strip->center;
    strip->connected = 0;
    if (a.vz < near_depth || a.vz > far_depth || b.vz < near_depth || b.vz > far_depth)
        return 1;
    dx = (b.vx / b.vz - a.vx / a.vz) / hd_effect_pixel_aspect;
    dy = b.vy / b.vz - a.vy / a.vz;
    length = sqrtf(dx * dx + dy * dy);
    if (length <= 0)
        return 1;
    dx /= length;
    dy /= length;
    width_a = size * frame_width / (2 * a.vz);
    width_b = size * frame_width / (2 * b.vz);
    if (width_a < 2) width_a = 2;
    if (width_b < 2) width_b = 2;
    if (distance <= 0)
        return 0;
    if (new_start)
    {
        hd_effect_offset(start, &a, dy * width_a, -dx * width_a);
        hd_effect_offset(start + 1, &a, -dy * width_a, dx * width_a);
    }
    else
    {
        start[0] = strip->edges[0];
        start[1] = strip->edges[1];
    }
    if (flags & 0x10)
    {
        strip->saved[0] = start[0];
        strip->saved[1] = start[1];
    }
    if (flags & 0x20)
    {
        end[0] = strip->saved[0];
        end[1] = strip->saved[1];
    }
    else
    {
        hd_effect_offset(end, &b, dy * width_b, -dx * width_b);
        hd_effect_offset(end + 1, &b, -dy * width_b, dx * width_b);
    }
    effect.count = 4;
    for (corner = 0; corner < 4; ++corner)
    {
        uint32 edge = (corner >> (flags & 2 ? 0 : 1)) & 1;
        int endpoint = ((corner >> (flags & 2 ? 1 : 0)) & 1) != 0;
        if (!!(flags & 1) != !!(flags & 2)) edge = 1 - edge;
        if (flags & 2) endpoint = !endpoint;
        effect.vertices[corner].pos = endpoint ? end[edge] : start[edge];
        if (corner & 1) effect.vertices[corner].u = (uint8)((sint32)effect.vertices[corner].u - 1);
        if (corner & 2) effect.vertices[corner].v = (uint8)((sint32)effect.vertices[corner].v - 1);
    }
    strip->center = b;
    strip->edges[0] = end[0];
    strip->edges[1] = end[1];
    strip->connected = 1;
    if (!hd_effect_sort(&effect, &b, 1, HD_DEPTH_MIN, 0, 0))
        return 0;
    return hd_effect_add(&effect, b.vz, bias, 0);
}

static VECTORF hd_effect_project(const VECTORF *point)
{
    float distance = hd_capture_camera.focal_y * 120;
    VECTORF screen = {256 + hd_capture_camera.center_x * 256, 120 - hd_capture_camera.center_y * 120, point->vz};
    if (point->vz != 0)
    {
        screen.vx += distance * point->vx / (point->vz * hd_effect_pixel_aspect);
        screen.vy += distance * point->vy / point->vz;
    }
    return screen;
}

static VECTORF hd_effect_unproject(const VECTORF *screen, float depth)
{
    float scale = depth / (hd_capture_camera.focal_y * 120);
    VECTORF point = {(screen->vx - 256 - hd_capture_camera.center_x * 256) * scale * hd_effect_pixel_aspect, (screen->vy - 120 + hd_capture_camera.center_y * 120) * scale, depth};
    return point;
}

static void hd_effect_normal(const VECTORF *a, const VECTORF *b, float minimum, float *nx, float *ny)
{
    float dx = (b->vx - a->vx) * 320 / 512, dy = b->vy - a->vy;
    float x = fabsf(dx), y = fabsf(dy), norm = x > y ? x + y / 2 : y + x / 2;
    *nx = *ny = 0;
    if (norm >= minimum && norm > 0)
    {
        *nx = dy * 512 / (norm * 320);
        *ny = -dx / norm;
    }
}

int hd_effect_beam(HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page, float near_depth, float far_depth)
{
    uint32 index;
    float nx = 0, ny = 0;
    if (count < 2 || !hd_reserve((void **)&hd_beam_draw, &hd_beam_capacity, count, sizeof(*hd_beam_draw)))
        return 0;
    for (index = 0; index < count; ++index)
    {
        HD_BEAM_DRAW *draw = hd_beam_draw + index;
        draw->screen = hd_effect_project(&points[index].point);
        draw->valid = points[index].point.vz >= near_depth && points[index].point.vz <= far_depth;
    }
    for (index = 0; index + 1 < count; ++index)
    {
        float x = 0, y = 0;
        HD_BEAM_DRAW *draw = hd_beam_draw + index;
        if (draw->valid && draw[1].valid)
            hd_effect_normal(&draw->screen, &draw[1].screen, 2, &x, &y);
        draw->nx = index ? (nx + x) / 2 : x;
        draw->ny = index ? (ny + y) / 2 : y;
        nx = x;
        ny = y;
    }
    hd_beam_draw[count - 1].nx = nx;
    hd_beam_draw[count - 1].ny = ny;
    for (index = 0; index < count; ++index)
    {
        HD_BEAM_DRAW *draw = hd_beam_draw + index;
        float width = draw->valid ? 400 * points[index].size / points[index].point.vz : 0;
        hd_effect_offset(draw->edges, &points[index].point, draw->nx * width, draw->ny * width);
        hd_effect_offset(draw->edges + 1, &points[index].point, -draw->nx * width, -draw->ny * width);
    }
    for (index = 1; index < count; ++index)
    {
        const HD_BEAM_DRAW *previous = hd_beam_draw + index - 1, *next = hd_beam_draw + index;
        HD_EFFECT effect = {0};
        uint32 edge;
        if (!(previous->nx || previous->ny) || !(next->nx || next->ny))
            continue;
        effect.count = 4;
        effect.page = page;
        effect.flags = HD_SEMITRANSPARENT;
        effect.vertices[2].pos = points[index].point;
        effect.vertices[3].pos = points[index - 1].point;
        effect.vertices[2].color = points[index].color;
        effect.vertices[3].color = points[index - 1].color;
        if (!hd_effect_sort(&effect, &points[index].point, 1, HD_DEPTH_MIN, 0, 0))
            return 0;
        if (mode >= 3 && valid_history)
        {
            effect.vertices[0].pos = hd_effect_unproject(&points[index].history[1], points[index].point.vz);
            effect.vertices[1].pos = hd_effect_unproject(&points[index - 1].history[1], points[index - 1].point.vz);
            if (!hd_effect_add(&effect, points[index].point.vz, bias, 0))
                return 0;
        }
        for (edge = 0; edge < 2; ++edge)
        {
            effect.vertices[0].pos = next->edges[edge];
            effect.vertices[1].pos = previous->edges[edge];
            if (!hd_effect_add(&effect, points[index].point.vz, bias, 0))
                return 0;
        }
    }
    if (mode)
        for (index = 0; index < count; ++index)
        {
            if (mode == 1 || !valid_history)
                points[index].history[1] = points[index].history[0] = hd_beam_draw[index].screen;
            else
            {
                points[index].history[1] = points[index].history[0];
                points[index].history[0] = hd_beam_draw[index].screen;
            }
        }
    return 1;
}

void hd_beam_reset(HD_BEAM_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_beam_free(HD_BEAM_HISTORY *history)
{
    if (history)
    {
        free(history->previous);
        free(history->current);
        free(history);
    }
}

static int hd_beam_record(HD_BEAM_HISTORY **owner, const HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page, float near_depth, float far_depth, int logic)
{
    HD_BEAM_HISTORY *history = *owner;
    uint32 index, begin = hd_beam_count, previous_mode, previous_bias;
    uint16 previous_page;
    int seed, previous_valid, sample_step;
    if (hd_active || count < 2 || (!logic && (near_depth <= 0 || far_depth <= near_depth)) || count > 16777215 - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count - hd_polygon_count - hd_model_effect_count || count > (size_t)-1 / sizeof(*points))
        return 0;
    if (!logic && history && history->initialized && history->serial == hd_capture_serial && history->epoch == hd_history_epoch && (!history->clock.valid || (history->clock.published && history->clock.published_tick == hd_logic_tick)))
        return 1;
    if (!history || history->count != count)
    {
        HD_BEAM_HISTORY *replacement = (HD_BEAM_HISTORY *)calloc(1, sizeof(*replacement));
        if (!replacement)
            return 0;
        replacement->previous = (HD_BEAM_POINT *)malloc(count * sizeof(*points));
        replacement->current = (HD_BEAM_POINT *)malloc(count * sizeof(*points));
        if (!replacement->previous || !replacement->current)
        {
            hd_beam_free(replacement);
            return 0;
        }
        replacement->count = count;
        hd_beam_free(history);
        *owner = history = replacement;
    }
    sample_step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, 0, logic);
    if (sample_step < 0) return 0;
    if (sample_step == HD_SAMPLE_SEED || sample_step == HD_SAMPLE_ADVANCE)
    {
        seed = sample_step == HD_SAMPLE_SEED || !valid_history;
        history->previous_mode = seed ? mode : history->mode;
        history->previous_valid = seed ? valid_history : history->valid;
        history->previous_bias = seed ? bias : history->bias;
        history->previous_page = seed ? page : history->page;
        memcpy(history->previous, seed ? points : history->current, count * sizeof(*points));
        memcpy(history->current, points, count * sizeof(*points));
        history->mode = mode;
        history->valid = valid_history;
        history->bias = bias;
        history->page = page;
    }
    else if (!logic && sample_step == HD_SAMPLE_REUSE)
    {
        // Source Render can advance beam modes/history without a logic tick
        history->mode = mode;
        history->valid = valid_history;
        history->bias = bias;
        history->page = page;
        for (index = 0; index < count; ++index)
        {
            history->current[index].history[0] = points[index].history[0];
            history->current[index].history[1] = points[index].history[1];
        }
    }
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic) return 1;
    previous_mode = history->previous_mode;
    previous_valid = history->previous_valid;
    previous_bias = history->previous_bias;
    previous_page = history->previous_page;
    mode = history->mode;
    valid_history = history->valid;
    bias = history->bias;
    page = history->page;
    points = history->current;
    if (!hd_reserve((void **)&hd_beams, &hd_beam_sample_capacity, begin + count, sizeof(*hd_beams)))
        return 0;
    for (index = 0; index < count; ++index)
    {
        HD_BEAM_SAMPLE *sample = hd_beams + hd_beam_count++;
        sample->previous = history->previous[index];
        sample->current = history->current[index];
        sample->begin = begin;
        sample->count = count;
        sample->previous_mode = previous_mode;
        sample->current_mode = mode;
        sample->previous_valid = previous_valid;
        sample->current_valid = valid_history;
        sample->previous_bias = previous_bias;
        sample->current_bias = bias;
        sample->previous_page = previous_page;
        sample->current_page = page;
        sample->near_depth = near_depth;
        sample->far_depth = far_depth;
    }
    for (index = 1; index < count; ++index)
    {
        uint32 layer, first = ((previous_mode >= 3 && previous_valid) || (mode >= 3 && valid_history)) ? 0 : 1;
        VECTORF center;
        RotTransf(&hd_capture_camera.view, &points[index].point, &center);
        for (layer = first; layer < 3; ++layer)
        {
            HD_EFFECT effect = {0};
            effect.count = 4;
            effect.page = page;
            effect.flags = HD_SEMITRANSPARENT;
            effect.space = HD_EFFECT_WORLD;
            effect.beam = begin + index + 1;
            effect.beam_layer = layer;
            if (!hd_effect_add(&effect, center.vz, bias, 0))
                return 0;
        }
    }
    history->serial = hd_capture_serial;
    history->epoch = hd_history_epoch;
    history->mode = mode;
    history->valid = valid_history;
    history->bias = bias;
    history->page = page;
    history->initialized = 1;
    return 1;
}

int hd_beam_tick(HD_BEAM_HISTORY **owner, const HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page)
{
    return hd_beam_record(owner, points, count, mode, valid_history, bias, page, 0, 0, 1);
}

int hd_beam_capture(HD_BEAM_HISTORY **owner, HD_BEAM_POINT *points, uint32 count, uint32 mode, int valid_history, uint32 bias, uint16 page, float near_depth, float far_depth)
{
    uint32 index;
    HD_BEAM_HISTORY *history = *owner;
    int captured = history && history->initialized && history->serial == hd_capture_serial && history->epoch == hd_history_epoch && (!history->clock.valid || (history->clock.published && history->clock.published_tick == hd_logic_tick));
    if (!hd_beam_record(owner, points, count, mode, valid_history, bias, page, near_depth, far_depth, 0)) return 0;
    if (captured) return 1;
    history = *owner;
    mode = history->mode;
    valid_history = history->valid;
    if (mode)
        for (index = 0; index < count; ++index)
        {
            VECTORF point, screen;
            RotTransf(&hd_capture_camera.view, &history->current[index].point, &point);
            screen = hd_effect_project(&point);
            if (mode == 1 || !valid_history)
                points[index].history[1] = points[index].history[0] = screen;
            else
            {
                points[index].history[1] = points[index].history[0];
                points[index].history[0] = screen;
            }
        }
    return 1;
}

static int hd_effect_clip(VECTORF *a, VECTORF *b, float near_depth, float far_depth)
{
    VECTORF p0 = *a, p1 = *b;
    float start = 0, end = 1, dz = p1.vz - p0.vz;
    uint32 endpoint;
    if ((p0.vz < near_depth && p1.vz < near_depth) || (p0.vz > far_depth && p1.vz > far_depth))
        return 0;
    if (p0.vz < near_depth) start = (near_depth - p0.vz) / dz;
    if (p1.vz < near_depth) end = (near_depth - p0.vz) / dz;
    if (p0.vz > far_depth) start = (far_depth - p0.vz) / dz;
    if (p1.vz > far_depth) end = (far_depth - p0.vz) / dz;
    for (endpoint = 0; endpoint < 2; ++endpoint)
    {
        float phase = endpoint ? end : start;
        VECTORF *point = endpoint ? b : a;
        point->vx = p0.vx + (p1.vx - p0.vx) * phase;
        point->vy = p0.vy + (p1.vy - p0.vy) * phase;
        point->vz = p0.vz + (p1.vz - p0.vz) * phase;
    }
    return 1;
}

HD_RIBBON_POINT *hd_effect_ribbon_points(uint32 count)
{
    return hd_reserve((void **)&hd_ribbon_points, &hd_ribbon_capacity, count, sizeof(*hd_ribbon_points)) ? hd_ribbon_points : 0;
}

static void hd_effect_ribbon_edges(VECTORF edges[4], const VECTORF *point, float nx, float ny, float width, float height)
{
    uint32 edge;
    for (edge = 0; edge < 4; ++edge)
    {
        float extent = edge < 2 ? width : height;
        float sign = edge & 1 ? -1.0f : 1.0f;
        float scale = 400 * extent / point->vz;
        hd_effect_offset(edges + edge, point, nx * scale * sign, ny * scale * sign);
    }
}

struct HD_RIBBON_HISTORY
{
    HD_SAMPLE_TICK clock;
    HD_RIBBON previous, current;
    HD_RIBBON_POINT *previous_points, *current_points;
    float near_depth, far_depth;
    uint32 count, serial, epoch;
    int initialized;
};

void hd_ribbon_reset(HD_RIBBON_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_ribbon_free(HD_RIBBON_HISTORY *history)
{
    if (history)
    {
        free(history->previous_points);
        free(history->current_points);
        free(history);
    }
}

int hd_effect_ribbon(const HD_RIBBON *ribbon, float near_depth, float far_depth)
{
    static const uint32 order[4] = {0, 2, 1, 3};
    static const uint32 outer[4] = {0, 1, 2, 3};
    static const sint32 inner[4] = {-1, -1, 0, 1};
    static const uint32 u_outer[4] = {16, 48, 0, 64}, u_inner[4] = {32, 32, 16, 48};
    VECTORF previous_edges[4];
    uint32 index, phase = ribbon->v_scroll;
    float nx = 0, ny = 0;
    int start_strip = 1;
    for (index = 0; index + 1 < ribbon->count; ++index)
    {
        const HD_RIBBON_POINT *previous = ribbon->points + index, *next = previous + 1;
        VECTORF a = previous->point, b = next->point, scr_a, scr_b, edges[4];
        float x, y, start_x = nx, start_y = ny, depth;
        uint32 layer;
        if (!hd_effect_clip(&a, &b, near_depth, far_depth))
        {
            nx = ny = 0;
            start_strip = 1;
            continue;
        }
        scr_a = hd_effect_project(&a);
        scr_b = hd_effect_project(&b);
        hd_effect_normal(&scr_a, &scr_b, 3, &x, &y);
        if (nx || ny)
        {
            if (x || y)
            {
                nx = x;
                ny = y;
            }
        }
        else
        {
            if (!(x || y))
            {
                start_strip = 1;
                continue;
            }
            start_x = nx = x;
            start_y = ny = y;
        }
        if (start_strip)
            hd_effect_ribbon_edges(previous_edges, &a, start_x, start_y, previous->width, previous->height);
        hd_effect_ribbon_edges(edges, &b, nx, ny, next->width, next->height);
        if (ribbon->waving && index >= ribbon->count / 2)
        {
            float wave0 = sinf(((ribbon->tick * 200 + 130) & 4095) * RAD12) * 2;
            float wave1 = sinf((((ribbon->tick * 1000) & 0x3FF8) >> 2) * RAD12) * 3;
            float wave2 = sinf(((ribbon->tick * 150 + 100) & 4095) * RAD12) * 2;
            hd_effect_offset(edges, edges, 0, wave0);
            hd_effect_offset(edges + 1, edges + 1, 0, wave1 + wave2);
        }
        depth = a.vz < b.vz ? a.vz : b.vz;
        for (layer = 0; layer < 4; ++layer)
        {
            uint32 part = order[layer], corner;
            uint32 u = ribbon->u + (ribbon->u_scroll & 63), v = ribbon->v + (phase & 63);
            HD_EFFECT effect = {0};
            effect.count = 4;
            effect.flags = HD_TEXTURED | HD_SEMITRANSPARENT;
            effect.page = ribbon->page;
            {
                VECTORF sort[2] = {a, b};
                if (!hd_effect_sort(&effect, sort, 2, HD_DEPTH_MIN, 0, 0))
                    return 0;
            }
            effect.clut = ribbon->clut;
            effect.window = 0x318 | ((ribbon->u >> 3) << 10) | ((ribbon->v >> 3) << 15);
            effect.flags |= HD_FILTERED;
            effect.texture_region = ribbon->texture_region;
            for (corner = 0; corner < 4; ++corner)
            {
                int end = !(corner & 1), center = (corner & 2) != 0;
                const VECTORF *edge = end ? edges : previous_edges;
                const HD_RIBBON_POINT *point = end ? next : previous;
                HD_EFFECT_VERTEX *vertex = effect.vertices + corner;
                if (center && inner[part] < 0)
                    vertex->pos = end ? b : a;
                else
                    vertex->pos = edge[center ? inner[part] : outer[part]];
                vertex->color = center ? (inner[part] < 0 ? point->center : point->edge) : (part < 2 ? point->edge : 0);
                vertex->u = (uint8)(u + (center ? u_inner[part] : u_outer[part]));
                vertex->v = (uint8)(v + (end ? 0 : ribbon->v_step));
            }
            if (!hd_effect_add(&effect, depth, 0, 0))
                return 0;
        }
        memcpy(previous_edges, edges, sizeof(previous_edges));
        phase -= ribbon->v_step;
        start_strip = 0;
    }
    return 1;
}

static int hd_ribbon_record(HD_RIBBON_HISTORY **owner, const HD_RIBBON *ribbon, float near_depth, float far_depth, int logic)
{
    static const uint32 order[4] = {0, 2, 1, 3};
    HD_RIBBON_HISTORY *history = *owner;
    uint32 index, count = ribbon->count, begin = hd_ribbon_count;
    int step;
    if (hd_active || !ribbon->points || count < 2 || count > (size_t)-1 / sizeof(*ribbon->points) || near_depth <= 0 || far_depth <= near_depth || count > 16777215 - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count - hd_polygon_count - hd_model_effect_count)
        return 0;
    if (!logic && history && history->initialized && history->epoch == hd_history_epoch && history->serial == hd_capture_serial && (!history->clock.valid || (history->clock.published && history->clock.published_tick == hd_logic_tick)))
        return 1;
    if (!history || history->count != count)
    {
        HD_RIBBON_HISTORY *replacement = (HD_RIBBON_HISTORY *)calloc(1, sizeof(*replacement));
        if (!replacement)
            return 0;
        replacement->previous_points = (HD_RIBBON_POINT *)malloc(count * sizeof(*ribbon->points));
        replacement->current_points = (HD_RIBBON_POINT *)malloc(count * sizeof(*ribbon->points));
        if (!replacement->previous_points || !replacement->current_points)
        {
            hd_ribbon_free(replacement);
            return 0;
        }
        replacement->count = count;
        hd_ribbon_free(history);
        *owner = history = replacement;
    }
    step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, history->near_depth != near_depth || history->far_depth != far_depth, logic);
    if (step < 0) return 0;
    if (step == HD_SAMPLE_SEED || step == HD_SAMPLE_ADVANCE)
    {
        history->previous = step == HD_SAMPLE_SEED ? *ribbon : history->current;
        memcpy(history->previous_points, step == HD_SAMPLE_SEED ? ribbon->points : history->current_points, count * sizeof(*ribbon->points));
        memcpy(history->current_points, ribbon->points, count * sizeof(*ribbon->points));
        history->current = *ribbon;
        history->previous.points = history->previous_points;
        history->current.points = history->current_points;
    }
    history->near_depth = near_depth;
    history->far_depth = far_depth;
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic) return 1;
    ribbon = &history->current;
    if (!hd_reserve((void **)&hd_ribbons, &hd_ribbon_sample_capacity, begin + count, sizeof(*hd_ribbons)))
        return 0;
    for (index = 0; index < count; ++index)
    {
        HD_RIBBON_SAMPLE *sample = hd_ribbons + hd_ribbon_count++;
        sample->previous = history->previous_points[index];
        sample->current = history->current_points[index];
        sample->previous_config = history->previous;
        sample->current_config = history->current;
        sample->previous_config.points = sample->current_config.points = 0;
        sample->begin = begin;
        sample->count = count;
        sample->near_depth = near_depth;
        sample->far_depth = far_depth;
    }
    for (index = 1; index < count; ++index)
    {
        uint32 layer;
        VECTORF center;
        RotTransf(&hd_capture_camera.view, &history->current_points[index].point, &center);
        for (layer = 0; layer < 4; ++layer)
        {
            HD_EFFECT effect = {0};
            effect.count = 4;
            effect.space = HD_EFFECT_WORLD;
            effect.page = ribbon->page;
            effect.clut = ribbon->clut;
            effect.flags = HD_TEXTURED | HD_SEMITRANSPARENT;
            effect.ribbon = begin + index + 1;
            effect.ribbon_layer = order[layer];
            if (!hd_effect_add(&effect, center.vz, 0, 0))
                return 0;
        }
    }
    history->serial = hd_capture_serial;
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    return 1;
}

int hd_ribbon_capture(HD_RIBBON_HISTORY **owner, const HD_RIBBON *ribbon, float near_depth, float far_depth)
{
    return hd_ribbon_record(owner, ribbon, near_depth, far_depth, 0);
}

int hd_ribbon_tick(HD_RIBBON_HISTORY **owner, const HD_RIBBON *ribbon, float near_depth, float far_depth)
{
    return hd_ribbon_record(owner, ribbon, near_depth, far_depth, 1);
}

int hd_effect_line(const HD_EFFECT *line, float near_depth, float far_depth, int clip, uint32 bias, uint32 forced)
{
    HD_EFFECT effect = *line;
    VECTORF a = line->vertices[0].pos, b = line->vertices[1].pos;
    float start = 0, end = 1, dz = b.vz - a.vz, dx, dy, length;
    uint32 corner;
    if (clip)
    {
        if ((a.vz < near_depth && b.vz < near_depth) || (a.vz > far_depth && b.vz > far_depth))
            return 1;
        if (a.vz < near_depth) start = (near_depth - a.vz) / dz;
        if (b.vz < near_depth) end = (near_depth - a.vz) / dz;
        if (a.vz > far_depth) start = (far_depth - a.vz) / dz;
        if (b.vz > far_depth) end = (far_depth - a.vz) / dz;
    }
    else if (a.vz < near_depth || b.vz < near_depth || a.vz > far_depth || b.vz > far_depth)
        return 1;
    for (corner = 0; corner < 2; ++corner)
    {
        float phase = corner ? end : start;
        VECTORF *point = corner ? &b : &a;
        const VECTORF *p0 = &line->vertices[0].pos, *p1 = &line->vertices[1].pos;
        point->vx = p0->vx + (p1->vx - p0->vx) * phase;
        point->vy = p0->vy + (p1->vy - p0->vy) * phase;
        point->vz = p0->vz + (p1->vz - p0->vz) * phase;
    }
    dx = (b.vx / b.vz - a.vx / a.vz) / hd_effect_pixel_aspect;
    dy = b.vy / b.vz - a.vy / a.vz;
    length = sqrtf(dx * dx + dy * dy);
    if (length <= 0)
        return 1;
    dx /= length * 2;
    dy /= length * 2;
    effect.count = 4;
    hd_effect_offset(&effect.vertices[0].pos, &a, dy, -dx);
    hd_effect_offset(&effect.vertices[1].pos, &b, dy, -dx);
    hd_effect_offset(&effect.vertices[2].pos, &a, -dy, dx);
    hd_effect_offset(&effect.vertices[3].pos, &b, -dy, dx);
    effect.vertices[0].color = effect.vertices[2].color = line->vertices[0].color;
    effect.vertices[1].color = effect.vertices[3].color = line->vertices[1].color;
    {
        VECTORF sort[2] = {a, b};
        if (!hd_effect_sort(&effect, line->sort_endpoint == 2 ? &line->vertices[1].pos : sort, line->sort_endpoint == 2 ? 1 : 2, HD_DEPTH_MIN, 0, 0))
            return 0;
    }
    return hd_effect_add(&effect, line->sort_endpoint == 2 ? line->vertices[1].pos.vz : (a.vz < b.vz ? a.vz : b.vz), bias, forced);
}

int hd_effect_sprite(const VECTORF *world, const HD_UI_IMAGE *image, uint32 color, uint32 flags, float size, float scale_x, float scale_y, float angle, uint32 flip, uint32 bias, uint32 forced, float near_depth, float far_depth, int large)
{
    HD_EFFECT effect = {0};
    VECTORF center;
    float distance = hd_capture_camera.focal_y * 120;
    float left, top, right, bottom, sine = sinf(angle), cosine = cosf(angle);
    uint32 corner;
    RotTransf(&hd_capture_camera.view, world, &center);
    if (center.vz < near_depth || center.vz > far_depth || (!large && size * 128 / center.vz >= 513))
        return 1;
    if (distance <= 0)
        return 0;
    left = image->offset_x * size / distance;
    top = image->offset_y * size / distance;
    right = left + image->width * size / distance;
    bottom = top + image->height * size / distance;
    if (!angle)
    {
        if (image->width * size / center.vz < 5)
            right = left + 2 * center.vz / distance;
        if (image->height * size / center.vz < 5)
            bottom = top + center.vz / distance;
        scale_x = scale_y = 1;
    }
    effect.count = 4;
    effect.page = image->page;
    effect.clut = image->clut;
    effect.flags = flags | HD_TEXTURED;
    effect.space = HD_EFFECT_BILLBOARD;
    if (!hd_effect_sort(&effect, world, 1, HD_DEPTH_MIN, 0, 0))
        return 0;
    for (corner = 0; corner < 4; ++corner)
    {
        uint32 source = corner ^ (flip ? 1 : 0);
        float x = source & 1 ? right : left, y = source & 2 ? bottom : top;
        HD_EFFECT_VERTEX *vertex = effect.vertices + corner;
        vertex->pos = *world;
        effect.offsets[corner].vx = (x * cosine - y * sine) * scale_x * hd_effect_pixel_aspect * hd_capture_camera.focal_y;
        effect.offsets[corner].vy = (x * sine + y * cosine) * scale_y * hd_capture_camera.focal_y;
        vertex->u = image->uv[corner] & 255;
        vertex->v = image->uv[corner] >> 8;
        if (size * 128 / center.vz >= 256 || angle)
        {
            if (corner & 1) vertex->u = (uint8)((sint32)vertex->u - 1);
            if (corner & 2) vertex->v = (uint8)((sint32)vertex->v - 1);
        }
        vertex->color = color;
    }
    return hd_effect_add(&effect, center.vz, bias, forced);
}

static int hd_sprite_record(HD_SPRITE_HISTORY **owner, const HD_SPRITE_SAMPLE *sample, int logic)
{
    HD_SPRITE_HISTORY *history = *owner;
    HD_EFFECT effect = {0};
    VECTORF center;
    uint32 corner;
    int step;
    if (hd_active || sample->near_depth <= 0 || sample->far_depth <= sample->near_depth || hd_sprite_count >= 16777215)
        return 0;
    if (!history)
    {
        history = (HD_SPRITE_HISTORY *)calloc(1, sizeof(*history));
        if (!history)
            return 0;
        *owner = history;
    }
    step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, history->initialized && history->samples.current.pixels != sample->pixels, logic);
    if (step < 0)
        return 0;
    if (step == HD_SAMPLE_SEED)
        history->samples.previous = history->samples.current = *sample;
    else if (step == HD_SAMPLE_ADVANCE)
    {
        history->samples.previous = history->samples.current;
        history->samples.current = *sample;
    }
    else if (step == HD_SAMPLE_REUSE)
    {
        history->samples.current.near_depth = sample->near_depth;
        history->samples.current.far_depth = sample->far_depth;
    }
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic)
        return 1;
    history->serial = hd_capture_serial;
    sample = &history->samples.current;
    if (!hd_reserve((void **)&hd_sprites, &hd_sprite_capacity, hd_sprite_count + 1, sizeof(*hd_sprites)))
        return 0;
    hd_sprites[hd_sprite_count] = history->samples;
    effect.sprite = ++hd_sprite_count;
    effect.space = HD_EFFECT_BILLBOARD;
    effect.count = 4;
    effect.page = sample->image.page;
    effect.clut = sample->image.clut;
    effect.flags = sample->flags | (sample->pixels ? 0 : HD_TEXTURED);
    for (corner = 0; corner < 4; ++corner)
    {
        uint32 source = corner ^ (sample->flip ? 1 : 0);
        effect.vertices[corner].pos.vx = (float)(source & 1);
        effect.vertices[corner].pos.vy = (float)(source >> 1);
        effect.vertices[corner].u = sample->image.uv[corner] & 255;
        effect.vertices[corner].v = sample->image.uv[corner] >> 8;
        effect.vertices[corner].color = sample->color;
    }
    RotTransf(&hd_capture_camera.view, &sample->pos, &center);
    if (!hd_effect_sort(&effect, &sample->pos, 1, HD_DEPTH_MIN, 0, 0))
        return 0;
    return hd_effect_add(&effect, center.vz, sample->bias, sample->forced);
}

int hd_sprite_capture(HD_SPRITE_HISTORY **owner, const HD_SPRITE_SAMPLE *sample)
{
    return hd_sprite_record(owner, sample, 0);
}

int hd_sprite_tick(HD_SPRITE_HISTORY **owner, const HD_SPRITE_SAMPLE *sample)
{
    return hd_sprite_record(owner, sample, 1);
}

void hd_sprite_reset(HD_SPRITE_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_sprite_free(HD_SPRITE_HISTORY *history)
{
    free(history);
}

static float hd_line_depth(const HD_LINE_PAIR *pair, float phase, const MATRIXF *view)
{
    const HD_LINE_SAMPLE *sample = &pair->current;
    VECTORF points[2], clipped[2];
    float start = 0, end = 1, dz;
    uint32 point;
    for (point = 0; point < 2; ++point)
    {
        const VECTORF *a = pair->previous.points + point, *b = sample->points + point;
        VECTORF world = {a->vx + (b->vx - a->vx) * phase, a->vy + (b->vy - a->vy) * phase, a->vz + (b->vz - a->vz) * phase};
        RotTransf(view, &world, points + point);
    }
    if (sample->sort_endpoint == 2)
        return points[1].vz;
    dz = points[1].vz - points[0].vz;
    if (sample->clip && dz != 0)
    {
        if (points[0].vz < sample->near_depth) start = (sample->near_depth - points[0].vz) / dz;
        if (points[1].vz < sample->near_depth) end = (sample->near_depth - points[0].vz) / dz;
        if (points[0].vz > sample->far_depth) start = (sample->far_depth - points[0].vz) / dz;
        if (points[1].vz > sample->far_depth) end = (sample->far_depth - points[0].vz) / dz;
    }
    clipped[0].vz = points[0].vz + dz * start;
    clipped[1].vz = points[0].vz + dz * end;
    return clipped[0].vz < clipped[1].vz ? clipped[0].vz : clipped[1].vz;
}

static int hd_line_record(HD_LINE_HISTORY *history, const HD_LINE_SAMPLE *sample, int logic)
{
    HD_EFFECT effect = {0};
    uint32 corner;
    int step;
    if (hd_active || sample->near_depth <= 0 || sample->far_depth <= sample->near_depth || hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count >= 16777215)
        return 0;
    step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, 0, logic);
    if (step < 0)
        return 0;
    if (step == HD_SAMPLE_SEED)
        history->samples.previous = history->samples.current = *sample;
    else if (step == HD_SAMPLE_ADVANCE)
    {
        history->samples.previous = history->samples.current;
        history->samples.current = *sample;
    }
    else if (step == HD_SAMPLE_REUSE)
    {
        history->samples.current.near_depth = sample->near_depth;
        history->samples.current.far_depth = sample->far_depth;
    }
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic)
        return 1;
    history->serial = hd_capture_serial;
    sample = &history->samples.current;
    if (!hd_reserve((void **)&hd_lines, &hd_line_capacity, hd_line_count + 1, sizeof(*hd_lines)))
        return 0;
    hd_lines[hd_line_count] = history->samples;
    effect.line = ++hd_line_count;
    effect.count = 4;
    effect.space = HD_EFFECT_WORLD;
    effect.page = sample->page;
    effect.flags = sample->flags;
    for (corner = 0; corner < 4; ++corner)
        effect.vertices[corner].color = sample->colors[corner & 1];
    return hd_effect_add(&effect, hd_line_depth(&history->samples, 1, &hd_capture_camera.view), sample->bias, sample->forced);
}

static int hd_line_owner_record(HD_LINE_HISTORY **owner, const HD_LINE_SAMPLE *sample, int logic)
{
    if (!*owner)
    {
        *owner = (HD_LINE_HISTORY *)calloc(1, sizeof(**owner));
        if (!*owner)
            return 0;
    }
    return hd_line_record(*owner, sample, logic);
}

int hd_line_capture(HD_LINE_HISTORY **owner, const HD_LINE_SAMPLE *sample)
{
    return hd_line_owner_record(owner, sample, 0);
}

int hd_line_tick(HD_LINE_HISTORY **owner, const HD_LINE_SAMPLE *sample)
{
    return hd_line_owner_record(owner, sample, 1);
}

void hd_line_reset(HD_LINE_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_line_free(HD_LINE_HISTORY *history)
{
    free(history);
}

HD_LINE_SAMPLE *hd_line_samples(uint32 count)
{
    return hd_reserve((void **)&hd_line_staging, &hd_line_staging_capacity, count, sizeof(*hd_line_staging)) ? hd_line_staging : 0;
}

static int hd_line_chain_record(HD_LINE_CHAIN **owner, const HD_LINE_SAMPLE *samples, uint32 count, int logic)
{
    HD_LINE_CHAIN *history = *owner;
    uint32 line;
    if (hd_active || count > 16777215 || count > (size_t)-1 / sizeof(HD_LINE_HISTORY))
        return 0;
    if (!history)
    {
        history = (HD_LINE_CHAIN *)calloc(1, sizeof(*history));
        if (!history)
            return 0;
        *owner = history;
    }
    if (history->count != count)
    {
        HD_LINE_HISTORY *replacement = count ? (HD_LINE_HISTORY *)calloc(count, sizeof(*replacement)) : 0;
        if (count && !replacement)
            return 0;
        free(history->lines);
        history->lines = replacement;
        history->count = count;
    }
    for (line = 0; line < count; ++line)
        if (!hd_line_record(history->lines + line, samples + line, logic))
            return 0;
    return 1;
}

int hd_line_chain_capture(HD_LINE_CHAIN **owner, const HD_LINE_SAMPLE *samples, uint32 count)
{
    return hd_line_chain_record(owner, samples, count, 0);
}

int hd_line_chain_tick(HD_LINE_CHAIN **owner, const HD_LINE_SAMPLE *samples, uint32 count)
{
    return hd_line_chain_record(owner, samples, count, 1);
}

void hd_line_chain_reset(HD_LINE_CHAIN *history)
{
    uint32 line;
    if (history)
        for (line = 0; line < history->count; ++line)
            hd_line_reset(history->lines + line);
}

void hd_line_chain_free(HD_LINE_CHAIN *history)
{
    if (history)
    {
        free(history->lines);
        free(history);
    }
}

static uint32 hd_trail_segment(uint32 count, uint32 step, sint64 tail, sint64 sample)
{
    sint64 offset = sample - tail;
    if (offset <= 0)
        return 0;
    if (offset >= (sint64)(count - 1) * step)
        return count - 2;
    return (uint32)(offset / step);
}

static VECTORF hd_trail_point(const VECTORF *points, uint32 segment, uint32 step, sint64 tail, sint64 sample, float *range)
{
    const VECTORF *a = points + segment, *b = a + 1;
    VECTORF point;
    sint64 offset = sample - tail - (sint64)segment * step;
    float phase = offset <= 0 ? 0 : (offset >= step ? 1 : (float)(sint32)offset / step);
    *range = phase;
    if (phase == 0)
        return *a;
    if (phase == 1)
        return *b;
    point.vx = a->vx + (b->vx - a->vx) * phase;
    point.vy = a->vy + (b->vy - a->vy) * phase;
    point.vz = a->vz + (b->vz - a->vz) * phase;
    return point;
}

int hd_trail_correspond(const VECTORF *previous, const VECTORF *current, uint32 count, uint32 step, uint32 advance, HD_TRAIL_EDGE *edges, uint32 capacity, uint32 *used)
{
    sint64 span, before_tail, after_tail, last = 0;
    uint32 before = 0, after = 0, have_last = 0;
    *used = 0;
    if (count < 2 || !step || count > 0x7FFFFFFF || (uint64)(count - 1) * step > 0x7FFFFFFF || capacity < count * 2 - 1)
        return 0;
    span = (sint64)(count - 1) * step;
    before_tail = -span;
    after_tail = (sint64)advance - span;
    while (before < count || after < count)
    {
        sint64 a = before_tail + (sint64)before * step, b = after_tail + (sint64)after * step;
        sint64 sample;
        if (before == count)
            sample = b;
        else if (after == count)
            sample = a;
        else
            sample = a < b ? a : b;
        if (before < count && sample == a)
            ++before;
        if (after < count && sample == b)
            ++after;
        if (have_last)
        {
            HD_TRAIL_EDGE *edge = edges + (*used)++;
            edge->previous_segment = hd_trail_segment(count, step, before_tail, last);
            edge->current_segment = hd_trail_segment(count, step, after_tail, last);
            edge->previous[0] = hd_trail_point(previous, edge->previous_segment, step, before_tail, last, edge->previous_range);
            edge->previous[1] = hd_trail_point(previous, edge->previous_segment, step, before_tail, sample, edge->previous_range + 1);
            edge->current[0] = hd_trail_point(current, edge->current_segment, step, after_tail, last, edge->current_range);
            edge->current[1] = hd_trail_point(current, edge->current_segment, step, after_tail, sample, edge->current_range + 1);
        }
        last = sample;
        have_last = 1;
    }
    return 1;
}

HD_TRAIL_SEGMENT *hd_trail_segments(uint32 count)
{
    return hd_reserve((void **)&hd_trail_staging, &hd_trail_staging_capacity, count, sizeof(*hd_trail_staging)) ? hd_trail_staging : 0;
}

int hd_trail_captured(const HD_TRAIL_HISTORY *history)
{
    return history && history->initialized && history->epoch == hd_history_epoch && history->serial == hd_capture_serial && (!history->clock.valid || (history->clock.published && history->clock.published_tick == hd_logic_tick));
}

void hd_trail_reset(HD_TRAIL_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_trail_free(HD_TRAIL_HISTORY *history)
{
    if (history)
    {
        free(history->previous);
        free(history->current);
        free(history->positions);
        free(history->edges);
        free(history);
    }
}

static int hd_trail_record(HD_TRAIL_HISTORY **owner, const HD_TRAIL_SEGMENT *segments, uint32 count, uint32 step, uint64 insertions, int logic)
{
    HD_TRAIL_HISTORY *history = *owner;
    uint32 index, point, begin = hd_strip_count;
    int seed, sample_step, changed;
    if (hd_active || !count || (uint64)count * step > 0x7FFFFFFF || count > ((size_t)-1 / sizeof(HD_TRAIL_EDGE) - 1) / 2 || count + 1 > (size_t)-1 / (2 * sizeof(VECTORF)) || count > (size_t)-1 / sizeof(HD_TRAIL_SEGMENT))
        return 0;
    if (!logic && hd_trail_captured(history))
        return 1;
    if (!history || history->count != count)
    {
        HD_TRAIL_HISTORY *replacement = (HD_TRAIL_HISTORY *)calloc(1, sizeof(*replacement));
        if (!replacement)
            return 0;
        replacement->previous = (HD_TRAIL_SEGMENT *)malloc(count * sizeof(*segments));
        replacement->current = (HD_TRAIL_SEGMENT *)malloc(count * sizeof(*segments));
        replacement->positions = (VECTORF *)malloc((count + 1) * 2 * sizeof(VECTORF));
        replacement->edges = (HD_TRAIL_EDGE *)malloc((count * 2 + 1) * sizeof(HD_TRAIL_EDGE));
        if (!replacement->previous || !replacement->current || !replacement->positions || !replacement->edges)
        {
            hd_trail_free(replacement);
            return 0;
        }
        replacement->count = count;
        hd_trail_free(history);
        *owner = history = replacement;
    }
    changed = history->step != step;
    sample_step = hd_sample_step(&history->clock, history->serial, history->epoch, history->initialized, changed, logic);
    if (sample_step < 0) return 0;
    if (sample_step == HD_SAMPLE_SEED || sample_step == HD_SAMPLE_ADVANCE)
    {
        seed = sample_step == HD_SAMPLE_SEED || insertions < history->insertions || insertions - history->insertions > 0xFFFFFFFF || (!history->insertions && insertions);
        if (!seed)
            for (index = 0; index < count; ++index)
                seed |= history->current[index].present != segments[index].present;
        memcpy(history->previous, seed ? segments : history->current, count * sizeof(*segments));
        memcpy(history->current, segments, count * sizeof(*segments));
        for (point = 0; point <= count; ++point)
        {
            history->positions[point] = point ? history->previous[point - 1].points[1] : history->previous[0].points[0];
            history->positions[count + 1 + point] = point ? history->current[point - 1].points[1] : history->current[0].points[0];
        }
        history->edge_count = 0;
        if (step && !hd_trail_correspond(history->positions, history->positions + count + 1, count + 1, step, seed ? 0 : (uint32)(insertions - history->insertions), history->edges, count * 2 + 1, &history->edge_count))
            return 0;
        history->insertions = insertions;
        history->step = step;
    }
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    if (logic) return 1;
    history->serial = hd_capture_serial;
    for (index = 0; index < count; ++index)
    {
        history->previous[index].near_depth = history->current[index].near_depth = segments[index].near_depth;
        history->previous[index].far_depth = history->current[index].far_depth = segments[index].far_depth;
    }
    for (index = 0; index < history->edge_count; ++index)
    {
        const HD_TRAIL_EDGE *edge = history->edges + index;
        const HD_TRAIL_SEGMENT *sample = history->current + edge->current_segment;
        HD_STRIP_DRAW *draw;
        HD_EFFECT effect = {0};
        if (!sample->present)
            continue;
        if (hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count >= 16777215 || !hd_reserve((void **)&hd_strips, &hd_strip_capacity, hd_strip_count + 1, sizeof(*hd_strips)))
            return 0;
        draw = hd_strips + hd_strip_count++;
        draw->edge = *edge;
        draw->previous = history->previous[edge->previous_segment];
        draw->current = *sample;
        draw->begin = begin;
        effect.count = 4;
        effect.space = HD_EFFECT_WORLD;
        effect.strip = hd_strip_count;
        effect.page = sample->image.page;
        effect.clut = sample->image.clut;
        effect.flags = HD_TEXTURED | HD_FILTERED | ((sample->color & 0x1000000) ? HD_RAW_TEXTURE : 0) | ((sample->color & 0x2000000) ? HD_SEMITRANSPARENT : 0);
        for (point = 0; point < 4; ++point)
        {
            effect.vertices[point].color = sample->color;
            effect.vertices[point].u = sample->image.uv[point] & 255;
            effect.vertices[point].v = sample->image.uv[point] >> 8;
        }
        {
            VECTORF center;
            RotTransf(&hd_capture_camera.view, edge->current + 1, &center);
            if (!hd_effect_add(&effect, center.vz, sample->bias, 0))
                return 0;
        }
    }
    return 1;
}

int hd_trail_capture(HD_TRAIL_HISTORY **owner, const HD_TRAIL_SEGMENT *segments, uint32 count, uint32 step, uint64 insertions)
{
    return hd_trail_record(owner, segments, count, step, insertions, 0);
}

int hd_trail_tick(HD_TRAIL_HISTORY **owner, const HD_TRAIL_SEGMENT *segments, uint32 count, uint32 step, uint64 insertions)
{
    return hd_trail_record(owner, segments, count, step, insertions, 1);
}

static float hd_effect_dot(const VECTORF *a, const VECTORF *b)
{
    return a->vx * b->vx + a->vy * b->vy + a->vz * b->vz;
}

int hd_model_effect_geometry(const HD_MODEL_EFFECT *source, const HD_CAMERA *camera, HD_POLYGON_SAMPLE *sample)
{
    VECTORF sort[4];
    VECTORF local_center = {(source->minimum.vx + source->maximum.vx) / 2, (source->minimum.vy + source->maximum.vy) / 2, (source->minimum.vz + source->maximum.vz) / 2};
    VECTORF extent = {(source->maximum.vx - source->minimum.vx) / 2, (source->maximum.vy - source->minimum.vy) / 2, (source->maximum.vz - source->minimum.vz) / 2};
    VECTORF center, direction, n, cross, normal, basis[2], axis = {0}, unused;
    MATRIXF inverse, diagonal = {0};
    float average, length, factor;
    uint32 edge, corner;
    HD_EFFECT effect = {0};
    memset(sample, 0, sizeof(*sample));
    extent.vx *= source->projection ? 1.25f : source->scale;
    extent.vy *= source->projection ? 1.375f : source->scale;
    extent.vz *= source->projection ? 1.25f : source->scale;
    if (extent.vx <= 0 || extent.vy <= 0 || extent.vz <= 0)
        return 1;
    RotTransf(&source->matrix, &local_center, &center);
    if (source->projection)
        direction = source->direction;
    else
    {
        VECTORF translation = {camera->view.t[0], camera->view.t[1], camera->view.t[2]}, offset;
        TransposeMatrixf(&camera->view, &inverse);
        ApplyMatrixf(&inverse, &translation, &offset);
        direction.vx = center.vx + offset.vx;
        direction.vy = center.vy + offset.vy;
        direction.vz = center.vz + offset.vz;
        NormalizeVectorf(&direction, &direction);
    }
    average = (extent.vx + extent.vy + extent.vz) / 3;
    diagonal.m[0][0] = average / extent.vx;
    diagonal.m[1][1] = average / extent.vy;
    diagonal.m[2][2] = average / extent.vz;
    TransposeMatrixf(&source->matrix, &inverse);
    ApplyMatrixf(&inverse, &direction, &normal);
    NormalizeVectorf(&normal, &normal);
    ApplyMatrixf(&diagonal, &normal, &n);
    length = NormalizeVectorf(&n, &unused);
    if (fabsf(n.vx) <= fabsf(n.vy) && fabsf(n.vx) <= fabsf(n.vz))
        axis.vx = 1;
    else if (fabsf(n.vy) <= fabsf(n.vx) && fabsf(n.vy) <= fabsf(n.vz))
        axis.vy = 1;
    else
        axis.vz = 1;
    OuterProductf(&axis, &n, &cross);
    NormalizeVectorf(&cross, &normal);
    ApplyMatrixf(&diagonal, &normal, basis);
    ApplyMatrixf(&source->matrix, basis, basis);
    OuterProductf(&direction, basis, basis);
    OuterProductf(&cross, &n, &normal);
    NormalizeVectorf(&normal, &normal);
    ApplyMatrixf(&diagonal, &normal, basis + 1);
    ApplyMatrixf(&source->matrix, basis + 1, basis + 1);
    OuterProductf(&direction, basis + 1, basis + 1);
    factor = extent.vx * extent.vy * extent.vz * length / (average * average);
    for (edge = 0; edge < 2; ++edge)
    {
        basis[edge].vx *= factor;
        basis[edge].vy *= factor;
        basis[edge].vz *= factor;
    }
    if (source->projection)
    {
        VECTORF plane = {source->plane.vx - center.vx, source->plane.vy - center.vy, source->plane.vz - center.vz};
        float alignment = hd_effect_dot(&direction, &source->normal);
        float distance = hd_effect_dot(&plane, &source->normal);
        if (alignment < 0.25f || distance > 0)
            return 1;
        factor = distance / alignment;
        center.vx += direction.vx * factor;
        center.vy += direction.vy * factor;
        center.vz += direction.vz * factor;
        // Lift projected shadows vertically above the receiver in world space
        center.vy -= 0.5f;
        for (edge = 0; edge < 2; ++edge)
        {
            factor = -hd_effect_dot(basis + edge, &source->normal) / alignment;
            basis[edge].vx += direction.vx * factor;
            basis[edge].vy += direction.vy * factor;
            basis[edge].vz += direction.vz * factor;
        }
    }
    effect.count = 4;
    effect.space = HD_EFFECT_WORLD;
    effect.flags = HD_TEXTURED | HD_SEMITRANSPARENT;
    effect.flags |= HD_FILTERED;
    effect.texture_region = source->image.region;
    effect.page = (source->image.page & ~0x60) | (source->projection ? 0x40 : 0x20);
    effect.clut = source->image.clut;
    for (corner = 0; corner < 4; ++corner)
    {
        HD_EFFECT_VERTEX *vertex = effect.vertices + corner;
        float a = corner & 1 ? 1.0f : -1.0f, b = corner & 2 ? 1.0f : -1.0f;
        vertex->pos.vx = center.vx + a * basis[0].vx + b * basis[1].vx;
        vertex->pos.vy = center.vy + a * basis[0].vy + b * basis[1].vy;
        vertex->pos.vz = center.vz + a * basis[0].vz + b * basis[1].vz;
        sort[corner] = vertex->pos;
        vertex->color = source->color;
        vertex->u = (source->image.uv[corner] & 255) - (corner & 1 ? 1 : 0);
        vertex->v = (source->image.uv[corner] >> 8) - (corner & 2 ? 1 : 0);
        vertex->u = (uint8)(sint32)vertex->u;
        vertex->v = (uint8)(sint32)vertex->v;
    }
    memcpy(effect.depth.points, sort, sizeof(sort));
    effect.depth.count = 4;
    effect.depth.rule = HD_DEPTH_MEAN;
    effect.depth.offset = source->projection ? 0 : source->depth_bias * 4;
    effect.depth.clamp = 1;
    sample->effect = effect;
    sample->present = 1;
    return 1;
}

int hd_effect_model(const HD_MODEL_EFFECT *source)
{
    HD_POLYGON_PAIR pair;
    if (!hd_model_effect_geometry(source, &hd_capture_camera, &pair.current)) return 0;
    if (!pair.current.present) return 1;
    pair.previous = pair.current;
    return hd_effect_add(&pair.current.effect, hd_polygon_depth(&pair, 1, &hd_capture_camera.view), 0, 0);
}

static VECTORF hd_model_effect_mix(const VECTORF *a, const VECTORF *b, float phase)
{
    VECTORF result = {a->vx + (b->vx - a->vx) * phase, a->vy + (b->vy - a->vy) * phase, a->vz + (b->vz - a->vz) * phase};
    return result;
}

static void hd_model_effect_sample(const HD_MODEL_EFFECT_ENTRY *entry, float phase, HD_MODEL_EFFECT *source)
{
    const HD_MODEL_EFFECT *a = &entry->previous, *b = &entry->current;
    *source = phase <= 0 ? *a : *b;
    if (phase <= 0 || phase >= 1) return;
    hd_matrix_evaluate(&entry->matrix, phase, &source->matrix);
    source->minimum = hd_model_effect_mix(&a->minimum, &b->minimum, phase);
    source->maximum = hd_model_effect_mix(&a->maximum, &b->maximum, phase);
    source->normal = hd_model_effect_mix(&a->normal, &b->normal, phase);
    source->plane = hd_model_effect_mix(&a->plane, &b->plane, phase);
    source->direction = hd_model_effect_mix(&a->direction, &b->direction, phase);
    source->scale = a->scale + (b->scale - a->scale) * phase;
}

struct HD_MODEL_EFFECT_CHAIN
{
    HD_MODEL_EFFECT_ENTRY *entries;
    uint32 count, identity;
};

void hd_model_effect_reset(HD_MODEL_EFFECT_CHAIN *history)
{
    uint32 slot;
    if (history)
        for (slot = 0; slot < history->count; ++slot) history->entries[slot].initialized = 0;
}

void hd_model_effect_free(HD_MODEL_EFFECT_CHAIN *history)
{
    if (history)
    {
        free(history->entries);
        free(history);
    }
}

static int hd_model_effect_record(HD_MODEL_EFFECT_CHAIN **owner, uint32 slot, uint32 count, uint32 identity, const HD_MODEL_EFFECT *source, int logic)
{
    HD_MODEL_EFFECT_CHAIN *history = *owner;
    HD_MODEL_EFFECT_ENTRY *entry;
    int step;
    if (hd_active || slot >= count || count > (size_t)-1 / sizeof(HD_MODEL_EFFECT_ENTRY)) return 0;
    if (!source)
    {
        if (history && history->identity == identity && slot < history->count)
            history->entries[slot].initialized = 0;
        return 1;
    }
    if (!history || history->count != count || history->identity != identity)
    {
        HD_MODEL_EFFECT_CHAIN *replacement = (HD_MODEL_EFFECT_CHAIN *)calloc(1, sizeof(*replacement));
        if (!replacement) return 0;
        replacement->entries = (HD_MODEL_EFFECT_ENTRY *)calloc(count, sizeof(*replacement->entries));
        if (!replacement->entries) { hd_model_effect_free(replacement); return 0; }
        replacement->count = count;
        replacement->identity = identity;
        hd_model_effect_free(history);
        *owner = history = replacement;
    }
    entry = history->entries + slot;
    if (!logic && entry->initialized && entry->epoch == hd_history_epoch && entry->serial == hd_capture_serial && (!entry->clock.valid || (entry->clock.published && entry->clock.published_tick == hd_logic_tick))) return 1;
    step = hd_sample_step(&entry->clock, entry->serial, entry->epoch, entry->initialized, entry->current.projection != source->projection, logic);
    if (step < 0) return 0;
    if (step == HD_SAMPLE_SEED || step == HD_SAMPLE_ADVANCE)
    {
        entry->previous = step == HD_SAMPLE_SEED ? *source : entry->current;
        entry->current = *source;
        if (step == HD_SAMPLE_SEED) entry->matrix.initialized = 0;
        hd_matrix_capture(&entry->matrix, &source->matrix);
    }
    else if (!logic && step == HD_SAMPLE_REUSE)
        entry->current.image = source->image;
    entry->epoch = hd_history_epoch;
    entry->initialized = 1;
    if (logic) return 1;
    source = &entry->current;
    if (hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count >= 16777215 || !hd_reserve((void **)&hd_model_effects, &hd_model_effect_capacity, hd_model_effect_count + 1, sizeof(*hd_model_effects))) return 0;
    hd_model_effects[hd_model_effect_count] = *entry;
    {
        HD_EFFECT effect = {0};
        effect.count = 4;
        effect.space = HD_EFFECT_WORLD;
        effect.flags = HD_TEXTURED | HD_SEMITRANSPARENT;
        effect.page = (source->image.page & ~0x60) | (source->projection ? 0x40 : 0x20);
        effect.clut = source->image.clut;
        effect.model_effect = ++hd_model_effect_count;
        if (!hd_effect_add(&effect, 0, 0, 0)) return 0;
    }
    entry->serial = hd_capture_serial;
    entry->epoch = hd_history_epoch;
    entry->initialized = 1;
    return 1;
}

int hd_model_effect_capture(HD_MODEL_EFFECT_CHAIN **owner, uint32 slot, uint32 count, uint32 identity, const HD_MODEL_EFFECT *source)
{
    return hd_model_effect_record(owner, slot, count, identity, source, 0);
}

int hd_model_effect_tick(HD_MODEL_EFFECT_CHAIN **owner, uint32 slot, uint32 count, uint32 identity, const HD_MODEL_EFFECT *source)
{
    return hd_model_effect_record(owner, slot, count, identity, source, 1);
}

static int hd_sprite_upload(void)
{
    uint32 sprite, cell, component, height = (hd_sprite_count * 14 + 1023) / 1024;
    uint16 *pixels;
    if (!hd_sprite_count && hd_sprite_texture)
        return 1;
    if (!height)
        height = 1;
    if (height > hd_sprite_texture_height)
    {
        VID_TEXTURE *replacement;
        uint32 capacity = hd_sprite_texture_height ? hd_sprite_texture_height : 1;
        while (capacity < height)
            capacity *= 2;
        replacement = vid_tex_create_fmt(1024, capacity, VID_RGBA16UI);
        if (!replacement)
            return 0;
        vid_tex_free(hd_sprite_texture);
        hd_sprite_texture = replacement;
        hd_sprite_texture_height = capacity;
    }
    pixels = (uint16 *)vid_tex_data(hd_sprite_texture);
    for (sprite = 0; sprite < hd_sprite_count; ++sprite)
    {
        const HD_SPRITE_SAMPLE *a = &hd_sprites[sprite].previous, *b = &hd_sprites[sprite].current;
        float values[7][4] = {
            {a->pos.vx, a->pos.vy, a->pos.vz, a->size},
            {b->pos.vx, b->pos.vy, b->pos.vz, b->size},
            {a->scale_x, a->scale_y, a->angle, b->angle},
            {b->scale_x, b->scale_y, b->near_depth, b->far_depth},
            {(float)b->image.width, (float)b->image.height, (float)b->image.offset_x, (float)b->image.offset_y},
            {(float)b->pixels, (float)(a->color & 0xFFFFFF), (float)(b->color & 0xFFFFFF), (float)b->large},
            {(float)b->image.region.x, (float)b->image.region.y, (float)b->image.region.width, (float)b->image.region.height}};
        for (cell = 0; cell < 7; ++cell)
        {
            uint32 bits[4], at = (sprite * 14 + cell * 2) * 4;
            memcpy(bits, values[cell], sizeof(bits));
            for (component = 0; component < 4; ++component)
            {
                pixels[at + component] = bits[component];
                pixels[at + 4 + component] = bits[component] >> 16;
            }
        }
    }
    vid_tex_commit(hd_sprite_texture);
    return 1;
}

static int hd_line_upload(void)
{
    uint32 line, cell, component, height = ((hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count) * 28 + hd_model_effect_count * 6 + 1023) / 1024;
    uint16 *pixels;
    if (!hd_line_count && !hd_strip_count && !hd_beam_count && !hd_ribbon_count && !hd_ring_count && !hd_polygon_count && !hd_model_effect_count && hd_line_texture)
        return 1;
    if (!height)
        height = 1;
    if (height > hd_line_texture_height)
    {
        VID_TEXTURE *replacement;
        uint32 capacity = hd_line_texture_height ? hd_line_texture_height : 1;
        while (capacity < height)
            capacity *= 2;
        replacement = vid_tex_create_fmt(1024, capacity, VID_RGBA16UI);
        if (!replacement)
            return 0;
        vid_tex_free(hd_line_texture);
        hd_line_texture = replacement;
        hd_line_texture_height = capacity;
    }
    pixels = (uint16 *)vid_tex_data(hd_line_texture);
    for (line = 0; line < hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count; ++line)
    {
        float values[14][4] = {{0}};
        if (line < hd_line_count)
        {
            const HD_LINE_SAMPLE *a = &hd_lines[line].previous, *b = &hd_lines[line].current;
            float source[5][4] = {
            {a->points[0].vx, a->points[0].vy, a->points[0].vz, (float)(a->colors[0] & 0xFFFFFF)},
            {b->points[0].vx, b->points[0].vy, b->points[0].vz, (float)(b->colors[0] & 0xFFFFFF)},
            {a->points[1].vx, a->points[1].vy, a->points[1].vz, (float)(a->colors[1] & 0xFFFFFF)},
            {b->points[1].vx, b->points[1].vy, b->points[1].vz, (float)(b->colors[1] & 0xFFFFFF)},
            {b->near_depth, b->far_depth, (float)b->clip, (float)b->sort_endpoint}};
            memcpy(values, source, sizeof(source));
        }
        else if (line < hd_line_count + hd_strip_count)
        {
            const HD_STRIP_DRAW *draw = hd_strips + line - hd_line_count;
            const HD_TRAIL_EDGE *edge = &draw->edge;
            const HD_TRAIL_SEGMENT *a = &draw->previous, *b = &draw->current;
            float source[12][4] = {
                {edge->previous[0].vx, edge->previous[0].vy, edge->previous[0].vz, (float)(a->color & 0xFFFFFF)},
                {edge->current[0].vx, edge->current[0].vy, edge->current[0].vz, (float)(b->color & 0xFFFFFF)},
                {edge->previous[1].vx, edge->previous[1].vy, edge->previous[1].vz, 0},
                {edge->current[1].vx, edge->current[1].vy, edge->current[1].vz, 0},
                {a->near_depth, a->far_depth, b->near_depth, b->far_depth},
                {a->size * a->image.width, b->size * b->image.width, (float)a->flags, (float)b->flags},
                {edge->previous_range[0], edge->previous_range[1], edge->current_range[0], edge->current_range[1]},
                {(float)a->image.uv[0], (float)a->image.uv[1], (float)a->image.uv[2], (float)a->image.uv[3]},
                {(float)b->image.uv[0], (float)b->image.uv[1], (float)b->image.uv[2], (float)b->image.uv[3]},
                {(float)a->image.page, (float)a->image.clut, (float)(HD_TEXTURED | HD_FILTERED | ((a->color >> 23) & 6)), (float)(hd_line_count + draw->begin)},
                {(float)b->image.page, (float)b->image.clut, (float)(HD_TEXTURED | HD_FILTERED | ((b->color >> 23) & 6)), 0},
                {(float)(edge->previous_segment && (a->flags & 16) && edge->previous_range[0] == 0 && edge->previous_range[1] > 0), (float)(edge->current_segment && (b->flags & 16) && edge->current_range[0] == 0 && edge->current_range[1] > 0), 0, 0}};
            memcpy(values, source, sizeof(source));
            values[12][0] = a->image.region.x; values[12][1] = a->image.region.y;
            values[12][2] = a->image.region.width; values[12][3] = a->image.region.height;
            values[13][0] = b->image.region.x; values[13][1] = b->image.region.y;
            values[13][2] = b->image.region.width; values[13][3] = b->image.region.height;
        }
        else if (line < hd_line_count + hd_strip_count + hd_beam_count)
        {
            const HD_BEAM_SAMPLE *sample = hd_beams + line - hd_line_count - hd_strip_count;
            const HD_BEAM_POINT *a = &sample->previous, *b = &sample->current;
            float source[6][4] = {
                {a->point.vx, a->point.vy, a->point.vz, a->size},
                {b->point.vx, b->point.vy, b->point.vz, b->size},
                {a->history[1].vx, a->history[1].vy, (float)(a->color & 0xFFFFFF), (float)sample->previous_page},
                {b->history[1].vx, b->history[1].vy, (float)(b->color & 0xFFFFFF), (float)sample->current_page},
                {sample->near_depth, sample->far_depth, (float)(hd_line_count + hd_strip_count + sample->begin), (float)sample->count},
                {(float)sample->previous_mode, (float)sample->current_mode, (float)sample->previous_valid, (float)sample->current_valid}};
            memcpy(values, source, sizeof(source));
        }
        else if (line < hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count)
        {
            const HD_RIBBON_SAMPLE *sample = hd_ribbons + line - hd_line_count - hd_strip_count - hd_beam_count;
            const HD_RIBBON_POINT *a = &sample->previous, *b = &sample->current;
            const HD_RIBBON *old = &sample->previous_config, *now = &sample->current_config;
            float source[8][4] = {
                {a->point.vx, a->point.vy, a->point.vz, a->width},
                {b->point.vx, b->point.vy, b->point.vz, b->width},
                {a->height, b->height, (float)(a->center & 0xFFFFFF), (float)(b->center & 0xFFFFFF)},
                {(float)(a->edge & 0xFFFFFF), (float)(b->edge & 0xFFFFFF), (float)(hd_line_count + hd_strip_count + hd_beam_count + sample->begin), (float)sample->count},
                {sample->near_depth, sample->far_depth, (float)((old->page & 511) | ((uint32)(old->clut & 32767) << 9)), (float)((now->page & 511) | ((uint32)(now->clut & 32767) << 9))},
                {(float)(old->u | ((uint32)old->v << 8)), (float)(now->u | ((uint32)now->v << 8)), (float)(old->u_scroll & 63), (float)(now->u_scroll & 63)},
                {(float)(old->v_scroll & 63), (float)(now->v_scroll & 63), (float)(old->v_step & 255), (float)(now->v_step & 255)},
                {(float)(old->waving != 0), (float)(now->waving != 0), (float)(old->tick & 16383), (float)(now->tick & 16383)}};
            memcpy(values, source, sizeof(source));
            values[12][0] = old->texture_region.x; values[12][1] = old->texture_region.y;
            values[12][2] = old->texture_region.width; values[12][3] = old->texture_region.height;
            values[13][0] = now->texture_region.x; values[13][1] = now->texture_region.y;
            values[13][2] = now->texture_region.width; values[13][3] = now->texture_region.height;
        }
        else if (line < hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count)
        {
            const HD_RING_PAIR *pair = hd_rings + line - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count;
            const HD_RING_PRIMITIVE *a = &pair->previous, *b = &pair->current;
            float source[9][4] = {
                {a->center.vx, a->center.vy, a->center.vz, (float)a->page},
                {b->center.vx, b->center.vy, b->center.vz, (float)b->page},
                {a->angles[0], a->angles[1], b->angles[0], b->angles[1]},
                {a->radii[0], a->radii[1], a->radii[2], a->radii[3]},
                {b->radii[0], b->radii[1], b->radii[2], b->radii[3]},
                {(float)(a->colors[0] & 0xFFFFFF), (float)(a->colors[1] & 0xFFFFFF), (float)(a->colors[2] & 0xFFFFFF), (float)(a->colors[3] & 0xFFFFFF)},
                {(float)(b->colors[0] & 0xFFFFFF), (float)(b->colors[1] & 0xFFFFFF), (float)(b->colors[2] & 0xFFFFFF), (float)(b->colors[3] & 0xFFFFFF)},
                {(float)a->flags, (float)b->flags, (float)a->visible, (float)b->visible},
                {(float)pair->count, 0, 0, 0}};
            memcpy(values, source, sizeof(source));
        }
        else if (line < hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count)
        {
            const HD_POLYGON_PAIR *pair = hd_polygons + line - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count;
            const HD_EFFECT *a = &pair->previous.effect, *b = &pair->current.effect;
            uint32 corner;
            for (corner = 0; corner < 4; ++corner)
            {
                const HD_EFFECT_VERTEX *p = a->vertices + (corner < a->count ? corner : 2);
                const HD_EFFECT_VERTEX *q = b->vertices + (corner < b->count ? corner : 2);
                values[corner][0] = p->pos.vx; values[corner][1] = p->pos.vy; values[corner][2] = p->pos.vz; values[corner][3] = (float)(p->color & 0xFFFFFF);
                values[corner + 4][0] = q->pos.vx; values[corner + 4][1] = q->pos.vy; values[corner + 4][2] = q->pos.vz; values[corner + 4][3] = (float)(q->color & 0xFFFFFF);
                values[8][corner] = (float)((uint8)p->u | ((uint32)(uint8)p->v << 8));
                values[9][corner] = (float)((uint8)q->u | ((uint32)(uint8)q->v << 8));
            }
            values[10][0] = (float)((a->page & 511) | ((uint32)(a->clut & 32767) << 9));
            values[10][1] = (float)((b->page & 511) | ((uint32)(b->clut & 32767) << 9));
            values[10][2] = (float)(a->flags | (pair->previous.present ? 256 : 0) | (a->cull_positive ? 512 : 0));
            values[10][3] = (float)(b->flags | (pair->current.present ? 256 : 0) | (b->cull_positive ? 512 : 0));
            values[11][0] = (float)(a->window & 0xFFFFF); values[11][1] = (float)(b->window & 0xFFFFF);
            values[11][2] = (float)pair->previous.uv_period; values[11][3] = (float)pair->current.uv_period;
            values[12][0] = a->texture_region.x; values[12][1] = a->texture_region.y;
            values[12][2] = a->texture_region.width; values[12][3] = a->texture_region.height;
            values[13][0] = b->texture_region.x; values[13][1] = b->texture_region.y;
            values[13][2] = b->texture_region.width; values[13][3] = b->texture_region.height;
        }
        else
        {
            const HD_MODEL_EFFECT_ENTRY *entry = hd_model_effects + line - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count - hd_polygon_count;
            const HD_MODEL_EFFECT *a = &entry->previous, *b = &entry->current;
            uint32 page_a = (a->image.page & ~0x60) | (a->projection ? 0x40 : 0x20), page_b = (b->image.page & ~0x60) | (b->projection ? 0x40 : 0x20);
            float source[12][4] = {
                {a->minimum.vx, a->minimum.vy, a->minimum.vz, a->scale},
                {a->maximum.vx, a->maximum.vy, a->maximum.vz, (float)a->projection},
                {b->minimum.vx, b->minimum.vy, b->minimum.vz, b->scale},
                {b->maximum.vx, b->maximum.vy, b->maximum.vz, (float)b->projection},
                {a->direction.vx, a->direction.vy, a->direction.vz, (float)(a->color & 0xFFFFFF)},
                {b->direction.vx, b->direction.vy, b->direction.vz, (float)(b->color & 0xFFFFFF)},
                {a->normal.vx, a->normal.vy, a->normal.vz, (float)((page_a & 511) | ((uint32)(a->image.clut & 32767) << 9))},
                {b->normal.vx, b->normal.vy, b->normal.vz, (float)((page_b & 511) | ((uint32)(b->image.clut & 32767) << 9))},
                {a->plane.vx, a->plane.vy, a->plane.vz, 1},
                {b->plane.vx, b->plane.vy, b->plane.vz, 1},
                {(float)a->image.uv[0], (float)a->image.uv[1], (float)a->image.uv[2], (float)a->image.uv[3]},
                {(float)b->image.uv[0], (float)b->image.uv[1], (float)b->image.uv[2], (float)b->image.uv[3]}};
            source[8][3] = (float)(line - hd_line_count - hd_strip_count - hd_beam_count - hd_ribbon_count - hd_ring_count - hd_polygon_count);
            memcpy(values, source, sizeof(source));
            values[12][0] = a->image.region.x; values[12][1] = a->image.region.y;
            values[12][2] = a->image.region.width; values[12][3] = a->image.region.height;
            values[13][0] = b->image.region.x; values[13][1] = b->image.region.y;
            values[13][2] = b->image.region.width; values[13][3] = b->image.region.height;
        }
        for (cell = 0; cell < 14; ++cell)
        {
            uint32 bits[4], at = (line * 28 + cell * 2) * 4;
            memcpy(bits, values[cell], sizeof(bits));
            for (component = 0; component < 4; ++component)
            {
                pixels[at + component] = bits[component];
                pixels[at + 4 + component] = bits[component] >> 16;
            }
        }
    }
    vid_tex_commit(hd_line_texture);
    return 1;
}

static int hd_model_effect_upload(void)
{
    uint32 total = hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count;
    uint32 index, row, column;
    uint16 *pixels;
    int changed = 0;
    if (!hd_model_effect_count)
        return 1;
    pixels = (uint16 *)vid_tex_data(hd_line_texture);
    for (index = 0; index < hd_model_effect_count; ++index)
    {
        MATRIXF matrix;
        hd_matrix_evaluate(&hd_model_effects[index].matrix, hd_render_phase, &matrix);
        for (row = 0; row < 3; ++row)
        {
            float values[4];
            uint32 bits[4], at = (total * 28 + index * 6 + row * 2) * 4;
            uint16 packed[8];
            for (column = 0; column < 3; ++column) values[column] = matrix.m[row][column];
            values[3] = matrix.t[row];
            memcpy(bits, values, sizeof(bits));
            for (column = 0; column < 4; ++column)
            {
                packed[column] = (uint16)bits[column];
                packed[column + 4] = (uint16)(bits[column] >> 16);
            }
            if (memcmp(pixels + at, packed, sizeof(packed)))
            {
                memcpy(pixels + at, packed, sizeof(packed));
                changed = 1;
            }
        }
    }
    if (changed) vid_tex_commit(hd_line_texture);
    return 1;
}

static int hd_effect_upload(void)
{
    uint32 index;
    VID_VERTEX3D *vertices;
    if (!hd_effect_count)
        return 1;
    if (hd_effect_upload_valid && hd_effect_upload_version == hd_effect_version)
        return 1;
    if (!hd_sprite_upload() || !hd_line_upload())
        return 0;
    if (hd_effect_count > hd_effect_mesh_capacity)
    {
        uint32 capacity = hd_effect_capacity;
        if (capacity > 0xFFFFFFFF / 6)
            return 0;
        vid_mesh_free(hd_effect_mesh);
        hd_effect_mesh_capacity = 0;
        hd_effect_mesh = vid_mesh_create_3d(capacity * 4);
        if (!hd_effect_mesh)
            return 0;
        hd_effect_mesh_capacity = capacity;
    }
    vertices = vid_mesh_data_3d(hd_effect_mesh);
    for (index = 0; index < hd_effect_count; ++index)
    {
        const HD_EFFECT *effect = &hd_effects[index].effect;
        uint32 corner, window = effect->window;
        for (corner = 0; corner < 4; ++corner)
        {
            const HD_EFFECT_VERTEX *source = effect->vertices + (corner < effect->count ? corner : 2);
            VID_VERTEX3D *vertex = vertices + index * 4 + corner;
            memset(vertex, 0, sizeof(*vertex));
            vertex->pos[0] = source->pos.vx;
            vertex->pos[1] = source->pos.vy;
            vertex->pos[2] = source->pos.vz;
            vertex->pos[3] = (float)(effect->forced_depth * 4);
            vertex->uv[0] = source->u;
            vertex->uv[1] = source->v;
            vertex->color[0] = source->color & 255;
            vertex->color[1] = (source->color >> 8) & 255;
            vertex->color[2] = (source->color >> 16) & 255;
            vertex->color[3] = 255;
            if (effect->space == HD_EFFECT_BILLBOARD)
            {
                uint32 offset = corner < effect->count ? corner : 2;
                vertex->n[0] = effect->offsets[offset].vx;
                vertex->n[1] = effect->offsets[offset].vy;
                vertex->n[2] = effect->offsets[offset].vz;
            }
            if (effect->sprite)
            {
                vertex->n[0] = (float)corner;
                vertex->n[1] = vertex->n[2] = 0;
                vertex->n[3] = (float)effect->sprite;
            }
            if (effect->line)
            {
                vertex->n[0] = (float)corner;
                vertex->n[3] = -(float)effect->line;
            }
            if (effect->strip)
            {
                vertex->n[0] = (float)corner;
                vertex->n[1] = 1;
                vertex->n[3] = -(float)(hd_line_count + effect->strip);
            }
            if (effect->beam)
            {
                vertex->n[0] = (float)corner;
                vertex->n[1] = (float)(2 + effect->beam_layer);
                vertex->n[3] = -(float)(hd_line_count + hd_strip_count + effect->beam);
            }
            if (effect->ribbon)
            {
                vertex->n[0] = (float)corner;
                vertex->n[1] = (float)(5 + effect->ribbon_layer);
                vertex->n[3] = -(float)(hd_line_count + hd_strip_count + hd_beam_count + effect->ribbon);
            }
            if (effect->ring)
            {
                vertex->n[0] = (float)(corner < effect->count ? corner : 2);
                vertex->n[1] = 9;
                vertex->n[3] = -(float)(hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + effect->ring);
            }
            if (effect->polygon)
            {
                vertex->n[0] = (float)corner;
                vertex->n[1] = 10;
                vertex->n[2] = (float)(hd_polygons[effect->polygon - 1].previous.effect.forced_depth * 4);
                vertex->n[3] = -(float)(hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + effect->polygon);
            }
            if (effect->model_effect)
            {
                vertex->n[0] = (float)corner;
                vertex->n[1] = 11;
                vertex->n[3] = -(float)(hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + effect->model_effect);
            }
            // These integer fields fit exactly in the float source attributes
            vertex->extra[0] = (float)((effect->page & 511) | ((uint32)(effect->clut & 32767) << 9));
            vertex->extra[1] = (float)(effect->flags | (effect->space == HD_EFFECT_BILLBOARD ? 256 : 0));
            vertex->extra[2] = (float)(((window & 31) << 3) | ((window & 992) << 6));
            vertex->extra[3] = (float)(((window >> 7) & 248) | ((window >> 4) & 63488));
        }
    }
    vid_mesh_commit(hd_effect_mesh);
    hd_effect_upload_version = hd_effect_version;
    hd_effect_upload_valid = 1;
    return 1;
}

static int hd_effect_attachment(const HD_EFFECT *effect, VECTORF *delta)
{
    const HD_ATTACHMENT *a = 0, *b = 0, *source;
    uint32 index;
    delta->vx = delta->vy = delta->vz = 0;
    if (effect->sprite)
    {
        a = &hd_sprites[effect->sprite - 1].previous.attachment;
        b = &hd_sprites[effect->sprite - 1].current.attachment;
    }
    if (effect->ring)
    {
        a = &hd_rings[effect->ring - 1].previous.attachment;
        b = &hd_rings[effect->ring - 1].current.attachment;
    }
    if (!a)
        return 0;
    source = hd_render_phase <= 0 ? a : b;
    if (!source->owner)
        return 0;
    for (index = 0; index < hd_object_count; ++index)
    {
        const HD_SCENE_OBJECT *object = hd_objects + index;
        if (object->owner_history == source->owner && source->part < object->pose_count)
        {
            VECTORF reference = source->reference;
            RotTransf(hd_poses + object->pose_offset + source->part, &source->point, delta);
            if (a->owner == b->owner && a->part == b->part)
            {
                reference.vx = a->reference.vx + (b->reference.vx - a->reference.vx) * hd_render_phase;
                reference.vy = a->reference.vy + (b->reference.vy - a->reference.vy) * hd_render_phase;
                reference.vz = a->reference.vz + (b->reference.vz - a->reference.vz) * hd_render_phase;
            }
            delta->vx -= reference.vx;
            delta->vy -= reference.vy;
            delta->vz -= reference.vz;
            return 1;
        }
    }
    return 0;
}

typedef struct HD_EFFECT_DRAW
{
    float values[11][4];
    uint32 pass, secondary;
    int overlay;
} HD_EFFECT_DRAW;

static void hd_effect_parameters(const HD_EFFECT_DRAW *draw, uint32 pass)
{
    static const float alpha[5] = {1, 0.5f, 1, 1, 0.25f};
    float values[11][4];
    memcpy(values, draw->values, sizeof(values));
    values[1][3] = alpha[pass];
    values[2][0] = (float)(pass + 1);
    vid_uniforms(&values[0][0], 11);
}

static int hd_effect_pass(const HD_EFFECT_DRAW *draw, uint32 pass)
{
    static const uint32 blends[5] = {VID_BLEND_NONE, VID_BLEND_ALPHA, VID_BLEND_ADD, VID_BLEND_SUBTRACT, VID_BLEND_ADD};
    VID_MATERIAL **slot = &hd_effect_materials[pass][draw->overlay];
    if (!*slot)
    {
        VID_MATERIAL_DATA *data;
        *slot = vid_mat_create();
        if (!*slot)
            return 0;
        data = vid_mat_data(*slot);
        data->shader = hd_effect_shader;
        data->texture = hd_vram;
        data->aux[0] = hd_sprite_texture;
        data->aux[1] = hd_line_texture;
        hd_effect_sprite_bindings[pass][draw->overlay] = hd_sprite_texture;
        hd_effect_line_bindings[pass][draw->overlay] = hd_line_texture;
        data->blend = blends[pass];
        data->depth_test = 1;
        data->depth_write = !pass && !draw->overlay;
        vid_mat_commit(*slot);
    }
    else if (hd_effect_sprite_bindings[pass][draw->overlay] != hd_sprite_texture || hd_effect_line_bindings[pass][draw->overlay] != hd_line_texture)
    {
        vid_mat_data(*slot)->aux[0] = hd_sprite_texture;
        vid_mat_data(*slot)->aux[1] = hd_line_texture;
        hd_effect_sprite_bindings[pass][draw->overlay] = hd_sprite_texture;
        hd_effect_line_bindings[pass][draw->overlay] = hd_line_texture;
        vid_mat_commit(*slot);
    }
    hd_effect_parameters(draw, pass);
    vid_pass(*slot);
    return 1;
}

static int hd_effect_prepare(uint32 index, HD_EFFECT_DRAW *draw)
{
    const HD_EFFECT *effect = &hd_effects[index].effect;
    uint32 flags = effect->flags, page = effect->page, forced = effect->forced_depth;
    int semi, textured;
    VECTORF delta;
    if (effect->strip)
    {
        const HD_STRIP_DRAW *draw = hd_strips + effect->strip - 1;
        const HD_TRAIL_SEGMENT *sample = hd_render_phase <= 0 ? &draw->previous : &draw->current;
        flags = HD_TEXTURED | HD_FILTERED | ((sample->color >> 23) & 6) | sample->image.texel_flags;
        page = sample->image.page;
    }
    if (effect->beam)
    {
        const HD_BEAM_SAMPLE *sample = hd_beams + effect->beam - 1;
        page = hd_render_phase <= 0 ? sample->previous_page : sample->current_page;
    }
    if (effect->ribbon)
    {
        const HD_RIBBON_SAMPLE *sample = hd_ribbons + effect->ribbon - 1;
        page = hd_render_phase <= 0 ? sample->previous_config.page : sample->current_config.page;
    }
    if (effect->ring)
    {
        const HD_RING_PAIR *pair = hd_rings + effect->ring - 1;
        const HD_RING_PRIMITIVE *sample = hd_render_phase <= 0 ? &pair->previous : &pair->current;
        page = sample->page;
        flags = sample->flags;
    }
    if (effect->polygon)
    {
        const HD_POLYGON_PAIR *pair = hd_polygons + effect->polygon - 1;
        const HD_POLYGON_SAMPLE *state = hd_render_phase <= 0 ? &pair->previous : &pair->current;
        const HD_EFFECT *sample = &state->effect;
        flags = sample->flags;
        page = sample->page;
        forced = sample->forced_depth;
        if (state->reject_zero_depth && !forced)
        {
            MATRIXF view;
            uint32 row, column;
            for (row = 0; row < 3; ++row)
            {
                for (column = 0; column < 3; ++column) view.m[row][column] = hd_draw[row + 3][column];
                view.t[row] = hd_draw[row + 3][3];
            }
            if (hd_polygon_depth(pair, hd_render_phase, &view) == 0) return 0;
        }
    }
    semi = (flags & HD_SEMITRANSPARENT) != 0;
    if (effect->model_effect)
    {
        const HD_MODEL_EFFECT_ENTRY *entry = hd_model_effects + effect->model_effect - 1;
        const HD_MODEL_EFFECT *source = hd_render_phase <= 0 ? &entry->previous : &entry->current;
        page = (source->image.page & ~0x60) | (source->projection ? 0x40 : 0x20);
        flags |= source->image.texel_flags;
    }
    textured = (flags & HD_TEXTURED) != 0;
    memset(draw, 0, sizeof(*draw));
    draw->pass = semi && !textured ? 1 + ((page >> 5) & 3) : 0;
    draw->secondary = semi && textured ? 1 + ((page >> 5) & 3) : 0;
    if (draw->secondary && (flags & (HD_OPAQUE_TEXELS_ONLY | HD_STP_TEXELS_ONLY)) == HD_STP_TEXELS_ONLY)
    {
        draw->pass = draw->secondary;
        draw->secondary = 0;
    }
    else if (draw->secondary && (flags & (HD_OPAQUE_TEXELS_ONLY | HD_STP_TEXELS_ONLY)) == HD_OPAQUE_TEXELS_ONLY)
        draw->secondary = 0;
    draw->overlay = forced != 0;
    memcpy(draw->values[0], hd_draw[6], sizeof(draw->values[0]));
    draw->values[1][0] = hd_draw[7][2];
    draw->values[1][1] = hd_draw[7][3];
    draw->values[2][1] = hd_draw[6][1];
    draw->values[2][2] = hd_render_phase;
    memcpy(draw->values + 3, hd_draw + 3, 3 * sizeof(draw->values[0]));
    draw->values[6][0] = (float)(hd_line_count + hd_strip_count + hd_beam_count + hd_ribbon_count + hd_ring_count + hd_polygon_count + hd_model_effect_count);
    // History-backed primitives read their own region from descriptor textures
    if (!effect->sprite && !effect->line && !effect->strip && !effect->beam && !effect->ribbon && !effect->ring && !effect->polygon && !effect->model_effect && (flags & HD_FILTERED))
    {
        draw->values[9][0] = effect->texture_region.x; draw->values[9][1] = effect->texture_region.y;
        draw->values[9][2] = effect->texture_region.width; draw->values[9][3] = effect->texture_region.height;
    }
    hd_effect_attachment(effect, &delta);
    draw->values[10][0] = hd_draw[3][0] * delta.vx + hd_draw[3][1] * delta.vy + hd_draw[3][2] * delta.vz;
    draw->values[10][1] = hd_draw[4][0] * delta.vx + hd_draw[4][1] * delta.vy + hd_draw[4][2] * delta.vz;
    draw->values[10][2] = hd_draw[5][0] * delta.vx + hd_draw[5][1] * delta.vy + hd_draw[5][2] * delta.vz;
    return 1;
}

static int hd_effect_same_draw(const HD_EFFECT_DRAW *a, const HD_EFFECT_DRAW *b)
{
    return !a->secondary && !b->secondary && a->pass == b->pass && a->overlay == b->overlay && !memcmp(a->values, b->values, sizeof(a->values));
}

static int hd_effect_submit(const HD_EFFECT_DRAW *draw, uint32 first, uint32 count)
{
    if (!hd_effect_pass(draw, draw->pass) || !vid_submit(hd_effect_mesh, hd_scene_indices, first, count))
        return 0;
    return !draw->secondary || (hd_effect_pass(draw, draw->secondary) && vid_submit(hd_effect_mesh, hd_scene_indices, first, count));
}

static int hd_effect_range(const HD_EFFECT_DRAW *draw, uint32 first, uint32 count, uint32 *active_pass)
{
    uint32 part, count_passes = draw->secondary ? 2 : 1;
    uint32 passes[2] = {draw->pass, draw->secondary};
    for (part = 0; part < count_passes; ++part)
    {
        uint32 pass = passes[part], key = (pass + 1) * 2 + draw->overlay;
        if (*active_pass != key)
        {
            if (!hd_effect_pass(draw, pass)) return 0;
            *active_pass = key;
        }
        else
            hd_effect_parameters(draw, pass);
        if (!vid_submit(hd_effect_mesh, hd_scene_indices, first, count)) return 0;
    }
    return 1;
}

void hd_frame_camera_reset(HD_FRAME *frame)
{
    frame->camera_logic_valid = 0;
    frame->camera_history.pose.initialized = 0;
    frame->camera_captured = 0;
    frame->cached = 0;
}

void hd_frame_camera_tick(HD_FRAME *frame, const HD_CAMERA *camera, uint32 tick)
{
    int reset;
    if (frame->camera_logic_valid && frame->camera_logic_epoch == hd_history_epoch && frame->camera_logic_tick == tick)
        return;
    reset = !frame->camera_logic_valid || frame->camera_logic_epoch != hd_history_epoch || frame->camera_logic_tick + 1 != tick;
    reset |= !frame->camera_logic_history.pose.initialized || frame->camera_logic_history.current.mode != camera->mode || frame->camera_logic_history.current.near_depth != camera->near_depth || frame->camera_logic_history.current.far_depth != camera->far_depth;
    hd_camera_capture(&frame->camera_logic_history, camera, reset);
    frame->camera_logic_seeded = reset;
    frame->camera_logic_tick = tick;
    frame->camera_logic_epoch = hd_history_epoch;
    frame->camera_logic_valid = 1;
}

void hd_scene_capture_begin(const HD_CAMERA *camera)
{
    sint32 width = 0, height = 0;
    ++hd_capture_serial;
    hd_scene_clear();
    hd_capture_camera = *camera;
    if (hd_current_frame && hd_current_frame->selected)
    {
        HD_FRAME *frame = hd_current_frame;
        int reset = frame->camera_epoch != hd_history_epoch || frame->camera_serial + 1 != hd_capture_serial;
        if (frame->camera_logic_valid && frame->camera_logic_epoch == hd_history_epoch && hd_logic_tick_valid && frame->camera_logic_tick == hd_logic_tick)
        {
            // Replay the same immutable logic samples as the animated objects
            frame->camera_history = frame->camera_logic_history;
        }
        else
            hd_camera_capture(&frame->camera_history, camera, reset);
        frame->camera_epoch = hd_history_epoch;
        frame->camera_serial = hd_capture_serial;
        frame->camera_revision = hd_scene_revision;
        frame->camera_captured = 1;
    }
    {
        VECTORF translation = {-camera->view.t[0], -camera->view.t[1], -camera->view.t[2]}, world;
        IdentityMatrixf(&hd_capture_inverse);
        TransposeMatrixf(&camera->view, &hd_capture_inverse);
        ApplyMatrixf(&hd_capture_inverse, &translation, &world);
        TransMatrixf(&hd_capture_inverse, &world);
    }
    vid_resize(&width, &height);
    hd_capture_aspect = width > 0 && height > 0 ? (float)width / height : 4.0f / 3;
    hd_collecting = 1;
}

void hd_scene_capture_end(void)
{
    hd_collecting = 0;
}

int hd_scene_collecting(void)
{
    return hd_collecting;
}

MATRIXF *hd_scene_pose_buffer(uint32 count)
{
    if (!hd_reserve((void **)&hd_pose_scratch, &hd_pose_scratch_capacity, count, sizeof(*hd_pose_scratch)))
        return 0;
    return hd_pose_scratch;
}

void hd_model_tick_begin(uint32 tick)
{
    hd_logic_tick = tick;
    hd_logic_tick_valid = 1;
}

static HD_MODEL_HISTORY *hd_model_history_storage(HD_MODEL_HISTORY **owner, uint32 count)
{
    HD_MODEL_HISTORY *history = *owner;
    if (!history)
    {
        history = (HD_MODEL_HISTORY *)calloc(1, sizeof(*history));
        if (!history)
            return 0;
        *owner = history;
    }
    if (count + 1 > history->capacity)
    {
        HD_MATRIX_HISTORY *replacement;
        if (count + 1 > (size_t)-1 / sizeof(*history->matrices))
            return 0;
        replacement = (HD_MATRIX_HISTORY *)realloc(history->matrices, (count + 1) * sizeof(*history->matrices));
        if (!replacement)
            return 0;
        history->matrices = replacement;
        history->capacity = count + 1;
    }
    return history;
}

int hd_model_history_tick(HD_MODEL_HISTORY **owner, const HD_OBJECT_STATE *state, const MATRIXF *poses, uint32 count)
{
    HD_MODEL_HISTORY *history;
    uint32 index;
    int seed;
    if (!hd_logic_tick_valid || hd_active || hd_collecting || !state->bank || (count && !poses) || count == 0xFFFFFFFF)
        return 0;
    history = hd_model_history_storage(owner, count);
    if (!history)
        return 0;
    seed = !history->initialized || !history->logic_valid || history->epoch != hd_history_epoch || history->count != count + 1 || history->bank != state->bank->identity || history->model != state->model;
    if (!seed && history->logic_tick == hd_logic_tick)
        return 1;
    if (history->logic_tick + 1 != hd_logic_tick)
        seed = 1;
    history->previous_radius = seed ? state->radius : history->current_radius;
    history->current_radius = state->radius;
    for (index = 0; index <= count; ++index)
    {
        const MATRIXF *matrix = index ? poses + index - 1 : &state->transform;
        if (seed)
            hd_matrix_seed(history->matrices + index, matrix);
        else
            hd_matrix_capture(history->matrices + index, matrix);
    }
    history->count = count + 1;
    history->epoch = hd_history_epoch;
    history->bank = state->bank->identity;
    history->model = state->model;
    history->initialized = history->logic_valid = 1;
    history->logic_tick = hd_logic_tick;
    history->scene_revision = ~hd_scene_revision;
    return 1;
}

int hd_model_history_point(const HD_MODEL_HISTORY *history, uint32 part, const VECTORF *local, VECTORF *world)
{
    if (!history || !history->initialized || history->epoch != hd_history_epoch || part + 1 >= history->count)
        return 0;
    RotTransf(&history->matrices[part + 1].current, local, world);
    return 1;
}

int hd_model_history_copy_poses(const HD_MODEL_HISTORY *history, MATRIXF *poses, uint32 count)
{
    uint32 index;
    if (!history || history->count != count + 1 || (count && !poses))
        return 0;
    for (index = 0; index < count; ++index)
        poses[index] = history->matrices[index + 1].current;
    return 1;
}

int hd_model_history_capture(HD_MODEL_HISTORY **owner, HD_OBJECT_STATE *state, const MATRIXF *poses, uint32 count)
{
    HD_MODEL_HISTORY *history;
    uint32 index;
    int seed, logic_ready;
    if (!hd_collecting || hd_active || !state->bank || (count && !poses) || count == 0xFFFFFFFF)
        return 0;
    history = hd_model_history_storage(owner, count);
    if (!history)
        return 0;
    seed = !history->initialized || history->epoch != hd_history_epoch || history->count != count + 1 || history->bank != state->bank->identity || history->model != state->model;
    logic_ready = !seed && history->logic_valid && hd_logic_tick_valid && history->logic_tick == hd_logic_tick;
    if (logic_ready)
    {
        state->transform = history->matrices[0].current;
        state->radius = history->current_radius;
    }
    else if (seed || history->serial != hd_capture_serial)
    {
        history->logic_valid = 0;
        if (history->serial + 1 != hd_capture_serial)
            seed = 1;
        history->previous_radius = seed ? state->radius : history->current_radius;
        history->current_radius = state->radius;
        for (index = 0; index <= count; ++index)
        {
            const MATRIXF *matrix = index ? poses + index - 1 : &state->transform;
            if (seed)
                hd_matrix_seed(history->matrices + index, matrix);
            else
                hd_matrix_capture(history->matrices + index, matrix);
        }
        history->scene_revision = ~hd_scene_revision;
    }
    if (history->scene_revision != hd_scene_revision)
    {
        if (count > 0xFFFFFFFF - hd_pose_count || !hd_reserve((void **)&hd_poses, &hd_pose_capacity, hd_pose_count + count, sizeof(*hd_poses)) || !hd_reserve((void **)&hd_pose_history, &hd_pose_history_capacity, hd_pose_count + count, sizeof(*hd_pose_history)))
            return 0;
        history->scene_offset = hd_pose_count;
        for (index = 0; index < count; ++index)
        {
            hd_pose_history[hd_pose_count + index] = history->matrices[index + 1];
            hd_poses[hd_pose_count + index] = history->matrices[index + 1].current;
        }
        hd_pose_count += count;
        history->scene_revision = hd_scene_revision;
    }
    history->count = count + 1;
    history->epoch = hd_history_epoch;
    history->serial = hd_capture_serial;
    history->bank = state->bank->identity;
    history->model = state->model;
    history->initialized = 1;
    state->history = history;
    return 1;
}

void hd_model_history_reset(HD_MODEL_HISTORY *history)
{
    if (history)
        history->initialized = 0;
}

void hd_model_history_free(HD_MODEL_HISTORY *history)
{
    if (history)
        free(history->matrices);
    free(history);
}

static int hd_scene_hierarchy(HD_SCENE_OBJECT *object, const HD_OBJECT_STATE *state)
{
    uint32 index, earlier, count = object->pose_count;
    HD_POSE_SAMPLE *samples;
    object->hierarchy_count = 0;
    object->hierarchy_source_count = 0;
    if (!object->interpolate || !state->pose_link_count)
        return 1;
    if (!state->pose_links || state->pose_link_count > count)
        return 0;
    for (index = 0; index < state->pose_link_count; ++index)
    {
        const HD_POSE_LINK *link = state->pose_links + index;
        int found = link->parent == 0xFFFFFFFF;
        if (link->part >= count)
            return 0;
        for (earlier = 0; earlier < index; ++earlier)
        {
            if (state->pose_links[earlier].part == link->part)
                return 0;
            if (state->pose_links[earlier].part == link->parent)
                found = 1;
        }
        if (!found)
            return 0;
    }
    if (hd_object_count > 1)
    {
        const HD_SCENE_OBJECT *previous = object - 1;
        if (previous->pose_offset == object->pose_offset && previous->hierarchy_count == count && previous->hierarchy_source_count == state->pose_link_count)
        {
            samples = hd_skeletal_samples + previous->hierarchy_offset;
            for (index = 0; index < state->pose_link_count; ++index)
                if (samples[index].part != state->pose_links[index].part || samples[index].parent != state->pose_links[index].parent)
                    break;
            if (index == state->pose_link_count)
            {
                object->hierarchy_offset = previous->hierarchy_offset;
                object->hierarchy_count = count;
                object->hierarchy_source_count = state->pose_link_count;
                return 1;
            }
        }
    }
    if (count > 0xFFFFFFFF - hd_skeletal_count || !hd_reserve((void **)&hd_skeletal_samples, &hd_skeletal_capacity, hd_skeletal_count + count, sizeof(*hd_skeletal_samples)))
        return 0;
    samples = hd_skeletal_samples + hd_skeletal_count;
    for (index = 0; index < state->pose_link_count; ++index)
    {
        samples[index].part = state->pose_links[index].part;
        samples[index].parent = state->pose_links[index].parent;
    }
    earlier = state->pose_link_count;
    for (index = 0; index < count; ++index)
    {
        uint32 link;
        for (link = 0; link < state->pose_link_count; ++link)
            if (samples[link].part == index)
                break;
        if (link == state->pose_link_count)
        {
            samples[earlier].part = index;
            samples[earlier++].parent = 0xFFFFFFFF;
        }
    }
    for (index = 0; index < count; ++index)
    {
        const HD_MATRIX_HISTORY *world = hd_pose_history + object->pose_offset + samples[index].part;
        const HD_MATRIX_HISTORY *parent = samples[index].parent == 0xFFFFFFFF ? &object->transform : hd_pose_history + object->pose_offset + samples[index].parent;
        MATRIXF previous, current;
        /* A singular authored scale cannot be factored; retain the world-space pair */
        if (!hd_matrix_relative(&parent->previous, &world->previous, &previous) || !hd_matrix_relative(&parent->current, &world->current, &current))
            return 1;
        hd_matrix_seed(&samples[index].local, &previous);
        hd_matrix_capture(&samples[index].local, &current);
    }
    object->hierarchy_offset = hd_skeletal_count;
    object->hierarchy_count = count;
    object->hierarchy_source_count = state->pose_link_count;
    hd_skeletal_count += count;
    return 1;
}

int hd_scene_add(const HD_OBJECT_STATE *state, const MATRIXF *poses, uint32 count)
{
    HD_SCENE_OBJECT *object;
    HD_BANK *bank = state->bank;
    const HD_MODEL_RANGE *model;
    uint32 i, slot;
    int shared;
    if (hd_active || !bank || !bank->sealed || state->model >= bank->model_count || (count && !poses))
        return 0;
    model = bank->models + state->model;
    for (i = 0; count && i < model->batch_count; ++i)
    {
        const HD_POSE_BATCH *batch = bank->batches + model->first_batch + i;
        for (slot = 0; slot < batch->part_count; ++slot)
            if (batch->parts[slot] >= count)
                return 0;
    }
    shared = state->history && state->history->count == count + 1 && state->history->scene_revision == hd_scene_revision;
    if ((!shared && count > 0xFFFFFFFF - hd_pose_count) || hd_object_count == 0xFFFFFFFF)
        return 0;
    if (!hd_reserve((void **)&hd_objects, &hd_object_capacity, hd_object_count + 1, sizeof(*hd_objects)))
        return 0;
    if (!shared && (!hd_reserve((void **)&hd_poses, &hd_pose_capacity, hd_pose_count + count, sizeof(*hd_poses)) || !hd_reserve((void **)&hd_pose_history, &hd_pose_history_capacity, hd_pose_count + count, sizeof(*hd_pose_history))))
        return 0;
    object = hd_objects + hd_object_count++;
    object->state = *state;
    object->owner_history = state->history;
    object->state.history = 0;
    object->state.pose_links = 0;
    object->state.pose_link_count = 0;
    object->interpolate = state->history && state->history->count == count + 1;
    object->radius = state->radius;
    if (object->interpolate)
    {
        if (object->radius <= 0 || state->history->previous_radius <= 0)
            object->radius = 0;
        else if (state->history->previous_radius > object->radius)
            object->radius = state->history->previous_radius;
    }
    if (object->interpolate)
        object->transform = state->history->matrices[0];
    else
        hd_matrix_seed(&object->transform, &state->transform);
    object->pose_offset = shared ? state->history->scene_offset : hd_pose_count;
    object->pose_count = count;
    object->order = hd_capture_order;
    hd_capture_order += model->face_count;
    for (i = 0; !shared && i < count; ++i)
    {
        hd_poses[hd_pose_count + i] = poses[i];
        if (state->history && state->history->count == count + 1)
            hd_pose_history[hd_pose_count + i] = state->history->matrices[i + 1];
        else
        {
            hd_matrix_seed(hd_pose_history + hd_pose_count + i, poses + i);
            hd_pose_history[hd_pose_count + i].initialized = 0;
        }
    }
    if (!shared)
        hd_pose_count += count;
    return hd_scene_hierarchy(object, state);
}

int hd_scene_phase(float phase)
{
    uint32 index;
    if (hd_active || hd_collecting)
        return 0;
    hd_render_phase = phase < 0 ? 0 : (phase > 1 ? 1 : phase);
    for (index = 0; index < hd_object_count; ++index)
        if (hd_objects[index].interpolate)
            hd_matrix_evaluate(&hd_objects[index].transform, phase, &hd_objects[index].state.transform);
    for (index = 0; index < hd_pose_count; ++index)
        if (hd_pose_history[index].initialized)
            hd_matrix_evaluate(hd_pose_history + index, phase, hd_poses + index);
    if (phase > 0 && phase < 1)
        for (index = 0; index < hd_object_count; ++index)
        {
            const HD_SCENE_OBJECT *object = hd_objects + index;
            uint32 joint;
            if (index && object->hierarchy_count && object->hierarchy_offset == hd_objects[index - 1].hierarchy_offset && object->pose_offset == hd_objects[index - 1].pose_offset)
                continue;
            for (joint = 0; joint < object->hierarchy_count; ++joint)
            {
                const HD_POSE_SAMPLE *sample = hd_skeletal_samples + object->hierarchy_offset + joint;
                const MATRIXF *parent = sample->parent == 0xFFFFFFFF ? &object->state.transform : hd_poses + object->pose_offset + sample->parent;
                MATRIXF local;
                hd_matrix_evaluate(&sample->local, phase, &local);
                CompMatrixf(parent, &local, hd_poses + object->pose_offset + sample->part);
            }
        }
    for (index = 0; index < hd_object_count; ++index)
    {
        HD_SCENE_OBJECT *object = hd_objects + index;
        if (!object->interpolate)
            continue;
        if (object->state.lighting.world_directions_valid)
        {
            const MATRIXF *matrix = object->state.model < object->pose_count ? hd_poses + object->pose_offset + object->state.model : &object->state.transform;
            MulMatrix0f(&object->state.lighting.world_directions, matrix, &object->state.lighting.directions);
        }
    }
    return 1;
}

static const MATRIXF *hd_scene_matrix(const HD_SCENE_OBJECT *object, uint32 part)
{
    return object->pose_count ? hd_poses + object->pose_offset + part : &object->state.transform;
}

static float hd_scene_face_depth(const HD_SCENE_OBJECT *object, const HD_POSE_BATCH *batch, const uint16 *data, uint32 count)
{
    static const uint32 perimeter[4] = {0, 1, 3, 2};
    float z[4], side[4], depth = -1.0e30f;
    uint32 corner;
    for (corner = 0; corner < count; ++corner)
    {
        const uint16 *point = data + 8 + corner * 4;
        const MATRIXF *matrix = hd_scene_matrix(object, batch->parts[point[3]]);
        VECTORF local = {(float)(sint16)point[0], (float)(sint16)point[1], (float)(sint16)point[2]}, world;
        RotTransf(matrix, &local, &world);
        z[corner] = hd_draw[5][0] * world.vx + hd_draw[5][1] * world.vy + hd_draw[5][2] * world.vz + hd_draw[5][3];
        side[corner] = object->state.water_y - world.vy;
        if ((!object->state.water_clip || side[corner] >= 0) && z[corner] > depth)
            depth = z[corner];
    }
    if (object->state.water_clip)
        for (corner = 0; corner < count; ++corner)
        {
            uint32 a = count == 4 ? perimeter[corner] : corner;
            uint32 b = count == 4 ? perimeter[(corner + 1) % count] : (corner + 1) % count;
            if ((side[a] < 0) != (side[b] < 0))
            {
                float phase = side[a] / (side[a] - side[b]);
                float intersection = z[a] + (z[b] - z[a]) * phase;
                if (intersection > depth)
                    depth = intersection;
            }
        }
    return depth;
}

static void hd_scene_materials(const HD_SCENE_OBJECT *object)
{
    const HD_OBJECT_STATE *state = &object->state;
    uint32 row, column;
    hd_draw[28][0] = (float)((state->water_clip ? 1 : 0) | (state->reflection ? 2 : 0));
    hd_draw[28][1] = state->water_y;
    hd_draw[28][2] = (float)(state->reflection_page | ((uint32)(state->reflection_clut & 32767) << 9));
    hd_draw[28][3] = (float)state->reflection_uv;
    hd_draw[9][3] = state->reflection_region.x;
    hd_draw[10][3] = state->reflection_region.y;
    hd_draw[11][3] = state->reflection_region.width;
    hd_draw[12][3] = state->reflection_region.height;
    if (state->reflection)
    {
        const MATRIXF *matrix = hd_scene_matrix(object, state->model);
        float eye[3];
        for (row = 0; row < 3; ++row)
        {
            eye[row] = hd_draw[row + 3][3];
            for (column = 0; column < 3; ++column)
                eye[row] += hd_draw[row + 3][column] * state->transform.t[column];
        }
        eye[0] *= 512.0f / 320;
        for (column = 0; column < 3; ++column)
        {
            float local_eye = 0;
            for (row = 0; row < 3; ++row)
            {
                float coefficient = 0;
                uint32 axis;
                for (axis = 0; axis < 3; ++axis)
                    coefficient += hd_draw[row + 3][axis] * matrix->m[axis][column];
                local_eye -= coefficient * eye[row];
                hd_draw[column + 29][row] = matrix->m[column][row];
            }
            hd_draw[column + 29][3] = local_eye;
        }
    }
}

static void hd_scene_parameters(const HD_SCENE_OBJECT *object, uint32 batch_index)
{
    const HD_POSE_BATCH *batch = object->state.bank->batches + batch_index;
    uint32 slot;
    hd_set_lighting(&object->state.lighting);
    hd_scene_materials(object);
    hd_draw[18][2] = 1;
    for (slot = 1; slot < batch->part_count; ++slot)
        hd_matrix_rows(hd_draw + 19 + (slot - 1) * 3, hd_scene_matrix(object, batch->parts[slot]));
    hd_matrix_rows(hd_draw, hd_scene_matrix(object, batch->parts[0]));
}

static int hd_scene_submit(const HD_SCENE_OBJECT *object, uint32 batch_index, VID_INDEX *indices, uint32 first, uint32 count)
{
    hd_scene_parameters(object, batch_index);
    vid_uniforms(&hd_draw[0][0], 32);
    return vid_submit(object->state.bank->mesh, indices, first, count);
}

static int hd_scene_opaque(uint32 first, uint32 *next)
{
    HD_BANK *bank = hd_objects[first].state.bank;
    HD_OPAQUE_GROUP *group;
    uint32 cursor = bank->opaque_group_cursor++, end = first, index, face, batch;
    uint32 face_rows = (bank->face_capacity * 8 + 1023) / 1024;
    uint32 height, used = 0, *indices;
    int changed = 0, texture_changed = 0;
    uint16 *pixels;
    if (bank->batch_count > (0x7FFFFFFF - face_rows * 1024) / 64)
        return 0;
    height = face_rows + (bank->batch_count * 64 + 1023) / 1024;
    if (cursor == bank->opaque_group_count)
    {
        if (!hd_reserve((void **)&bank->opaque_groups, &bank->opaque_group_capacity, cursor + 1, sizeof(*group)))
            return 0;
        memset(bank->opaque_groups + cursor, 0, sizeof(*group));
        ++bank->opaque_group_count;
    }
    group = bank->opaque_groups + cursor;
    if (height > group->height)
    {
        VID_TEXTURE *replacement = vid_tex_create_fmt(1024, height, VID_RGBA16UI);
        if (!replacement)
            return 0;
        vid_tex_free(group->faces);
        group->faces = replacement;
        group->height = height;
        group->face_revision = 0;
        if (group->material)
        {
            vid_mat_data(group->material)->aux[1] = group->faces;
            vid_mat_commit(group->material);
        }
    }
    if (!group->indices)
        group->indices = vid_index_create(bank->index_capacity);
    if (!group->indices)
        return 0;
    hd_bank_commit_materials(bank);
    pixels = (uint16 *)vid_tex_data(group->faces);
    if (group->face_revision != bank->face_revision)
    {
        memcpy(pixels, vid_tex_data(bank->face_materials), face_rows * 1024 * 4 * sizeof(*pixels));
        group->face_revision = bank->face_revision;
        texture_changed = 1;
    }
    indices = vid_index_data(group->indices);
    if (++bank->model_stamp == 0)
    {
        memset(bank->model_marks, 0, bank->model_count * sizeof(*bank->model_marks));
        ++bank->model_stamp;
    }
    hd_draw[7][0] = HD_TEXELS_ALL;
    hd_draw[7][1] = 1;
    hd_draw[8][0] = 1;
    hd_draw[18][3] = 1;
    for (; end < hd_object_count; ++end)
    {
        const HD_SCENE_OBJECT *object = hd_objects + end;
        const HD_MODEL_RANGE *model;
        if (!object->visible)
            continue;
        if (object->state.bank != bank || bank->model_marks[object->state.model] == bank->model_stamp)
            break;
        bank->model_marks[object->state.model] = bank->model_stamp;
        model = bank->models + object->state.model;
        for (batch = model->first_batch; batch < model->first_batch + model->batch_count; ++batch)
        {
            hd_scene_parameters(object, batch);
            for (index = 0; index < 32; ++index)
            {
                uint32 bits[4], component, at = (face_rows * 1024 + batch * 64 + index * 2) * 4;
                uint16 packed[8];
                memcpy(bits, hd_draw[index], sizeof(bits));
                for (component = 0; component < 4; ++component)
                {
                    packed[component] = (uint16)bits[component];
                    packed[component + 4] = (uint16)(bits[component] >> 16);
                }
                if (memcmp(pixels + at, packed, sizeof(packed)))
                {
                    memcpy(pixels + at, packed, sizeof(packed));
                    texture_changed = 1;
                }
            }
        }
        for (face = model->first_face; face < model->first_face + model->face_count; ++face)
        {
            const HD_FACE_RANGE *range = bank->faces + face;
            uint32 selected[6], count = hd_face_indices(selected, range->vertex, range->count);
            if (used + count > group->index_count || memcmp(indices + used, selected, count * sizeof(*selected)))
            {
                memcpy(indices + used, selected, count * sizeof(*selected));
                changed = 1;
            }
            used += count;
        }
    }
    *next = end;
    if (!used)
        return 1;
    if (changed || used != group->index_count)
        vid_index_commit(group->indices, used);
    group->index_count = used;
    if (texture_changed)
        vid_tex_commit(group->faces);
    if (!group->material)
    {
        VID_MATERIAL_DATA *data;
        group->material = vid_mat_create();
        if (!group->material)
            return 0;
        data = vid_mat_data(group->material);
        data->shader = hd_world_shader;
        data->texture = hd_vram;
        data->aux[0] = bank->palette;
        data->aux[1] = group->faces;
        data->depth_test = data->depth_write = 1;
        vid_mat_commit(group->material);
    }
    hd_draw[18][2] = 2;
    hd_draw[31][3] = (float)face_rows;
    vid_uniforms(&hd_draw[0][0], 32);
    vid_pass(group->material);
    return vid_submit(bank->mesh, group->indices, 0, used);
}

static int hd_alpha_after(const HD_SCENE_FACE *left, const HD_SCENE_FACE *right)
{
    return left->bucket < right->bucket || (left->bucket == right->bucket && left->order < right->order);
}

static void hd_alpha_sift(uint32 root, uint32 count)
{
    while (root < count / 2)
    {
        uint32 child = root * 2 + 1;
        HD_SCENE_FACE saved;
        if (child + 1 < count && hd_alpha_after(hd_alpha + child + 1, hd_alpha + child))
            ++child;
        if (!hd_alpha_after(hd_alpha + child, hd_alpha + root))
            return;
        saved = hd_alpha[root];
        hd_alpha[root] = hd_alpha[child];
        hd_alpha[child] = saved;
        root = child;
    }
}

static void hd_alpha_sort(void)
{
    uint32 count = hd_alpha_count, root = count / 2;
    while (root)
        hd_alpha_sift(--root, count);
    while (count > 1)
    {
        HD_SCENE_FACE saved = hd_alpha[0];
        hd_alpha[0] = hd_alpha[--count];
        hd_alpha[count] = saved;
        hd_alpha_sift(0, count);
    }
}

static uint32 hd_effect_bucket(const HD_EFFECT_ITEM *item)
{
    const HD_EFFECT_DEPTH *sort = &item->effect.depth;
    uint32 point, row, column;
    VECTORF delta;
    int changed = hd_effect_attachment(&item->effect, &delta);
    float depth = sort->rule == HD_DEPTH_MIN ? 1.0e30f : (sort->rule == HD_DEPTH_MAX ? -1.0e30f : 0);
    if (item->effect.model_effect)
    {
        HD_MODEL_EFFECT source;
        HD_POLYGON_PAIR pair;
        HD_CAMERA camera = hd_capture_camera;
        for (row = 0; row < 3; ++row)
        {
            for (column = 0; column < 3; ++column) camera.view.m[row][column] = hd_draw[row + 3][column];
            camera.view.t[row] = hd_draw[row + 3][3];
        }
        hd_model_effect_sample(hd_model_effects + item->effect.model_effect - 1, hd_render_phase, &source);
        if (!hd_model_effect_geometry(&source, &camera, &pair.current) || !pair.current.present) return 0;
        pair.previous = pair.current;
        depth = hd_polygon_depth(&pair, 1, &camera.view);
        return ((uint32)(sint32)depth & 0x3FFC) >> 2;
    }
    if (item->effect.polygon)
    {
        const HD_POLYGON_PAIR *pair = hd_polygons + item->effect.polygon - 1;
        const HD_POLYGON_SAMPLE *sample = hd_render_phase <= 0 ? &pair->previous : &pair->current;
        MATRIXF view;
        if (sample->effect.forced_depth) return sample->effect.forced_depth;
        for (row = 0; row < 3; ++row)
        {
            for (column = 0; column < 3; ++column) view.m[row][column] = hd_draw[row + 3][column];
            view.t[row] = hd_draw[row + 3][3];
        }
        depth = hd_polygon_depth(pair, hd_render_phase, &view);
        return (((uint32)(sint32)depth - sample->bias) & 0x3FFC) >> 2;
    }
    if (item->effect.ring && !item->effect.forced_depth)
    {
        const HD_RING_PAIR *pair = hd_rings + item->effect.ring - 1;
        const VECTORF *a = &pair->previous.center, *b = &pair->current.center;
        VECTORF world = {a->vx + (b->vx - a->vx) * hd_render_phase, a->vy + (b->vy - a->vy) * hd_render_phase, a->vz + (b->vz - a->vz) * hd_render_phase};
        world.vx += delta.vx; world.vy += delta.vy; world.vz += delta.vz;
        depth = hd_draw[5][0] * world.vx + hd_draw[5][1] * world.vy + hd_draw[5][2] * world.vz + hd_draw[5][3];
        return ((uint32)(sint32)depth & 0x3FFC) >> 2;
    }
    if (item->effect.ribbon && !item->effect.forced_depth)
    {
        const HD_RIBBON_SAMPLE *sample = hd_ribbons + item->effect.ribbon - 1;
        VECTORF points[2];
        MATRIXF view;
        for (row = 0; row < 3; ++row)
        {
            for (column = 0; column < 3; ++column)
                view.m[row][column] = hd_draw[row + 3][column];
            view.t[row] = hd_draw[row + 3][3];
        }
        for (point = 0; point < 2; ++point)
        {
            const HD_RIBBON_SAMPLE *endpoint = sample + point - 1;
            const VECTORF *a = &endpoint->previous.point, *b = &endpoint->current.point;
            VECTORF world = {a->vx + (b->vx - a->vx) * hd_render_phase, a->vy + (b->vy - a->vy) * hd_render_phase, a->vz + (b->vz - a->vz) * hd_render_phase};
            RotTransf(&view, &world, points + point);
        }
        if (!hd_effect_clip(points, points + 1, sample->near_depth, sample->far_depth))
            return 0;
        depth = points[0].vz < points[1].vz ? points[0].vz : points[1].vz;
        return ((uint32)(sint32)depth & 0x3FFC) >> 2;
    }
    if (item->effect.beam && !item->effect.forced_depth)
    {
        const HD_BEAM_SAMPLE *sample = hd_beams + item->effect.beam - 1;
        const VECTORF *a = &sample->previous.point, *b = &sample->current.point;
        uint32 bias = hd_render_phase <= 0 ? sample->previous_bias : sample->current_bias;
        VECTORF world = {a->vx + (b->vx - a->vx) * hd_render_phase, a->vy + (b->vy - a->vy) * hd_render_phase, a->vz + (b->vz - a->vz) * hd_render_phase};
        depth = hd_draw[5][0] * world.vx + hd_draw[5][1] * world.vy + hd_draw[5][2] * world.vz + hd_draw[5][3];
        return (((uint32)(sint32)depth - bias) & 0x3FFC) >> 2;
    }
    if (item->effect.strip && !item->effect.forced_depth)
    {
        const HD_STRIP_DRAW *draw = hd_strips + item->effect.strip - 1;
        const HD_TRAIL_EDGE *edge = &draw->edge;
        uint32 bias = hd_render_phase <= 0 ? draw->previous.bias : draw->current.bias;
        const VECTORF *a = edge->previous + 1, *b = edge->current + 1;
        VECTORF world = {a->vx + (b->vx - a->vx) * hd_render_phase, a->vy + (b->vy - a->vy) * hd_render_phase, a->vz + (b->vz - a->vz) * hd_render_phase};
        depth = hd_draw[5][0] * world.vx + hd_draw[5][1] * world.vy + hd_draw[5][2] * world.vz + hd_draw[5][3];
        return (((uint32)(sint32)depth - bias) & 0x3FFC) >> 2;
    }
    if (item->effect.line && !item->effect.forced_depth)
    {
        MATRIXF view;
        for (row = 0; row < 3; ++row)
        {
            for (column = 0; column < 3; ++column)
                view.m[row][column] = hd_draw[row + 3][column];
            view.t[row] = hd_draw[row + 3][3];
        }
        depth = hd_line_depth(hd_lines + item->effect.line - 1, hd_render_phase, &view);
        return (((uint32)(sint32)depth - item->bias) & 0x3FFC) >> 2;
    }
    if (item->effect.forced_depth || !sort->count)
        return item->bucket;
    for (row = 0; row < 3; ++row)
    {
        for (column = 0; column < 3; ++column)
            changed |= hd_draw[row + 3][column] != hd_capture_camera.view.m[row][column];
        changed |= hd_draw[row + 3][3] != hd_capture_camera.view.t[row];
    }
    // Reuse the original capture bucket when the view has not changed
    if (!changed && (!item->effect.sprite || hd_render_phase >= 1))
        return item->bucket;
    for (point = 0; point < sort->count; ++point)
    {
        VECTORF interpolated;
        const VECTORF *world = sort->points + point;
        if (item->effect.sprite)
        {
            const HD_SPRITE_PAIR *sprite = hd_sprites + item->effect.sprite - 1;
            interpolated.vx = sprite->previous.pos.vx + (sprite->current.pos.vx - sprite->previous.pos.vx) * hd_render_phase;
            interpolated.vy = sprite->previous.pos.vy + (sprite->current.pos.vy - sprite->previous.pos.vy) * hd_render_phase;
            interpolated.vz = sprite->previous.pos.vz + (sprite->current.pos.vz - sprite->previous.pos.vz) * hd_render_phase;
            world = &interpolated;
        }
        float z = hd_draw[5][0] * (world->vx + delta.vx) + hd_draw[5][1] * (world->vy + delta.vy) + hd_draw[5][2] * (world->vz + delta.vz) + hd_draw[5][3];
        if (sort->clamp)
            z = z < 0 ? 0 : (z > 65535 ? 65535 : z);
        if (sort->rule == HD_DEPTH_MEAN)
            depth += z;
        else if ((sort->rule == HD_DEPTH_MIN && z < depth) || (sort->rule == HD_DEPTH_MAX && z > depth))
            depth = z;
    }
    if (sort->rule == HD_DEPTH_MEAN)
        depth /= sort->count;
    depth += sort->offset;
    if (sort->clamp && (depth < 0 || depth > 16380))
        depth = 16380;
    return (((uint32)(sint32)depth - item->bias) & 0x3FFC) >> 2;
}

static int hd_scene_collect_alpha(void)
{
    uint32 index;
    hd_alpha_count = 0;
    for (index = 0; index < hd_object_count; ++index)
    {
        const HD_SCENE_OBJECT *object = hd_objects + index;
        if (!object->visible)
            continue;
        HD_BANK *bank = object->state.bank;
        const HD_MODEL_RANGE *model = bank->models + object->state.model;
        const uint16 *materials = (const uint16 *)vid_tex_data(bank->face_materials);
        uint32 face;
        for (face = model->first_face; face < model->first_face + model->face_count; ++face)
        {
            const HD_FACE_RANGE *range = bank->faces + face;
            const HD_POSE_BATCH *batch = bank->batches + range->batch;
            const uint16 *data = materials + face * 32;
            uint32 mask = object->state.lighting.draw_mask;
            uint32 flags = (data[27] & ((uint32)((sint32)mask >> 16))) | (mask & 65535);
            uint32 colored = (flags & 2048) || (flags & 12) == 12;
            uint32 textured = flags & (colored ? 1 : 3);
            uint32 page = data[4] | ((flags & 384) >> 2);
            float depth = -1.0e30f;
            uint32 sort_depth;
            HD_SCENE_FACE *entry;
            if (!(flags & 192) || !(flags & 64))
                continue;
            if (textured && !(flags & 1) && object->state.reflection)
                page = object->state.reflection_page | ((flags & 384) >> 2);
            depth = hd_scene_face_depth(object, batch, data, range->count);
            if (depth == -1.0e30f)
                continue;
            if (hd_alpha_count == 0xFFFFFFFF || !hd_reserve((void **)&hd_alpha, &hd_alpha_capacity, hd_alpha_count + 1, sizeof(*hd_alpha)))
                return 0;
            entry = hd_alpha + hd_alpha_count++;
            entry->object = index;
            entry->face = face;
            sort_depth = depth <= 0 ? 0 : (depth >= 65535 ? 65535 : (uint32)depth);
            if (flags & 0x6000)
                sort_depth += flags & 0x2000 ? object->state.near_bias : object->state.far_bias;
            entry->bucket = sort_depth < 16384 ? sort_depth >> 2 : 4095;
            entry->order = object->order + face - model->first_face;
            entry->pass = 1 + (textured ? ((page >> 5) & 3) : ((flags >> 7) & 3));
        }
    }
    for (index = 0; index < hd_effect_count; ++index)
    {
        HD_SCENE_FACE *entry;
        if (hd_alpha_count == 0xFFFFFFFF || !hd_reserve((void **)&hd_alpha, &hd_alpha_capacity, hd_alpha_count + 1, sizeof(*hd_alpha)))
            return 0;
        entry = hd_alpha + hd_alpha_count++;
        entry->object = 0xFFFFFFFF;
        entry->face = index;
        entry->bucket = hd_effect_bucket(hd_effects + index);
        entry->order = hd_effects[index].order;
        entry->pass = 0;
    }
    if (hd_alpha_count > 1)
        hd_alpha_sort();
    return 1;
}

int hd_scene_draw(void)
{
    uint32 i, used = 0, *indices;
    uint32 active_pass = 0;
    HD_BANK *pass_bank = 0;
    int changed = 0;
    if (!hd_active || hd_scene_submitted)
        return 0;
    hd_scene_submitted = 1;
#if defined(_DEBUG)
    hd_debug_scene_pending = 1;
#endif
    for (i = 0; i < hd_object_count; ++i)
    {
        HD_SCENE_OBJECT *object = hd_objects + i;
        VECTORF center = {object->state.transform.t[0], object->state.transform.t[1], object->state.transform.t[2]};
        object->visible = object->pose_count || object->radius <= 0 || hd_sphere_visible(&hd_draw_camera, hd_draw_aspect, &center, object->radius);
        object->state.bank->opaque_group_cursor = 0;
    }
    if (!hd_effect_upload() || !hd_model_effect_upload() || !hd_scene_collect_alpha() || hd_alpha_count > 0xFFFFFFFF / 6)
        return 0;
    if (hd_alpha_count * 6 > hd_scene_index_capacity)
    {
        uint32 capacity = hd_alpha_count * 6;
        VID_INDEX *replacement = vid_index_create(capacity);
        if (!replacement)
            return 0;
        vid_index_free(hd_scene_indices);
        hd_scene_indices = replacement;
        hd_scene_index_capacity = capacity;
        hd_scene_index_count = 0;
        changed = 1;
    }
    indices = vid_index_data(hd_scene_indices);
    for (i = 0; i < hd_alpha_count; ++i)
    {
        const HD_SCENE_FACE *entry = hd_alpha + i;
        uint32 selected[6], count;
        if (entry->object == 0xFFFFFFFF)
            count = hd_face_indices(selected, entry->face * 4, 4);
        else
        {
            const HD_FACE_RANGE *face = hd_objects[entry->object].state.bank->faces + entry->face;
            count = hd_face_indices(selected, face->vertex, face->count);
        }
        if (used + count > hd_scene_index_count || memcmp(indices + used, selected, count * sizeof(*selected)))
        {
            memcpy(indices + used, selected, count * sizeof(*selected));
            changed = 1;
        }
        used += count;
    }
    if (used && (changed || used != hd_scene_index_count))
        vid_index_commit(hd_scene_indices, used);
    hd_scene_index_count = used;
    for (i = 0; i < hd_object_count;)
    {
        const HD_SCENE_OBJECT *object = hd_objects + i;
        if (!object->visible)
        {
            ++i;
            continue;
        }
        if (!hd_scene_opaque(i, &i))
            return 0;
    }
    used = 0;
    for (i = 0; i < hd_alpha_count;)
    {
        const HD_SCENE_FACE *entry = hd_alpha + i;
        if (entry->object == 0xFFFFFFFF)
        {
            HD_EFFECT_DRAW draw;
            uint32 first = used;
            used += 6;
            ++i;
            if (!hd_effect_prepare(entry->face, &draw))
                continue;
            // Merge only adjacent commands and retain each mixed-STP pass boundary
            while (i < hd_alpha_count && hd_alpha[i].object == 0xFFFFFFFF)
            {
                HD_EFFECT_DRAW next;
                if (!hd_effect_prepare(hd_alpha[i].face, &next) || !hd_effect_same_draw(&draw, &next))
                    break;
                used += 6;
                ++i;
            }
            if (pass_bank)
            {
                pass_bank = 0;
                active_pass = 0;
            }
            if (!hd_effect_range(&draw, first, used - first, &active_pass))
                return 0;
            continue;
        }
        const HD_SCENE_OBJECT *object = hd_objects + entry->object;
        HD_BANK *bank = object->state.bank;
        uint32 batch = bank->faces[entry->face].batch;
        uint32 first = used;
        do
        {
            used += (bank->faces[hd_alpha[i].face].count - 2) * 3;
            ++i;
        } while (i < hd_alpha_count && hd_alpha[i].object == entry->object && hd_alpha[i].pass == entry->pass && bank->faces[hd_alpha[i].face].batch == batch);
        if (pass_bank != bank || active_pass != entry->pass)
        {
            hd_bank_scene_pass(bank, entry->pass);
            pass_bank = bank;
            active_pass = entry->pass;
        }
        if (!hd_scene_submit(object, batch, hd_scene_indices, first, used - first))
            return 0;
    }
    hd_draw[18][2] = 0;
#if defined(_DEBUG)
    if (hd_debug_probe_enabled)
        for (i = 0; i < 9; ++i)
        {
            if (!hd_debug_probes[i])
                hd_debug_probes[i] = vid_sample_create();
            if (!hd_debug_probes[i] || !vid_sample(hd_debug_probes[i], (float)(i % 3 + 1) / 4, (float)(i / 3 + 1) / 4))
                return 0;
        }
#endif
    return 1;
}

void hd_ui_begin(void)
{
    ++hd_ui_revision;
    hd_ui_mesh_valid = 0;
    hd_ui_count = 0;
    hd_ui_previous_count = 0;
    memset(hd_ui_heads, 255, sizeof(hd_ui_heads));
    hd_ui_current_anchor = HD_UI_CENTER;
    hd_ui_previous_scroll = 0;
    hd_ui_collect = 1;
}

void hd_ui_end(void)
{
    hd_ui_collect = 0;
}

int hd_ui_collecting(void)
{
    return hd_ui_collect;
}

void hd_ui_set_anchor(uint32 anchor)
{
    hd_ui_current_anchor = anchor;
}

void hd_ui_set_scroll(float previous_offset)
{
    hd_ui_previous_scroll = previous_offset;
}

uint32 hd_ui_anchor(float x)
{
    return hd_ui_current_anchor == HD_UI_EDGES ? (x < 256 ? HD_UI_LEFT : HD_UI_RIGHT) : hd_ui_current_anchor;
}

void hd_ui_rect(HD_UI_QUAD *quad, float left, float top, float right, float bottom)
{
    quad->x[0] = quad->x[2] = left;
    quad->x[1] = quad->x[3] = right;
    quad->y[0] = quad->y[1] = top;
    quad->y[2] = quad->y[3] = bottom;
}

void hd_ui_image(HD_UI_QUAD *quad, const HD_UI_IMAGE *image, uint32 color, uint32 flags, uint32 depth, uint32 anchor)
{
    uint32 i;
    memset(quad, 0, sizeof(*quad));
    quad->flags = flags;
    quad->depth = depth < 4096 ? depth : 4095;
    quad->anchor = anchor;
    for (i = 0; i < 4; ++i)
    {
        quad->color[i] = color;
        if (image)
        {
            quad->u[i] = image->uv[i] & 255;
            quad->v[i] = image->uv[i] >> 8;
        }
    }
    if (image)
    {
        quad->page = image->page;
        quad->clut = image->clut;
    }
}

void hd_ui_add(const HD_UI_QUAD *quad)
{
    HD_UI_ITEM *item;
    if (!hd_ui_collect || quad->depth >= 4096)
        abort();
    if (hd_ui_count == 0xFFFFFFFF || !hd_reserve((void **)&hd_ui_items, &hd_ui_capacity, hd_ui_count + 1, sizeof(*hd_ui_items)))
        abort();
    item = hd_ui_items + hd_ui_count;
    item->quad = *quad;
    item->previous_scroll = hd_ui_previous_scroll;
    item->previous = 0;
    item->next = hd_ui_heads[quad->depth];
    hd_ui_heads[quad->depth] = hd_ui_count++;
}

void hd_ui_add_pair(const HD_UI_QUAD *current, const HD_UI_QUAD *previous)
{
    uint32 corner;
    if (current->page != previous->page || current->clut != previous->clut || current->flags != previous->flags || current->depth != previous->depth || current->anchor != previous->anchor)
        abort();
    for (corner = 0; corner < 4; ++corner)
        if (current->u[corner] != previous->u[corner] || current->v[corner] != previous->v[corner])
            abort();
    if (hd_ui_previous_count == 0xFFFFFFFF || !hd_reserve((void **)&hd_ui_previous, &hd_ui_previous_capacity, hd_ui_previous_count + 1, sizeof(*hd_ui_previous)))
        abort();
    hd_ui_add(current);
    hd_ui_previous[hd_ui_previous_count++] = *previous;
    hd_ui_items[hd_ui_count - 1].previous = hd_ui_previous_count;
}

static void hd_ui_arrow(HD_UI_QUAD *quad, sint32 x, sint32 y, uint32 offset, uint32 side, uint32 anchor)
{
    sint16 tip = (sint16)((uint32)x + (side ? offset : 0 - offset));
    sint16 edge = (sint16)(tip + (side ? 20 : -20));
    hd_ui_image(quad, 0, 255, 0, 0, anchor);
    quad->x[0] = tip;
    quad->y[0] = (sint16)y;
    quad->x[1] = quad->x[2] = quad->x[3] = edge;
    quad->y[1] = (sint16)(y - 6);
    quad->y[2] = quad->y[3] = (sint16)(y + 6);
}

void hd_ui_arrows(HD_UI_ARROW_HISTORY *history, sint32 x, sint32 y, uint32 offset)
{
    uint32 side, anchor = hd_ui_anchor((float)x);
    int interpolate = history->initialized && history->revision + 1 == hd_ui_revision && history->epoch == hd_history_epoch && history->anchor == anchor;
    for (side = 0; side < 2; ++side)
    {
        HD_UI_QUAD current, previous;
        hd_ui_arrow(&current, x, y, offset, side, anchor);
        if (interpolate)
            hd_ui_arrow(&previous, history->x, history->y, history->offset, side, anchor);
        else
            previous = current;
        hd_ui_add_pair(&current, &previous);
    }
    history->x = x;
    history->y = y;
    history->offset = offset;
    history->anchor = anchor;
    history->revision = hd_ui_revision;
    history->epoch = hd_history_epoch;
    history->initialized = 1;
}

static int hd_ui_pass(uint32 pass)
{
    static const uint32 blends[5] = {VID_BLEND_NONE, VID_BLEND_ALPHA, VID_BLEND_ADD, VID_BLEND_SUBTRACT, VID_BLEND_ADD};
    static const float alpha[5] = {1, 0.5f, 1, 1, 0.25f};
    float values[4] = {0, alpha[pass], (float)(pass + 1), hd_render_phase};
    if (!hd_ui_materials[pass])
    {
        VID_MATERIAL_DATA *data;
        hd_ui_materials[pass] = vid_mat_create();
        if (!hd_ui_materials[pass])
            return 0;
        data = vid_mat_data(hd_ui_materials[pass]);
        data->shader = hd_ui_shader;
        data->texture = hd_vram;
        data->blend = blends[pass];
        vid_mat_commit(hd_ui_materials[pass]);
    }
    vid_uniforms(values, 1);
    vid_pass(hd_ui_materials[pass]);
    return 1;
}

int hd_ui_draw(void)
{
    sint32 width = 0, height = 0, depth;
    uint32 at = 0, pass = 5, run_first = 0, run_count = 0;
    VID_VERTEX3D *vertices;
    if (!hd_active || hd_ui_collect || hd_ui_submitted)
        return 0;
    hd_ui_submitted = 1;
    if (!hd_ui_count)
        return 1;
    if (hd_ui_count > hd_ui_mesh_capacity)
    {
        uint32 capacity = hd_ui_mesh_capacity ? hd_ui_mesh_capacity : 256;
        uint32 *indices, quad;
        while (capacity < hd_ui_count)
        {
            if (capacity > 0x1FFFFFFF)
                return 0;
            capacity *= 2;
        }
        vid_mesh_free(hd_ui_mesh);
        vid_index_free(hd_ui_indices);
        hd_ui_mesh = vid_mesh_create_3d(capacity * 4);
        hd_ui_indices = vid_index_create(capacity * 6);
        hd_ui_mesh_capacity = 0;
        hd_ui_mesh_valid = 0;
        if (!hd_ui_mesh || !hd_ui_indices)
            return 0;
        indices = vid_index_data(hd_ui_indices);
        for (quad = 0; quad < capacity; ++quad)
            hd_face_indices(indices + quad * 6, quad * 4, 4);
        vid_index_commit(hd_ui_indices, capacity * 6);
        vid_index_static(hd_ui_indices);
        hd_ui_mesh_capacity = capacity;
    }
    vid_resize(&width, &height);
    if (width <= 0 || height <= 0)
        return 0;
    if (!hd_ui_mesh_valid || hd_ui_uploaded_revision != hd_ui_revision || hd_ui_drawable_width != width || hd_ui_drawable_height != height)
    {
        vertices = vid_mesh_data_3d(hd_ui_mesh);
        for (depth = 4095; depth >= 0; --depth)
        {
            uint32 index;
            for (index = hd_ui_heads[depth]; index != 0xFFFFFFFF; index = hd_ui_items[index].next)
            {
                const HD_UI_QUAD *quad = &hd_ui_items[index].quad;
                const HD_UI_QUAD *previous = hd_ui_items[index].previous ? hd_ui_previous + hd_ui_items[index].previous - 1 : quad;
                uint32 corner;
                float u0 = quad->u[0], u1 = u0, v0 = quad->v[0], v1 = v0;
                for (corner = 1; corner < 4; ++corner)
                {
                    if (quad->u[corner] < u0) u0 = quad->u[corner];
                    if (quad->u[corner] > u1) u1 = quad->u[corner];
                    if (quad->v[corner] < v0) v0 = quad->v[corner];
                    if (quad->v[corner] > v1) v1 = quad->v[corner];
                }
                for (corner = 0; corner < 4; ++corner)
                {
                    VID_VERTEX3D *vertex = vertices + at * 4 + corner;
                    float x = quad->x[corner], scale = (float)height / 384;
                    float previous_x = previous->x[corner];
                    memset(vertex, 0, sizeof(*vertex));
                    if (quad->anchor == HD_UI_LEFT)
                    {
                        vertex->pos[0] = 2 * x * scale / width - 1;
                        vertex->pos[2] = 2 * previous_x * scale / width - 1;
                    }
                    else if (quad->anchor == HD_UI_RIGHT)
                    {
                        vertex->pos[0] = 1 + 2 * (x - 512) * scale / width;
                        vertex->pos[2] = 1 + 2 * (previous_x - 512) * scale / width;
                    }
                    else if (quad->anchor == HD_UI_STRETCH)
                    {
                        vertex->pos[0] = x / 256 - 1;
                        vertex->pos[2] = previous_x / 256 - 1;
                    }
                    else
                    {
                        vertex->pos[0] = 2 * (x - 256) * scale / width;
                        vertex->pos[2] = 2 * (previous_x - 256) * scale / width;
                    }
                    vertex->pos[1] = 1 - quad->y[corner] / 120;
                    vertex->pos[3] = 1 - (previous->y[corner] + hd_ui_items[index].previous_scroll) / 120;
                    vertex->uv[0] = quad->u[corner];
                    vertex->uv[1] = quad->v[corner];
                    vertex->color[0] = quad->color[corner] & 255;
                    vertex->color[1] = (quad->color[corner] >> 8) & 255;
                    vertex->color[2] = (quad->color[corner] >> 16) & 255;
                    vertex->color[3] = 255;
                    vertex->n[0] = previous->color[corner] & 255;
                    vertex->n[1] = (previous->color[corner] >> 8) & 255;
                    vertex->n[2] = (previous->color[corner] >> 16) & 255;
                    vertex->n[3] = 255;
                    // Page and CLUT occupy exactly twenty-four integer bits
                    vertex->extra[0] = (float)((quad->page & 511) | ((uint32)(quad->clut & 32767) << 9));
                    vertex->extra[1] = quad->flags;
                    vertex->extra[2] = (uint8)(sint32)u0 | ((uint16)(uint8)(sint32)u1 << 8);
                    vertex->extra[3] = (uint8)(sint32)v0 | ((uint16)(uint8)(sint32)v1 << 8);
                }
                ++at;
            }
        }
        vid_mesh_commit(hd_ui_mesh);
        hd_ui_uploaded_revision = hd_ui_revision;
        hd_ui_drawable_width = width;
        hd_ui_drawable_height = height;
        hd_ui_mesh_valid = 1;
    }
    at = 0;
    for (depth = 4095; depth >= 0; --depth)
    {
        uint32 index;
        for (index = hd_ui_heads[depth]; index != 0xFFFFFFFF; index = hd_ui_items[index].next, ++at)
        {
            const HD_UI_QUAD *quad = &hd_ui_items[index].quad;
            int semi = (quad->flags & HD_SEMITRANSPARENT) != 0;
            int textured = (quad->flags & HD_TEXTURED) != 0;
            uint32 selected = semi && !textured ? 1 + ((quad->page >> 5) & 3) : 0;
            if (run_count && (selected != pass || (semi && textured)))
            {
                if (!hd_ui_pass(pass) || !vid_submit(hd_ui_mesh, hd_ui_indices, run_first * 6, run_count * 6))
                    return 0;
                run_count = 0;
            }
            if (semi && textured)
            {
                if (!hd_ui_pass(0) || !vid_submit(hd_ui_mesh, hd_ui_indices, at * 6, 6) || !hd_ui_pass(1 + ((quad->page >> 5) & 3)) || !vid_submit(hd_ui_mesh, hd_ui_indices, at * 6, 6))
                    return 0;
            }
            else
            {
                if (!run_count)
                {
                    pass = selected;
                    run_first = at;
                }
                ++run_count;
            }
        }
    }
    return !run_count || (hd_ui_pass(pass) && vid_submit(hd_ui_mesh, hd_ui_indices, run_first * 6, run_count * 6));
}

static sint32 hd_ui_scale(sint32 value, uint32 scale)
{
    return (sint32)((uint32)value * scale) >> 8;
}

static float hd_ui_text_wrap(float value)
{
    if (value >= -32768 && value < 32768)
        return value;
    float turns = (value + 32768) / 65536;
    sint32 whole = (sint32)turns;
    if (turns < whole)
        --whole;
    return value - (float)whole * 65536;
}

static void hd_ui_text_quad(const HD_UI_QUAD *source)
{
    HD_UI_QUAD quad = *source;
    uint32 corner;
    for (corner = 0; corner < 4; ++corner)
    {
        quad.x[corner] = hd_ui_text_wrap(quad.x[corner]);
        quad.y[corner] = hd_ui_text_wrap(quad.y[corner]);
        quad.u[corner] = (float)(uint8)(sint32)quad.u[corner];
        quad.v[corner] = (float)(uint8)(sint32)quad.v[corner];
    }
    if (hd_ui_text_endpoint == 1)
    {
        if (hd_ui_text_staging_count == 0xFFFFFFFF || !hd_reserve((void **)&hd_ui_text_staging, &hd_ui_text_staging_capacity, hd_ui_text_staging_count + 1, sizeof(*hd_ui_text_staging)))
            abort();
        hd_ui_text_staging[hd_ui_text_staging_count++] = quad;
    }
    else if (hd_ui_text_endpoint == 2)
    {
        if (hd_ui_text_staging_cursor >= hd_ui_text_staging_count)
            abort();
        HD_UI_QUAD previous = hd_ui_text_staging[hd_ui_text_staging_cursor++];
        /* Texture cropping is discrete while glyph geometry keeps its scale history */
        memcpy(previous.u, quad.u, sizeof(quad.u));
        memcpy(previous.v, quad.v, sizeof(quad.v));
        hd_ui_add_pair(&quad, &previous);
    }
    else
        hd_ui_add(&quad);
}

uint32 hd_ui_text(sint32 x, sint32 y, const char *text, const HD_UI_TEXT_STYLE *style, int filter, uint32 glow_color, uint32 glow_scale, const HD_UI_IMAGE *glow, int (*glyph)(uint8 character, HD_UI_IMAGE *image), uint32 (*random)(uint32 limit))
{
    const uint8 *cursor = (const uint8 *)text;
    HD_UI_IMAGE image;
    uint32 glyphs = 0, width = 0, curve = 0;
    uint32 anchor = hd_ui_anchor((float)x);
    float pen, total, baseline = 9.0f * style->scale / 256;
    while (*cursor)
    {
        if (glyph(*cursor++, &image))
        {
            width += image.width + image.offset_x;
            ++glyphs;
        }
        else
            width += 10;
    }
    total = (float)width * style->scale / 256 + 3 * glyphs;
    pen = x - (style->alignment == 0 ? total / 2 : (style->alignment == 2 ? total : 0));
    cursor = (const uint8 *)text;
    glow_color = glow ? glow_color | (glow_color << 8) | (glow_color << 16) : 0;
    while (*cursor)
    {
        HD_UI_QUAD letter, black, colored, flame, fade;
        float left, top, right, bottom;
        if (!glyph(*cursor++, &image))
        {
            pen += 10.0f * style->scale / 256;
            continue;
        }
        left = pen + image.offset_x * (float)style->scale / 256;
        top = y + image.offset_y * (float)style->scale / 256 - baseline;
        right = left + image.width * (float)style->scale / 256;
        bottom = top + image.height * (float)style->scale / 256;
        hd_ui_image(&letter, &image, style->color, HD_TEXTURED | style->flags, style->depth, anchor);
        hd_ui_rect(&letter, (float)left, (float)top, (float)right, (float)bottom);
        if (style->scale >= 512 || filter)
        {
            --letter.u[1];
            --letter.u[3];
            --letter.v[2];
            --letter.v[3];
        }
        {
            uint32 corner;
            for (corner = 0; corner < 4; ++corner)
            {
                letter.u[corner] = (float)(uint8)(sint32)letter.u[corner];
                letter.v[corner] = (float)(uint8)(sint32)letter.v[corner];
            }
        }
        if (glow_color)
        {
            sint32 next = curve + 1 < 41 ? curve + 1 : 0;
            float y0 = top - style->curve[curve], y1 = top - style->curve[next];
            float x0 = left - 10, x1 = right + 10;
            float top0 = bottom - (bottom - y0) * glow_scale / 256;
            float top1 = bottom - (bottom - y1) * glow_scale / 256;
            float base0 = bottom - (bottom - top0) / 8, base1 = bottom - (bottom - top1) / 8;
            float center = (x0 + x1) / 2, half = 3 * (x1 - x0) / 16;
            sint32 u0 = glow->uv[2] & 255, u1 = glow->uv[3] & 255;
            sint32 uv_center = (u0 + u1) >> 1, uv_half = (3 * (u1 - u0)) >> 4;
            sint32 v0 = glow->uv[0] >> 8, v1 = glow->uv[2] >> 8;
            sint32 split = v1 - ((v1 - v0) >> 3);
            uint32 corner;
            curve = next + 1 < 41 ? next + 1 : 0;
            hd_ui_image(&flame, glow, glow_color, HD_TEXTURED | HD_SEMITRANSPARENT, style->depth, anchor);
            hd_ui_image(&fade, glow, glow_color, HD_TEXTURED | HD_SEMITRANSPARENT, style->depth, anchor);
            flame.page = fade.page = (glow->page & 0xFF9F) | 32;
            hd_ui_rect(&flame, (float)x0, (float)top0, (float)x1, (float)base0);
            flame.y[1] = (float)top1;
            flame.y[3] = (float)base1;
            flame.v[2] = flame.v[3] = (float)split;
            hd_ui_rect(&fade, (float)x0, (float)base0, (float)x1, (float)(bottom + 2));
            fade.y[1] = (float)base1;
            fade.x[2] = (float)(center - half);
            fade.x[3] = (float)(center + half);
            fade.u[2] = (float)(uv_center - uv_half);
            fade.u[3] = (float)(uv_center + uv_half);
            fade.v[0] = fade.v[1] = (float)split;
            fade.v[2] = (float)(v1 - 1);
            fade.v[3] = (float)((glow->uv[3] >> 8) - 1);
            for (corner = 2; corner < 4; ++corner)
                fade.color[corner] = 0;
        }
        black = colored = letter;
        {
            uint32 corner;
            for (corner = 0; corner < 4; ++corner)
            {
                black.x[corner] += 1;
                black.y[corner] += 1;
                black.color[corner] = 0;
                colored.x[corner] -= 2;
                colored.y[corner] -= 1;
                colored.color[corner] = style->shadow;
            }
        }
        if (!style->alternate)
        {
            if (glow_color)
            {
                hd_ui_text_quad(&flame);
                hd_ui_text_quad(&fade);
            }
            hd_ui_text_quad(&letter);
            hd_ui_text_quad(&colored);
            hd_ui_text_quad(&black);
        }
        else
        {
            uint32 spark;
            for (spark = 0; spark < 3; ++spark)
            {
                HD_UI_QUAD quad = letter;
                sint32 x0 = random(32), y0 = random(128);
                sint32 x1 = x0 + random(192 - x0) + 64;
                sint32 y1 = y0 + random(192 - y0) + 64;
                sint32 dx = random(6) - 3, dy = random(6) - 3;
                sint32 w = right - left, h = bottom - top;
                sint32 uw = (sint32)letter.u[1] - (sint32)letter.u[0];
                sint32 vh = (sint32)letter.v[2] - (sint32)letter.v[0];
                quad.flags |= HD_SEMITRANSPARENT;
                quad.page |= 96;
                hd_ui_rect(&quad, (float)(left + hd_ui_scale(x0, w) + dx), (float)(top + hd_ui_scale(y0, h) + dy), (float)(left + hd_ui_scale(x1, w) + dx), (float)(top + hd_ui_scale(y1, h) + dy));
                quad.u[0] = quad.u[2] = letter.u[0] + hd_ui_scale(x0, uw);
                quad.u[1] = quad.u[3] = letter.u[0] + hd_ui_scale(x1, uw);
                quad.v[0] = quad.v[1] = letter.v[0] + hd_ui_scale(y0, vh);
                quad.v[2] = quad.v[3] = letter.v[0] + hd_ui_scale(y1, vh);
                hd_ui_text_quad(&quad);
            }
        }
        pen += 3 + (image.width + image.offset_x) * (float)style->scale / 256;
    }
    return total;
}

uint32 hd_ui_text_pair(sint32 x, sint32 y, const char *text, const HD_UI_TEXT_STYLE *style, const HD_UI_TEXT_STYLE *previous, int filter, uint32 glow_color, uint32 glow_scale, uint32 previous_glow_color, uint32 previous_glow_scale, const HD_UI_IMAGE *glow, int (*glyph)(uint8 character, HD_UI_IMAGE *image), uint32 (*random)(uint32 limit))
{
    uint32 width;
    if (style->alternate || previous->alternate || style->flags != previous->flags || style->depth != previous->depth || style->alignment != previous->alignment || (!glow_color != !previous_glow_color))
        return hd_ui_text(x, y, text, style, filter, glow_color, glow_scale, glow, glyph, random);
    if (hd_ui_text_endpoint)
        abort();
    hd_ui_text_staging_count = hd_ui_text_staging_cursor = 0;
    hd_ui_text_endpoint = 1;
    hd_ui_text(x, y, text, previous, filter, previous_glow_color, previous_glow_scale, glow, glyph, random);
    hd_ui_text_endpoint = 2;
    width = hd_ui_text(x, y, text, style, filter, glow_color, glow_scale, glow, glyph, random);
    hd_ui_text_endpoint = 0;
    if (hd_ui_text_staging_cursor != hd_ui_text_staging_count)
        abort();
    return width;
}

uint32 hd_ui_text_history(HD_UI_TEXT_HISTORY *history, sint32 x, sint32 y, const char *text, const HD_UI_TEXT_STYLE *style, int filter, uint32 glow_color, uint32 glow_scale, const HD_UI_IMAGE *glow, int (*glyph)(uint8 character, HD_UI_IMAGE *image), uint32 (*random)(uint32 limit))
{
    uint32 width;
    if (history->initialized && history->revision + 1 == hd_ui_revision && history->epoch == hd_history_epoch && history->x == x && history->y == y)
    {
        HD_UI_TEXT_STYLE previous = history->style;
        previous.curve = history->curve;
        width = hd_ui_text_pair(x, y, text, style, &previous, filter, glow_color, glow_scale, history->glow_color, history->glow_scale, glow, glyph, random);
    }
    else
        width = hd_ui_text(x, y, text, style, filter, glow_color, glow_scale, glow, glyph, random);
    history->style = *style;
    history->style.curve = 0;
    if (style->curve)
        memcpy(history->curve, style->curve, sizeof(history->curve));
    else
        memset(history->curve, 0, sizeof(history->curve));
    history->x = x;
    history->y = y;
    history->glow_color = glow_color;
    history->glow_scale = glow_scale;
    history->revision = hd_ui_revision;
    history->epoch = hd_history_epoch;
    history->initialized = 1;
    return width;
}

int hd_scene_present(const HD_CAMERA *camera, float r, float g, float b)
{
    sint32 width = 0, height = 0;
    int queued;
#if defined(_DEBUG)
    uint64 start = xport_timer_get();
#endif
    vid_resize(&width, &height);
    if (width <= 0 || height <= 0)
        return 1;
    queued = hd_begin(camera, r, g, b) && hd_scene_draw() && hd_ui_draw();
#if defined(_DEBUG)
    hd_debug_timing.queue_us += xport_timer_get() - start;
    ++hd_debug_timing.queue_frames;
#endif
    return queued && hd_end();
}

void hd_frame_begin(HD_FRAME *frame)
{
    hd_frame_interrupt();
    hd_current_frame = frame;
    frame->cached = 0;
    frame->camera_captured = 0;
    frame->selected = xport_hd != 0;
    if (frame->selected != hd_last_selected)
    {
        hd_restore_og = !frame->selected && hd_last_selected == 1;
        ++hd_history_epoch;
        hd_last_selected = frame->selected;
    }
    frame->open = 1;
    gpu_set_raster(!frame->selected);
    gpu_frame_begin();
    if (frame->selected)
    {
        hd_scene_clear();
        hd_ui_begin();
        IdentityMatrixf(&frame->camera.view);
        hd_projection(&frame->camera, 120, 512, 240, 256, 120, 1, 32768);
    }
}

int hd_frame_present_phase(HD_FRAME *frame, float phase)
{
    HD_CAMERA camera;
    int result;
    if (!frame->open)
        return 1;
    hd_ui_end();
    camera = frame->camera;
    if (frame->selected && frame->camera_captured && frame->camera_revision == hd_scene_revision)
        hd_camera_evaluate(&frame->camera_history, phase, &camera);
    result = !frame->selected || (hd_scene_phase(phase) && hd_scene_present(&camera, 0, 0, 0));
    frame->cached = result && frame->selected;
    frame->cached_revision = hd_scene_revision;
    frame->ui_revision = hd_ui_revision;
    gpu_frame_end();
    frame->open = 0;
    hd_current_frame = 0;
    if (!frame->selected)
    {
        gpu_present();
        hd_restore_og = 0;
    }
    return result;
}

int hd_frame_present(HD_FRAME *frame)
{
    return hd_frame_present_phase(frame, 1);
}

int hd_frame_can_redraw(const HD_FRAME *frame)
{
    return frame && !frame->open && frame->cached && frame->selected && !hd_current_frame && !hd_active && !hd_collecting && !hd_ui_collect && frame->cached_revision == hd_scene_revision && frame->ui_revision == hd_ui_revision;
}

int hd_frame_redraw(HD_FRAME *frame, float phase)
{
    HD_CAMERA camera;
#if defined(_DEBUG)
    uint64 start = xport_timer_get();
#endif
    if (!hd_frame_can_redraw(frame))
        return 0;
    camera = frame->camera;
    if (frame->camera_captured && frame->camera_revision == frame->cached_revision && frame->camera_epoch == hd_history_epoch && frame->camera_serial == hd_capture_serial)
        hd_camera_evaluate(&frame->camera_history, phase, &camera);
    hd_scene_phase(phase);
#if defined(_DEBUG)
    hd_debug_timing.redraw_phase_us += xport_timer_get() - start;
    ++hd_debug_timing.redraw_phase_frames;
#endif
    return hd_scene_present(&camera, 0, 0, 0);
}

void hd_frame_abandon(HD_FRAME *frame)
{
    frame->cached = 0;
    if (frame->open)
        gpu_frame_end();
    frame->open = 0;
    hd_frame_camera_reset(frame);
    hd_reset();
    gpu_set_raster(1);
}

int hd_image_draw(uint32 x, uint32 y, uint32 width, uint32 height, int rgb24)
{
    static const uint8 corners[6] = {0, 1, 2, 1, 3, 2};
    sint32 drawable_width = 0, drawable_height = 0;
    float fit_x = 1, fit_y = 1;
    float values[2][4] = {{(float)x, (float)y, rgb24 ? 1.0f : 0.0f, 0}, {(float)width, (float)height, 0, 0}};
    VID_VERTEX *vertices;
    uint32 i;
    if (!hd_active || !width || !height || x >= 1024 || y >= 512 || height > 512 - y || width > (rgb24 ? (1024 - x) * 2 / 3 : 1024 - x))
        return 0;
    if (!hd_image_mesh)
        hd_image_mesh = vid_mesh_create(6);
    if (!hd_image_material)
    {
        VID_MATERIAL_DATA *data;
        hd_image_material = vid_mat_create();
        if (!hd_image_material)
            return 0;
        data = vid_mat_data(hd_image_material);
        data->shader = hd_image_shader;
        data->texture = hd_vram;
        vid_mat_commit(hd_image_material);
    }
    if (!hd_image_mesh)
        return 0;
    vid_resize(&drawable_width, &drawable_height);
    if (drawable_width <= 0 || drawable_height <= 0)
        return 0;
    if ((float)drawable_width / drawable_height > 4.0f / 3)
        fit_x = (float)drawable_height * 4 / (drawable_width * 3);
    else
        fit_y = (float)drawable_width * 3 / (drawable_height * 4);
    if (hd_image_source_width != width || hd_image_source_height != height || hd_image_drawable_width != drawable_width || hd_image_drawable_height != drawable_height)
    {
        vertices = vid_mesh_data(hd_image_mesh);
        if (!vertices)
            return 0;
        for (i = 0; i < 6; ++i)
        {
            uint32 corner = corners[i];
            VID_VERTEX *vertex = vertices + i;
            memset(vertex, 0, sizeof(*vertex));
            vertex->pos[0] = corner & 1 ? fit_x : -fit_x;
            vertex->pos[1] = corner & 2 ? -fit_y : fit_y;
            vertex->pos[3] = 1;
            vertex->uv[0] = corner & 1 ? (float)width : 0;
            vertex->uv[1] = corner & 2 ? (float)height : 0;
        }
        vid_mesh_commit(hd_image_mesh);
        hd_image_source_width = width;
        hd_image_source_height = height;
        hd_image_drawable_width = drawable_width;
        hd_image_drawable_height = drawable_height;
    }
    vid_uniforms(&values[0][0], 2);
    vid_pass(hd_image_material);
    return vid_submit(hd_image_mesh, 0, 0, 6);
}

int hd_frame_image(HD_FRAME *frame, uint32 x, uint32 y, uint32 width, uint32 height, int rgb24)
{
    int result = 1;
    sint32 drawable_width = 0, drawable_height = 0;
    if (!frame->open)
        return 1;
    hd_ui_end();
    vid_resize(&drawable_width, &drawable_height);
    if (frame->selected && drawable_width > 0 && drawable_height > 0)
        result = hd_begin(&frame->camera, 0, 0, 0) && hd_image_draw(x, y, width, height, rgb24) && hd_ui_draw() && hd_end();
    gpu_frame_end();
    frame->open = 0;
    hd_current_frame = 0;
    if (!frame->selected)
    {
        gpu_present();
        hd_restore_og = 0;
    }
    return result;
}

int hd_frame_restoring_og(void)
{
    return hd_restore_og && hd_current_frame && hd_current_frame->open && !hd_current_frame->selected;
}

int hd_frame_active(void)
{
    return hd_current_frame && hd_current_frame->open;
}

void hd_frame_interrupt(void)
{
    if (hd_frame_active())
        hd_frame_abandon(hd_current_frame);
}
