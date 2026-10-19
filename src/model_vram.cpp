#include <stdlib.h>

#include "game_diagnostics.h"
#include "model_vram.h"

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
    uint16 src_width;
    uint16 src_height;
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

static VramRecord *MergeFreeVramRectangles(VramRecord *rect0, VramRecord *rect1)
{
    FUNCTION_MARKER(0x80068E80, "SLUS_003.73");
    uint32 y0 = rect0->y;
    uint32 y1 = rect1->y;
    uint32 result, x, y, width, height;
    if (y0 == y1)
    {
        uint32 x0 = rect0->x;
        uint32 x1 = rect1->x;
        result = x1 < x0;
        if (x0 < x1)
        {
            uint32 width0 = rect0->width;
            result = x0 + width0;
            if (result != x1)
                return 0;
            uint32 height0 = rect0->height;
            result = rect1->height;
            if (height0 != result)
                return 0;
            x = x0;
            y = rect0->y;
            uint32 width1 = rect1->width;
            height = rect0->height;
            width = width0 + width1;
        }
        else if (x1 < x0)
        {
            x1 = rect1->x;
            uint32 width1 = rect1->width;
            x0 = rect0->x;
            result = x1 + width1;
            if (result != x0)
                return 0;
            uint32 height0 = rect0->height;
            result = rect1->height;
            if (height0 != result)
                return 0;
            x = x1;
            y = rect1->y;
            uint32 width0 = rect0->width;
            height = rect0->height;
            width = width0 + width1;
        }
        else
            return 0;
    }
    else
    {
        uint32 x0 = rect0->x;
        result = rect1->x;
        if (x0 != result)
            return 0;
        result = y1 < y0;
        if (y0 < y1)
        {
            y0 = rect0->y;
            uint32 height0 = rect0->height;
            y1 = rect1->y;
            result = y0 + height0;
            if (result != y1)
                return 0;
            uint32 width0 = rect0->width;
            result = rect1->width;
            if (width0 != result)
                return 0;
            y = y0;
            x = rect0->x;
            uint32 height1 = rect1->height;
            width = rect0->width;
            height = height0 + height1;
        }
        else if (y1 < y0)
        {
            y1 = rect1->y;
            uint32 height1 = rect1->height;
            y0 = rect0->y;
            result = y1 + height1;
            if (result != y0)
                return 0;
            uint32 width0 = rect0->width;
            result = rect1->width;
            if (width0 != result)
                return 0;
            y = y1;
            x = rect1->x;
            uint32 height0 = rect0->height;
            width = rect0->width;
            height = height0 + height1;
        }
        else
            return 0;
    }
    RemoveVramRecord(&free_rects, rect0);
    RemoveVramRecord(&free_rects, rect1);
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
    uint32 width, height, sel_width, selected_height;
    width = candidate->width;
    height = candidate->height;
    sel_width = selected->width;
    selected_height = selected->height;
    return (sint32)(width * height) < (sint32)(sel_width * selected_height);
}

VramRecord *AllocateModelTextureRectangle(uint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 resource_key)
{
    sint32 width_words;
    VramRecord *aligned_record;
    sint32 remaining_width;
    sint32 remaining_height;
    sint32 reserve_x;
    sint32 reserve_y;
    uint16 aligned_x_half;
    uint16 aligned_y_half;
    sint32 aligned_x_output;
    VramRecord *left_padding;
    VramRecord *right_padding;
    VramRecord *top_padding;
    VramRecord *bottom_padding;
    VramRecord *result;
    sint8 aligned_mode;
    uint16 aligned_source_height;
    VramRecord *sel;
    VramRecord *candidate;
    sint32 candidate_x;
    sint32 candidate_right;
    sint32 page_right;
    sint32 crosses_page;
    sint32 candidate_width;
    VramRecord *right_candidate;
    sint32 free_right;
    sint32 right_page_limit;
    sint32 right_crosses_page;
    VramRecord *aligned_candidate;
    sint32 sel_x;
    uint32 alignment_x;
    sint32 sel_width;
    sint32 alignment_padding;
    sint32 aligned_remainder_width;
    VramRecord *right_split_record;
    sint32 right_split_x;
    sint32 split_y;
    sint32 right_split_width;
    sint32 right_split_height;
    VramRecord *allocated;
    sint32 right_remainder_y;
    VramRecord *left_of_allocation;
    sint32 right_remainder_x;
    sint32 right_remainder_width;
    sint32 right_remainder_height;
    VramRecord *right_split_remainder;
    VramRecord *left_split_record;
    sint32 left_split_x;
    sint32 left_split_width;
    sint32 left_split_height;
    sint32 left_remainder_y;
    VramRecord *right_of_allocation;
    sint32 left_remainder_x;
    sint32 left_remainder_width;
    sint32 left_remainder_height;
    VramRecord *left_split_remainder;
    sint8 mode_byte;
    sint16 source_height_half;
    uint32 aligned_pos[2];
    sint32 requested_width;
    sint32 requested_height;
    FUNCTION_MARKER(0x80068450, "SLUS_003.73");
    // TODO Audit extreme signed dimensions
    // TODO Original recursive allocation failure is dereferenced without a null check
    requested_width = width;
    width_words = width;
    requested_height = height;
    if (!flags)
        goto SearchFreeRectangles;
    if (((width == 256) && (height == 256)))
        flags = 0;
    if (!flags)
    {
    SearchFreeRectangles:
        if ((depth != 16))
            width_words = (((sint32)(((uint32)(width) + (uint32)(1)))) / 2);

        if ((depth == 4))
            width_words = (((sint32)(((uint32)(width_words) + (uint32)(1)))) / 2);
        candidate = free_rects;
        sel = 0;
        if (candidate)
        {
            do
            {
                candidate_x = ((sint32)candidate->x);
                candidate_right = ((sint32)(((uint32)(candidate_x) + (uint32)(width_words))));
                if ((depth == 8))
                    page_right = ((uint32)((candidate_x & 0xFFFFFFC0)) + (uint32)(128));
                else
                    page_right = ((uint32)((candidate_x & 0xFFFFFFC0)) + (uint32)(64));
                crosses_page = (((sint32)(page_right)) < ((sint32)(candidate_right)));
                if (((!allocation_mode || ((((sint32)candidate->y) + height) & 255)) && ((((sint32)(owner)) < 0) || !crosses_page)))
                {
                    candidate_width = ((sint32)candidate->width);
                    if (((candidate_width == width_words) && (((sint32)candidate->height) == height)))
                    {
                        sel = candidate;
                        break;
                    }
                    if (((((((!sel || ((((sint32)sel->width) != width_words) && (((sint32)sel->height) != height))) || (candidate_width == width_words)) || (((sint32)candidate->height) == height)) && (((sint32)candidate->width) >= ((sint32)(width_words)))) && (((sint32)candidate->height) >= ((sint32)(height)))) && (!sel || IsVramRecordAreaSmaller(candidate, sel))))
                    {
                        sel = candidate;
                    }
                }
                candidate = candidate->next;
            } while (candidate);
        }
        if (!sel)
        {
            right_candidate = free_rects;
            if (right_candidate)
            {
                do
                {
                    free_right = ((uint32)(((sint32)right_candidate->x)) + (uint32)(((sint32)right_candidate->width)));
                    if ((depth == 8))
                        right_page_limit = ((uint32)((((sint32)(((uint32)(free_right) - (uint32)(width_words)))) & 0xFFFFFFC0)) + (uint32)(128));
                    else
                        right_page_limit = ((uint32)((((sint32)(((uint32)(free_right) - (uint32)(width_words)))) & 0xFFFFFFC0)) + (uint32)(64));
                    right_crosses_page = (((sint32)(right_page_limit)) < ((sint32)(free_right)));
                    if ((((!allocation_mode || ((((sint32)right_candidate->y) + height) & 255)) && ((((sint32)(owner)) < 0) || !right_crosses_page)) && (((((sint32)right_candidate->width) == width_words) && (((sint32)right_candidate->height) == height)) || ((((((!sel || ((((sint32)sel->width) != width_words) && (((sint32)sel->height) != height))) || (((sint32)right_candidate->width) == width_words)) || (((sint32)right_candidate->height) == height)) && (((sint32)right_candidate->width) >= ((sint32)(width_words)))) && (((sint32)right_candidate->height) >= ((sint32)(height)))) && (!sel || IsVramRecordAreaSmaller(right_candidate, sel))))))
                    {
                        sel = right_candidate;
                    }
                    right_candidate = right_candidate->next;
                } while (right_candidate);
            }
            if (!sel)
            {
                aligned_candidate = free_rects;
                if (aligned_candidate)
                {
                    do
                    {
                        if ((!allocation_mode || ((((sint32)aligned_candidate->y) + height) & 255)) && (!sel || (((sint32)sel->width) != width_words && ((sint32)sel->height) != height) || ((sint32)aligned_candidate->width) == width_words || ((sint32)aligned_candidate->height) == height) && (sint32)(((sint32)aligned_candidate->width) - (((((sint32)aligned_candidate->x) + 64) & 0xFFFFFFC0) - ((sint32)aligned_candidate->x))) >= width_words && (sint32)((sint32)aligned_candidate->height) >= (sint32)height && (!sel || (sint32)((uint32)((sint32)aligned_candidate->width) * ((sint32)aligned_candidate->height)) < (sint32)((uint32)((sint32)sel->width) * ((sint32)sel->height))))
                        {
                            sel = aligned_candidate;
                        }
                        aligned_candidate = aligned_candidate->next;
                    } while (aligned_candidate);
                }
                if (!sel)
                {
                    GameDiagnostic("Out of VRAM whilst packing %x, (%d,%d)\n", resource_key, requested_width, requested_height);
                    return 0;
                }
                sel_x = ((sint32)sel->x);
                alignment_x = (((uint32)(sel_x) + (uint32)(64)) & 0xFFFFFFC0);
                sel_width = ((sint32)sel->width);
                alignment_padding = ((uint32)(alignment_x) - (uint32)(sel_x));
                aligned_remainder_width = ((sint32)(((uint32)(sel_width) - (uint32)(alignment_padding))));
                if ((((sint32)(((sint32)(((uint32)(sel_width) - (uint32)(alignment_padding)))))) > 0))
                {
                    CreateFreeVramRectangle(alignment_x, ((sint32)sel->y), ((sint32)(((uint32)(sel_width) - (uint32)(alignment_padding)))), ((sint32)sel->height));
                    sel->width = (uint16)(sel->width - (uint32)aligned_remainder_width);
                    width = requested_width;
                }
                result = AllocateModelTextureRectangle(owner, width, height, x, y, depth, 0, resource_key);
                result->depth = (uint8)(depth);
                result->flags = (uint8)(flags);
                result->owner = (uint8)(owner);
                goto StoreSourceDimensions;
            }
            right_split_record = sel;
            right_split_x = ((sint32)sel->x);
            split_y = ((sint32)sel->y);
            right_split_width = ((sint32)right_split_record->width);
            right_split_height = ((sint32)right_split_record->height);
            RemoveVramRecord(&free_rects, right_split_record);
            allocated = CreateUsedVramRectangle(((sint32)(((uint32)(((sint32)(((uint32)(right_split_x) + (uint32)(right_split_width))))) - (uint32)(width_words)))), split_y, width_words, height);
            if ((right_split_width == width_words))
            {
                right_remainder_y = ((sint32)(((uint32)(split_y) + (uint32)(height))));
                if ((right_split_height == height))
                {
                StoreRightAlignedX:
                    *x = ((sint32)(((uint32)(((sint32)(((uint32)(right_split_x) + (uint32)(right_split_width))))) - (uint32)(width_words))));

                StoreYAndMetadata:
                    *y = split_y;

                    allocated->depth = (uint8)(depth);
                    result = allocated;
                    allocated->flags = (uint8)(flags);
                    allocated->owner = (uint8)(owner);
                StoreSourceDimensions:
                    mode_byte = allocation_mode;

                    result->src_width = (uint16)(requested_width);
                    source_height_half = requested_height;
                    result->mode = (uint8)(mode_byte);
                    result->src_height = (uint16)(source_height_half);
                    return result;
                }
                right_remainder_x = right_split_x;
                right_remainder_width = right_split_width;
            }
            else
            {
                right_remainder_y = split_y;
                if ((right_split_height == height))
                {
                    right_remainder_x = right_split_x;
                    right_remainder_width = ((sint32)(((uint32)(right_split_width) - (uint32)(width_words))));
                    right_remainder_height = right_split_height;
                CoalesceRightRemainder:
                    right_split_remainder = CreateFreeVramRectangle(right_remainder_x, right_remainder_y, right_remainder_width, right_remainder_height);

                    CoalesceFreeVramRectangles(right_split_remainder);
                    goto StoreRightAlignedX;
                }
                left_of_allocation = CreateFreeVramRectangle(right_split_x, split_y, ((sint32)(((uint32)(right_split_width) - (uint32)(width_words)))), right_split_height);
                CoalesceFreeVramRectangles(left_of_allocation);
                right_remainder_x = ((sint32)(((uint32)(((sint32)(((uint32)(right_split_x) + (uint32)(right_split_width))))) - (uint32)(width_words))));
                right_remainder_y = ((sint32)(((uint32)(split_y) + (uint32)(height))));
                right_remainder_width = width_words;
            }
            right_remainder_height = ((sint32)(((uint32)(right_split_height) - (uint32)(height))));
            goto CoalesceRightRemainder;
        }
        left_split_record = sel;
        left_split_x = ((sint32)sel->x);
        split_y = ((sint32)sel->y);
        left_split_width = ((sint32)left_split_record->width);
        left_split_height = ((sint32)left_split_record->height);
        RemoveVramRecord(&free_rects, left_split_record);
        allocated = CreateUsedVramRectangle(left_split_x, split_y, width_words, height);
        if ((left_split_width == width_words))
        {
            left_remainder_y = ((sint32)(((uint32)(split_y) + (uint32)(height))));
            if ((left_split_height == height))
            {
            StoreLeftAlignedX:
                *x = left_split_x;

                goto StoreYAndMetadata;
            }
            left_remainder_x = left_split_x;
            left_remainder_width = left_split_width;
        }
        else
        {
            left_remainder_y = split_y;
            if ((left_split_height == height))
            {
                left_remainder_x = ((sint32)(((uint32)(left_split_x) + (uint32)(width_words))));
                left_remainder_width = ((sint32)(((uint32)(left_split_width) - (uint32)(width_words))));
                left_remainder_height = left_split_height;
            CoalesceLeftRemainder:
                left_split_remainder = CreateFreeVramRectangle(left_remainder_x, left_remainder_y, left_remainder_width, left_remainder_height);

                CoalesceFreeVramRectangles(left_split_remainder);
                goto StoreLeftAlignedX;
            }
            right_of_allocation = CreateFreeVramRectangle(((sint32)(((uint32)(left_split_x) + (uint32)(width_words)))), split_y, ((sint32)(((uint32)(left_split_width) - (uint32)(width_words)))), left_split_height);
            CoalesceFreeVramRectangles(right_of_allocation);
            left_remainder_x = left_split_x;
            left_remainder_y = ((sint32)(((uint32)(split_y) + (uint32)(height))));
            left_remainder_width = width_words;
        }
        left_remainder_height = ((sint32)(((uint32)(left_split_height) - (uint32)(height))));
        goto CoalesceLeftRemainder;
    }
    if (((width == 256) || (height == 256)))
    {
        aligned_record = (AllocateModelTextureRectangle(owner, 256, 256, &aligned_pos[0], &aligned_pos[1], depth, 0, 0));
        remaining_width = 128;
        remaining_height = 256;
        if ((depth != 16))
            width_words = (((sint32)(((uint32)(width_words) + (uint32)(1)))) >> 1);
        if ((depth == 4))
        {
            width_words = (((sint32)(((uint32)(width_words) + (uint32)(1)))) >> 1);
            remaining_width = 64;
        }
        reserve_x = ((sint32)aligned_record->x);
        reserve_y = ((sint32)aligned_record->y);
    }
    else
    {
        aligned_record = (AllocateModelTextureRectangle(owner, 2 * width, 2 * height, &aligned_pos[0], &aligned_pos[1], depth, 0, resource_key));
        if ((depth != 16))
            width_words = (((sint32)(((uint32)(width_words) + (uint32)(1)))) / 2);
        remaining_width = ((uint32)(2) * (uint32)(width_words));
        if ((depth == 4))
        {
            width_words = (((sint32)(((uint32)(width_words) + (uint32)(1)))) / 2);
            remaining_width = ((uint32)(2) * (uint32)(width_words));
        }
        remaining_height = ((uint32)(2) * (uint32)(height));
        reserve_x = ((sint32)aligned_record->x);
        reserve_y = ((sint32)aligned_record->y);
        aligned_pos[0] = (((uint32)(((sint32)(((uint32)(aligned_pos[0]) + (uint32)(width_words))))) - (uint32)(1)) & (0 - (uint32)width_words));
        aligned_pos[1] = (((uint32)(((sint32)(((uint32)(aligned_pos[1]) + (uint32)(height))))) - (uint32)(1)) & (0 - height));
    }
    aligned_x_half = aligned_pos[0];
    aligned_y_half = aligned_pos[1];
    aligned_x_output = aligned_pos[0];
    aligned_record->width = (uint16)(width_words);
    aligned_record->height = (uint16)(height);
    aligned_record->x = (uint16)(aligned_x_half);
    aligned_record->y = (uint16)(aligned_y_half);
    *x = aligned_x_output;
    *y = aligned_pos[1];
    if ((((sint32)(reserve_x)) < ((sint32)(aligned_pos[0]))))
    {
        left_padding = CreateFreeVramRectangle(reserve_x, reserve_y, ((sint32)(((uint32)(aligned_pos[0]) - (uint32)(reserve_x)))), remaining_height);
        CoalesceFreeVramRectangles(left_padding);
        remaining_width = (sint32)((uint32)remaining_width - ((uint32)aligned_pos[0] - (uint32)reserve_x));
    }
    if ((((sint32)(width_words)) < ((sint32)(remaining_width))))
    {
        right_padding = CreateFreeVramRectangle(((sint32)(((uint32)(aligned_pos[0]) + (uint32)(width_words)))), reserve_y, ((sint32)(((uint32)(remaining_width) - (uint32)(width_words)))), remaining_height);
        CoalesceFreeVramRectangles(right_padding);
    }
    if ((((sint32)(reserve_y)) < ((sint32)(aligned_pos[1]))))
    {
        top_padding = CreateFreeVramRectangle(aligned_pos[0], reserve_y, width_words, ((sint32)(((uint32)(aligned_pos[1]) - (uint32)(reserve_y)))));
        CoalesceFreeVramRectangles(top_padding);
        remaining_height = (sint32)((uint32)remaining_height - ((uint32)aligned_pos[1] - (uint32)reserve_y));
    }
    if ((((sint32)(height)) < ((sint32)(remaining_height))))
    {
        bottom_padding = CreateFreeVramRectangle(aligned_pos[0], ((sint32)(((uint32)(aligned_pos[1]) + (uint32)(height)))), width_words, ((sint32)(((uint32)(remaining_height) - (uint32)(height)))));
        CoalesceFreeVramRectangles(bottom_padding);
    }
    aligned_record->depth = (uint8)(depth);
    result = aligned_record;
    aligned_record->flags = (uint8)(flags);
    aligned_record->owner = (uint8)(owner);
    aligned_mode = allocation_mode;
    aligned_record->src_width = (uint16)(requested_width);
    aligned_source_height = requested_height;
    aligned_record->mode = (uint8)(aligned_mode);
    aligned_record->src_height = (uint16)(aligned_source_height);
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
        AllocateModelTextureRectangle((uint32)(sint32)node->owner, node->src_width, node->src_height, &coordinates[0], &coordinates[1], node->depth, node->flags, 0);
    }
    while (copies)
    {
        VramRecord *node = copies;
        copies = node->next;
        free(node);
    }
}
