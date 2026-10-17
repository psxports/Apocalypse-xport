#include "game_sdk_bindings.h"
#include "model_vram.h"
#include <stdlib.h>

struct VramRecord
{
    uint16 x;
    uint16 y;
    uint16 width;
    uint16 height;
    VramRecord *next;
    sint8 owner;
    uint8 mode;
    uint8 depth;
    uint8 flags;
    uint16 source_width;
    uint16 source_height;
};

static VramRecord *free_rects;
static VramRecord *used_rects;
static uint32 record_count;
static uint32 allocation_mode;

void SetModelVramAllocationMode(uint32 mode)
{
    allocation_mode = mode;
}

static VramRecord *AllocateVramRecord()
{
    FUNCTION_MARKER(0x80068240, "SLUS_003.73");
    VramRecord *record = (VramRecord *)malloc(sizeof(VramRecord));
    if (!record)
    {
        GameDiagnostic("Cannot allocate a texture VRAM record\n");
        abort();
    }
    ++record_count;
    return record;
}

static uint32 ReleaseVramRecord(VramRecord *record)
{
    FUNCTION_MARKER(0x80068270, "SLUS_003.73");
    free(record);
    return --record_count;
}

static VramRecord *PrependVramRecord(VramRecord **head)
{
    FUNCTION_MARKER(0x80068298, "SLUS_003.73");
    VramRecord *record = AllocateVramRecord();
    record->next = *head;
    *head = record;
    return record;
}

static VramRecord *SetVramRectangle(VramRecord *record, uint32 x, uint32 y, uint32 width, uint32 height)
{
    record->x = (uint16)x;
    record->y = (uint16)y;
    record->width = (uint16)width;
    record->height = (uint16)height;
    return record;
}

static VramRecord *CreateFreeVramRectangle(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x800682CC, "SLUS_003.73");
    return SetVramRectangle(PrependVramRecord(&free_rects), x, y, width, height);
}

static VramRecord *CreateUsedVramRectangle(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x8006832C, "SLUS_003.73");
    return SetVramRectangle(PrependVramRecord(&used_rects), x, y, width, height);
}

static uint32 RemoveVramRecord(VramRecord **head, VramRecord *record)
{
    FUNCTION_MARKER(0x8006838C, "SLUS_003.73");
    VramRecord *prev = *head;
    if (prev == record)
        *head = record->next;
    else
    {
        // The caller owns the record in this nonempty list
        while (prev->next != record)
            prev = prev->next;
        prev->next = record->next;
    }
    return ReleaseVramRecord(record);
}

void InitializeModelVramRectangles()
{
    FUNCTION_MARKER(0x80068404, "SLUS_003.73");
    while (free_rects)
        RemoveVramRecord(&free_rects, free_rects);
    while (used_rects)
        RemoveVramRecord(&used_rects, used_rects);
    record_count = 0;
    CreateFreeVramRectangle(512, 256, 512, 256);
    CreateFreeVramRectangle(512, 0, 512, 256);
}

static void StoreModelVramCoordinate(uint32 *output, uint32 value)
{
    *output = value;
}

static VramRecord *MergeFreeVramRectangles(VramRecord *first, VramRecord *second)
{
    FUNCTION_MARKER(0x80068E80, "SLUS_003.73");
    uint32 first_y = first->y;
    uint32 second_y = second->y;
    uint32 result, x, y, width, height;
    if (first_y == second_y)
    {
        uint32 first_x = first->x;
        uint32 second_x = second->x;
        result = second_x < first_x;
        if (first_x < second_x)
        {
            uint32 first_width = first->width;
            result = first_x + first_width;
            if (result != second_x)
                return 0;
            uint32 first_height = first->height;
            result = second->height;
            if (first_height != result)
                return 0;
            x = first_x;
            y = first->y;
            uint32 second_width = second->width;
            height = first->height;
            width = first_width + second_width;
        }
        else if (second_x < first_x)
        {
            second_x = second->x;
            uint32 second_width = second->width;
            first_x = first->x;
            result = second_x + second_width;
            if (result != first_x)
                return 0;
            uint32 first_height = first->height;
            result = second->height;
            if (first_height != result)
                return 0;
            x = second_x;
            y = second->y;
            uint32 first_width = first->width;
            height = first->height;
            width = first_width + second_width;
        }
        else
            return 0;
    }
    else
    {
        uint32 first_x = first->x;
        result = second->x;
        if (first_x != result)
            return 0;
        result = second_y < first_y;
        if (first_y < second_y)
        {
            first_y = first->y;
            uint32 first_height = first->height;
            second_y = second->y;
            result = first_y + first_height;
            if (result != second_y)
                return 0;
            uint32 first_width = first->width;
            result = second->width;
            if (first_width != result)
                return 0;
            y = first_y;
            x = first->x;
            uint32 second_height = second->height;
            width = first->width;
            height = first_height + second_height;
        }
        else if (second_y < first_y)
        {
            second_y = second->y;
            uint32 second_height = second->height;
            first_y = first->y;
            result = second_y + second_height;
            if (result != first_y)
                return 0;
            uint32 first_width = first->width;
            result = second->width;
            if (first_width != result)
                return 0;
            y = second_y;
            x = second->x;
            uint32 first_height = first->height;
            width = first->width;
            height = first_height + second_height;
        }
        else
            return 0;
    }
    RemoveVramRecord(&free_rects, first);
    RemoveVramRecord(&free_rects, second);
    return CreateFreeVramRectangle(x, y, width, height);
}

static bool AreVramRectanglesAdjacent(VramRecord *record, VramRecord *candidate)
{
    uint32 record_y = record->y;
    uint32 candidate_y = candidate->y;
    bool horizontal = record_y == candidate_y;
    if (horizontal)
    {
        uint32 record_height = record->height;
        uint32 candidate_height = candidate->height;
        horizontal = record_height == candidate_height;
    }
    if (horizontal)
    {
        uint32 candidate_x = candidate->x;
        uint32 candidate_width = candidate->width;
        uint32 record_x = record->x;
        if (candidate_x + candidate_width == record_x)
            return true;
        uint32 record_width = record->width;
        return record_x + record_width == candidate_x;
    }
    uint32 record_x = record->x;
    uint32 candidate_x = candidate->x;
    if (record_x != candidate_x)
        return false;
    uint32 record_width = record->width;
    uint32 candidate_width = candidate->width;
    if (record_width != candidate_width)
        return false;
    candidate_y = candidate->y;
    record_y = record->y;
    if ((candidate_y & 256) != (record_y & 256))
        return false;
    candidate_y = candidate->y;
    uint32 candidate_height = candidate->height;
    record_y = record->y;
    if (candidate_y + candidate_height == record_y)
        return true;
    uint32 record_height = record->height;
    return record_y + record_height == candidate_y;
}

static uint32 CoalesceFreeVramRectangles(VramRecord *record)
{
    FUNCTION_MARKER(0x800690A0, "SLUS_003.73");
    uint32 result = 0;
    bool merged;
    do
    {
        VramRecord *candidate = free_rects;
        merged = false;
        while (candidate)
        {
            if (AreVramRectanglesAdjacent(record, candidate))
            {
                record = MergeFreeVramRectangles(candidate, record);
                merged = true;
                result = 1;
                break;
            }
            candidate = candidate->next;
        }
    } while (merged);
    return result;
}

uint32 ReleaseUsedVramRectangle(uint32 x, uint32 y)
{
    FUNCTION_MARKER(0x80068DC4, "SLUS_003.73");
    VramRecord *record;
    uint32 record_x, record_y, width, height;
    x = (uint32)(sint32)(sint16)x;
    y = (uint32)(sint32)(sint16)y;
    record = used_rects;
    while (record)
    {
        record_x = record->x;
        if (record_x == x)
        {
            record_y = record->y;
            if (record_y == y)
            {
                width = record->width;
                height = record->height;
                RemoveVramRecord(&used_rects, record);
                record = CreateFreeVramRectangle(record_x, record_y, width, height);
                return CoalesceFreeVramRectangles(record);
            }
        }
        record = record->next;
    }
    return 0;
}

static bool IsVramRecordAreaSmaller(VramRecord *candidate, VramRecord *selected)
{
    uint32 width, height, selected_width, selected_height;
    width = candidate->width;
    height = candidate->height;
    selected_width = selected->width;
    selected_height = selected->height;
    return (sint32)(width * height) < (sint32)(selected_width * selected_height);
}

VramRecord *AllocateModelTextureRectangle(uint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 name)
{
    sint32 v12;
    sint32 v14;
    VramRecord *v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    unsigned short v20;
    unsigned short v21;
    sint32 v22;
    VramRecord *v23;
    VramRecord *v24;
    VramRecord *v25;
    VramRecord *v26;
    VramRecord *result;
    sint8 v28;
    unsigned short v29;
    VramRecord *v30;
    VramRecord *v31;
    sint32 v32;
    signed int v33;
    signed int v34;
    sint32 v35;
    sint32 v36;
    VramRecord *v37;
    sint32 v38;
    signed int v39;
    sint32 v40;
    VramRecord *v41;
    sint32 v42;
    uint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    VramRecord *v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    VramRecord *v52;
    sint32 v53;
    VramRecord *v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    VramRecord *v58;
    VramRecord *v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    VramRecord *v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    VramRecord *v68;
    sint8 v69;
    short v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    FUNCTION_MARKER(0x80068450, "SLUS_003.73");
    // TODO Audit extreme signed dimensions
    // TODO Original recursive allocation failure is dereferenced without a null check
    v12 = width;
    v73 = width;
    v14 = width;
    v74 = height;
    if (!flags)
        goto LABEL_28;
    if (((width == 256) && (height == 256)))
        flags = 0;
    if (!flags)
    {
    LABEL_28:
        if ((depth != 16))
            v14 = (((sint32)(((uint32)(width) + (uint32)(1)))) / 2);

        if ((depth == 4))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
        v31 = free_rects;
        v30 = 0;
        if (v31)
        {
            do
            {
                v32 = ((sint32)v31->x);
                v33 = ((sint32)(((uint32)(v32) + (uint32)(v14))));
                if ((depth == 8))
                    v34 = ((uint32)((v32 & 0xFFFFFFC0)) + (uint32)(128));
                else
                    v34 = ((uint32)((v32 & 0xFFFFFFC0)) + (uint32)(64));
                v35 = (((sint32)(v34)) < ((sint32)(v33)));
                if (((!allocation_mode || ((((sint32)v31->y) + height) & 255)) && ((((sint32)(owner)) < 0) || !v35)))
                {
                    v36 = ((sint32)v31->width);
                    if (((v36 == v14) && (((sint32)v31->height) == height)))
                    {
                        v30 = v31;
                        break;
                    }
                    if (((((((!v30 || ((((sint32)v30->width) != v14) && (((sint32)v30->height) != height))) || (v36 == v14)) || (((sint32)v31->height) == height)) && (((sint32)v31->width) >= ((sint32)(v14)))) && (((sint32)v31->height) >= ((sint32)(height)))) && (!v30 || IsVramRecordAreaSmaller(v31, v30))))
                    {
                        v30 = v31;
                    }
                }
                v31 = v31->next;
            } while (v31);
        }
        if (!v30)
        {
            v37 = free_rects;
            if (v37)
            {
                do
                {
                    v38 = ((uint32)(((sint32)v37->x)) + (uint32)(((sint32)v37->width)));
                    if ((depth == 8))
                        v39 = ((uint32)((((sint32)(((uint32)(v38) - (uint32)(v14)))) & 0xFFFFFFC0)) + (uint32)(128));
                    else
                        v39 = ((uint32)((((sint32)(((uint32)(v38) - (uint32)(v14)))) & 0xFFFFFFC0)) + (uint32)(64));
                    v40 = (((sint32)(v39)) < ((sint32)(v38)));
                    if ((((!allocation_mode || ((((sint32)v37->y) + height) & 255)) && ((((sint32)(owner)) < 0) || !v40)) && (((((sint32)v37->width) == v14) && (((sint32)v37->height) == height)) || ((((((!v30 || ((((sint32)v30->width) != v14) && (((sint32)v30->height) != height))) || (((sint32)v37->width) == v14)) || (((sint32)v37->height) == height)) && (((sint32)v37->width) >= ((sint32)(v14)))) && (((sint32)v37->height) >= ((sint32)(height)))) && (!v30 || IsVramRecordAreaSmaller(v37, v30))))))
                    {
                        v30 = v37;
                    }
                    v37 = v37->next;
                } while (v37);
            }
            if (!v30)
            {
                v41 = free_rects;
                if (v41)
                {
                    do
                    {
                        if ((!allocation_mode || ((((sint32)v41->y) + height) & 255)) && (!v30 || (((sint32)v30->width) != v14 && ((sint32)v30->height) != height) || ((sint32)v41->width) == v14 || ((sint32)v41->height) == height) && (sint32)(((sint32)v41->width) - (((((sint32)v41->x) + 64) & 0xFFFFFFC0) - ((sint32)v41->x))) >= v14 && (sint32)((sint32)v41->height) >= (sint32)height && (!v30 || (sint32)((uint32)((sint32)v41->width) * ((sint32)v41->height)) < (sint32)((uint32)((sint32)v30->width) * ((sint32)v30->height))))
                        {
                            v30 = v41;
                        }
                        v41 = v41->next;
                    } while (v41);
                }
                if (!v30)
                {
                    GameDiagnostic("Out of VRAM whilst packing %x, (%d,%d)\n", name, v73, v74);
                    return 0;
                }
                v42 = ((sint32)v30->x);
                v43 = (((uint32)(v42) + (uint32)(64)) & 0xFFFFFFC0);
                v44 = ((sint32)v30->width);
                v45 = ((uint32)(v43) - (uint32)(v42));
                v46 = ((sint32)(((uint32)(v44) - (uint32)(v45))));
                if ((((sint32)(((sint32)(((uint32)(v44) - (uint32)(v45)))))) > 0))
                {
                    CreateFreeVramRectangle(v43, ((sint32)v30->y), ((sint32)(((uint32)(v44) - (uint32)(v45)))), ((sint32)v30->height));
                    v30->width = (uint16)(v30->width - (uint32)v46);
                    width = v12;
                }
                result = AllocateModelTextureRectangle(owner, width, height, x, y, depth, 0, name);
                result->depth = (uint8)(depth);
                result->flags = (uint8)(flags);
                result->owner = (uint8)(owner);
                goto LABEL_114;
            }
            v47 = v30;
            v48 = ((sint32)v30->x);
            v49 = ((sint32)v30->y);
            v50 = ((sint32)v47->width);
            v51 = ((sint32)v47->height);
            RemoveVramRecord(&free_rects, v47);
            v52 = CreateUsedVramRectangle(((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14)))), v49, v14, height);
            if ((v50 == v14))
            {
                v53 = ((sint32)(((uint32)(v49) + (uint32)(height))));
                if ((v51 == height))
                {
                LABEL_103:
                    StoreModelVramCoordinate(x, ((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14)))));

                LABEL_113:
                    StoreModelVramCoordinate(y, v49);

                    v52->depth = (uint8)(depth);
                    result = v52;
                    v52->flags = (uint8)(flags);
                    v52->owner = (uint8)(owner);
                LABEL_114:
                    v69 = allocation_mode;

                    result->source_width = (uint16)(v73);
                    v70 = v74;
                    result->mode = (uint8)(v69);
                    result->source_height = (uint16)(v70);
                    return result;
                }
                v55 = v48;
                v56 = v50;
            }
            else
            {
                v53 = v49;
                if ((v51 == height))
                {
                    v55 = v48;
                    v56 = ((sint32)(((uint32)(v50) - (uint32)(v14))));
                    v57 = v51;
                LABEL_102:
                    v58 = CreateFreeVramRectangle(v55, v53, v56, v57);

                    CoalesceFreeVramRectangles(v58);
                    goto LABEL_103;
                }
                v54 = CreateFreeVramRectangle(v48, v49, ((sint32)(((uint32)(v50) - (uint32)(v14)))), v51);
                CoalesceFreeVramRectangles(v54);
                v55 = ((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14))));
                v53 = ((sint32)(((uint32)(v49) + (uint32)(height))));
                v56 = v14;
            }
            v57 = ((sint32)(((uint32)(v51) - (uint32)(height))));
            goto LABEL_102;
        }
        v59 = v30;
        v60 = ((sint32)v30->x);
        v49 = ((sint32)v30->y);
        v61 = ((sint32)v59->width);
        v62 = ((sint32)v59->height);
        RemoveVramRecord(&free_rects, v59);
        v52 = CreateUsedVramRectangle(v60, v49, v14, height);
        if ((v61 == v14))
        {
            v63 = ((sint32)(((uint32)(v49) + (uint32)(height))));
            if ((v62 == height))
            {
            LABEL_112:
                StoreModelVramCoordinate(x, v60);

                goto LABEL_113;
            }
            v65 = v60;
            v66 = v61;
        }
        else
        {
            v63 = v49;
            if ((v62 == height))
            {
                v65 = ((sint32)(((uint32)(v60) + (uint32)(v14))));
                v66 = ((sint32)(((uint32)(v61) - (uint32)(v14))));
                v67 = v62;
            LABEL_111:
                v68 = CreateFreeVramRectangle(v65, v63, v66, v67);

                CoalesceFreeVramRectangles(v68);
                goto LABEL_112;
            }
            v64 = CreateFreeVramRectangle(((sint32)(((uint32)(v60) + (uint32)(v14)))), v49, ((sint32)(((uint32)(v61) - (uint32)(v14)))), v62);
            CoalesceFreeVramRectangles(v64);
            v65 = v60;
            v63 = ((sint32)(((uint32)(v49) + (uint32)(height))));
            v66 = v14;
        }
        v67 = ((sint32)(((uint32)(v62) - (uint32)(height))));
        goto LABEL_111;
    }
    if (((width == 256) || (height == 256)))
    {
        v15 = (AllocateModelTextureRectangle(owner, 256, 256, (uint32 *)&v71, (uint32 *)&v72, depth, 0, 0));
        v16 = 128;
        v17 = 256;
        if ((depth != 16))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) >> 1);
        if ((depth == 4))
        {
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) >> 1);
            v16 = 64;
        }
        v18 = ((sint32)v15->x);
        v19 = ((sint32)v15->y);
    }
    else
    {
        v15 = (AllocateModelTextureRectangle(owner, 2 * width, 2 * height, (uint32 *)&v71, (uint32 *)&v72, depth, 0, name));
        if ((depth != 16))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
        v16 = ((uint32)(2) * (uint32)(v14));
        if ((depth == 4))
        {
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
            v16 = ((uint32)(2) * (uint32)(v14));
        }
        v17 = ((uint32)(2) * (uint32)(height));
        v18 = ((sint32)v15->x);
        v19 = ((sint32)v15->y);
        v71 = (((uint32)(((sint32)(((uint32)(v71) + (uint32)(v14))))) - (uint32)(1)) & (0 - (uint32)v14));
        v72 = (((uint32)(((sint32)(((uint32)(v72) + (uint32)(height))))) - (uint32)(1)) & (0 - height));
    }
    v20 = v71;
    v21 = v72;
    v22 = v71;
    v15->width = (uint16)(v14);
    v15->height = (uint16)(height);
    v15->x = (uint16)(v20);
    v15->y = (uint16)(v21);
    StoreModelVramCoordinate(x, v22);
    StoreModelVramCoordinate(y, v72);
    if ((((sint32)(v18)) < ((sint32)(v71))))
    {
        v23 = CreateFreeVramRectangle(v18, v19, ((sint32)(((uint32)(v71) - (uint32)(v18)))), v17);
        CoalesceFreeVramRectangles(v23);
        v16 = (sint32)((uint32)v16 - ((uint32)v71 - (uint32)v18));
    }
    if ((((sint32)(v14)) < ((sint32)(v16))))
    {
        v24 = CreateFreeVramRectangle(((sint32)(((uint32)(v71) + (uint32)(v14)))), v19, ((sint32)(((uint32)(v16) - (uint32)(v14)))), v17);
        CoalesceFreeVramRectangles(v24);
    }
    if ((((sint32)(v19)) < ((sint32)(v72))))
    {
        v25 = CreateFreeVramRectangle(v71, v19, v14, ((sint32)(((uint32)(v72) - (uint32)(v19)))));
        CoalesceFreeVramRectangles(v25);
        v17 = (sint32)((uint32)v17 - ((uint32)v72 - (uint32)v19));
    }
    if ((((sint32)(height)) < ((sint32)(v17))))
    {
        v26 = CreateFreeVramRectangle(v71, ((sint32)(((uint32)(v72) + (uint32)(height)))), v14, ((sint32)(((uint32)(v17) - (uint32)(height)))));
        CoalesceFreeVramRectangles(v26);
    }
    v15->depth = (uint8)(depth);
    result = v15;
    v15->flags = (uint8)(flags);
    v15->owner = (uint8)(owner);
    v28 = allocation_mode;
    v15->source_width = (uint16)(v73);
    v29 = v74;
    v15->mode = (uint8)(v28);
    v15->source_height = (uint16)(v29);
    return result;
}

void RebuildModelVramAllocations()
{
    FUNCTION_MARKER(0x800691B8, "SLUS_003.73");
    while (free_rects)
        RemoveVramRecord(&free_rects, free_rects);
    CreateFreeVramRectangle(512, 256, 512, 256);
    CreateFreeVramRectangle(512, 0, 512, 256);
    VramRecord *copies = 0;
    for (VramRecord *node = used_rects; node; node = node->next)
    {
        VramRecord *copy = (VramRecord *)malloc(sizeof(VramRecord));
        if (!copy)
        {
            GameDiagnostic("Cannot rebuild texture VRAM allocations\n");
            abort();
        }
        *copy = *node;
        copy->next = copies;
        copies = copy;
    }
    while (used_rects)
        RemoveVramRecord(&used_rects, used_rects);
    uint32 coordinates[2];
    for (VramRecord *node = copies; node; node = node->next)
    {
        allocation_mode = node->mode;
        AllocateModelTextureRectangle((uint32)(sint32)node->owner, node->source_width, node->source_height, &coordinates[0], &coordinates[1], node->depth, node->flags, 0);
    }
    while (copies)
    {
        VramRecord *node = copies;
        copies = node->next;
        free(node);
    }
}
