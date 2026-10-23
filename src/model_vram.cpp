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
    bool mode;
    uint8 depth;
    uint8 flags;
    uint16 src_width;
    uint16 src_height;
};

static VramRecord *free_rects;
static VramRecord *used_rects;
static uint32 rec_count;
static bool allocation_mode;

void SetModelVramAllocationMode(bool mode)
{
    allocation_mode = mode;
}

static VramRecord *AllocateVramRec()
{
    FUNCTION_MARKER(0x80068240, "SLUS_003.73");
    VramRecord *rec = (VramRecord *)malloc(sizeof(*rec));
    if (!rec)
    {
        GameDiagnostic(("Cannot allocate a texture VRAM record\n"));
        abort();
    }
    ++rec_count;
    return rec;
}

static uint32 ReleaseVramRec(VramRecord *rec)
{
    FUNCTION_MARKER(0x80068270, "SLUS_003.73");
    free(rec);
    return --rec_count;
}

static VramRecord *PrependVramRec(VramRecord *&head)
{
    FUNCTION_MARKER(0x80068298, "SLUS_003.73");
    VramRecord *rec = AllocateVramRec();
    rec->next = head;
    head = rec;
    return rec;
}

static VramRecord *SetVramRect(VramRecord *rec, uint16 x, uint16 y, uint16 width, uint16 height)
{
    rec->x = x;
    rec->y = y;
    rec->width = width;
    rec->height = height;
    return rec;
}

static VramRecord *CreateFreeVramRect(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x800682CC, "SLUS_003.73");
    return SetVramRect(PrependVramRec(free_rects), x, y, width, height);
}

static VramRecord *CreateUsedVramRect(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x8006832C, "SLUS_003.73");
    return SetVramRect(PrependVramRec(used_rects), x, y, width, height);
}

static uint32 RemoveVramRec(VramRecord *&head, VramRecord *rec)
{
    FUNCTION_MARKER(0x8006838C, "SLUS_003.73");
    VramRecord *prev = head;
    if (prev == rec)
        head = rec->next;
    else
    {
        // The caller owns the record in this nonempty list
        while (prev->next != rec)
            prev = prev->next;
        prev->next = rec->next;
    }
    return ReleaseVramRec(rec);
}

void InitializeModelVramRects()
{
    FUNCTION_MARKER(0x80068404, "SLUS_003.73");
    while (free_rects)
        RemoveVramRec(free_rects, free_rects);
    while (used_rects)
        RemoveVramRec(used_rects, used_rects);
    rec_count = 0;
    CreateFreeVramRect(512, 256, 512, 256);
    CreateFreeVramRect(512, 0, 512, 256);
}

static VramRecord *MergeFreeVramRects(VramRecord *rect0, VramRecord *rect1)
{
    FUNCTION_MARKER(0x80068E80, "SLUS_003.73");
    uint32 x, y, width, height;
    if (rect0->y == rect1->y)
    {
        if (rect0->x < rect1->x)
        {
            if (rect0->x + rect0->width != rect1->x || rect0->height != rect1->height)
                return 0;
            x = rect0->x;
        }
        else if (rect1->x < rect0->x)
        {
            if (rect1->x + rect1->width != rect0->x || rect0->height != rect1->height)
                return 0;
            x = rect1->x;
        }
        else
            return 0;
        y = rect0->y;
        width = rect0->width + rect1->width;
        height = rect0->height;
    }
    else
    {
        if (rect0->x != rect1->x)
            return 0;
        if (rect0->y < rect1->y)
        {
            if (rect0->y + rect0->height != rect1->y || rect0->width != rect1->width)
                return 0;
            y = rect0->y;
        }
        else
        {
            if (rect1->y + rect1->height != rect0->y || rect0->width != rect1->width)
                return 0;
            y = rect1->y;
        }
        x = rect0->x;
        width = rect0->width;
        height = rect0->height + rect1->height;
    }
    RemoveVramRec(free_rects, rect0);
    RemoveVramRec(free_rects, rect1);
    return CreateFreeVramRect(x, y, width, height);
}

static bool AreVramRectsAdjacent(const VramRecord *rec, const VramRecord *candidate)
{
    if (rec->y == candidate->y && rec->height == candidate->height)
        return candidate->x + candidate->width == rec->x || rec->x + rec->width == candidate->x;
    if (rec->x != candidate->x || rec->width != candidate->width)
        return false;
    if ((candidate->y & 256) != (rec->y & 256))
        return false;
    return candidate->y + candidate->height == rec->y || rec->y + rec->height == candidate->y;
}

static bool CoalesceFreeVramRects(VramRecord *rec)
{
    FUNCTION_MARKER(0x800690A0, "SLUS_003.73");
    bool result = false;
    bool merged;
    do
    {
        VramRecord *candidate = free_rects;
        merged = false;
        while (candidate)
        {
            if (AreVramRectsAdjacent(rec, candidate))
            {
                rec = MergeFreeVramRects(candidate, rec);
                merged = true;
                result = true;
                break;
            }
            candidate = candidate->next;
        }
    } while (merged);
    return result;
}

bool ReleaseUsedVramRect(sint16 x, sint16 y)
{
    FUNCTION_MARKER(0x80068DC4, "SLUS_003.73");
    for (VramRecord *rec = used_rects; rec; rec = rec->next)
    {
        if (rec->x == x && rec->y == y)
        {
            uint16 rec_x = rec->x, rec_y = rec->y;
            uint16 width = rec->width, height = rec->height;
            RemoveVramRec(used_rects, rec);
            rec = CreateFreeVramRect(rec_x, rec_y, width, height);
            return CoalesceFreeVramRects(rec);
        }
    }
    return false;
}

static bool IsVramRecAreaSmaller(const VramRecord *candidate, const VramRecord *sel)
{
    uint32 width, height, sel_width, sel_height;
    width = candidate->width;
    height = candidate->height;
    sel_width = sel->width;
    sel_height = sel->height;
    return (sint32)(width * height) < (sint32)(sel_width * sel_height);
}

VramRecord *AllocateModelTexRect(sint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 resource_key)
{
    sint32 width_words;
    VramRecord *aligned_rec;
    sint32 remaining_width;
    sint32 remaining_height;
    sint32 reserve_x;
    sint32 reserve_y;
    VramRecord *left_padding;
    VramRecord *right_padding;
    VramRecord *top_padding;
    VramRecord *bottom_padding;
    VramRecord *result;
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
    sint32 right_split_x;
    sint32 split_y;
    sint32 right_split_width;
    sint32 right_split_height;
    uint32 right_allocation_x;
    VramRecord *allocated;
    sint32 right_remainder_y;
    VramRecord *left_of_allocation;
    sint32 right_remainder_x;
    sint32 right_remainder_width;
    sint32 right_remainder_height;
    VramRecord *right_split_remainder;
    sint32 left_split_x;
    sint32 left_split_width;
    sint32 left_split_height;
    sint32 left_remainder_y;
    VramRecord *right_of_allocation;
    sint32 left_remainder_x;
    sint32 left_remainder_width;
    sint32 left_remainder_height;
    VramRecord *left_split_remainder;
    uint32 aligned_pos[2];
    sint32 requested_width;
    sint32 requested_height;
    FUNCTION_MARKER(0x80068450, "SLUS_003.73");
    // Signed views retain MIPS SLT and DIV on wrapping dimension words
    requested_width = width;
    width_words = width;
    requested_height = height;
    if (!flags)
        goto SearchFreeRectangles;
    if (width == 256 && height == 256)
        flags = 0;
    if (!flags)
    {
    SearchFreeRectangles:
        if (depth != 16)
            width_words = ((sint32)(width + 1)) / 2;

        if (depth == 4)
            width_words = ((sint32)((uint32)width_words + 1)) / 2;
        candidate = free_rects;
        sel = 0;
        if (candidate)
        {
            do
            {
                candidate_x = candidate->x;
                candidate_right = (uint32)candidate_x + width_words;
                if (depth == 8)
                    page_right = (candidate_x & 0xFFFFFFC0) + 128;
                else
                    page_right = (candidate_x & 0xFFFFFFC0) + 64;
                crosses_page = page_right < candidate_right;
                if ((!allocation_mode || ((candidate->y + height) & 255)) && (owner < 0 || !crosses_page))
                {
                    candidate_width = candidate->width;
                    if (candidate_width == width_words && candidate->height == height)
                    {
                        sel = candidate;
                        break;
                    }
                    if ((!sel || (sel->width != width_words && sel->height != height) || candidate_width == width_words || candidate->height == height) && candidate->width >= width_words && candidate->height >= (sint32)height && (!sel || IsVramRecAreaSmaller(candidate, sel)))
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
                    free_right = right_candidate->x + right_candidate->width;
                    if (depth == 8)
                        right_page_limit = (((uint32)free_right - width_words) & 0xFFFFFFC0) + 128;
                    else
                        right_page_limit = (((uint32)free_right - width_words) & 0xFFFFFFC0) + 64;
                    right_crosses_page = right_page_limit < free_right;
                    if ((!allocation_mode || ((right_candidate->y + height) & 255)) && (owner < 0 || !right_crosses_page) && ((right_candidate->width == width_words && right_candidate->height == height) || ((!sel || (sel->width != width_words && sel->height != height) || right_candidate->width == width_words || right_candidate->height == height) && right_candidate->width >= width_words && right_candidate->height >= (sint32)height && (!sel || IsVramRecAreaSmaller(right_candidate, sel)))))
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
                        if ((!allocation_mode || ((aligned_candidate->y + height) & 255)) && (!sel || (sel->width != width_words && sel->height != height) || aligned_candidate->width == width_words || aligned_candidate->height == height) && (sint32)(aligned_candidate->width - (((aligned_candidate->x + 64) & 0xFFFFFFC0) - aligned_candidate->x)) >= width_words && aligned_candidate->height >= (sint32)height && (!sel || IsVramRecAreaSmaller(aligned_candidate, sel)))
                        {
                            sel = aligned_candidate;
                        }
                        aligned_candidate = aligned_candidate->next;
                    } while (aligned_candidate);
                }
                if (!sel)
                {
                    GameDiagnostic(("Out of VRAM whilst packing %x, (%d,%d)\n", resource_key, requested_width, requested_height));
                    return 0;
                }
                sel_x = sel->x;
                alignment_x = (sel_x + 64) & 0xFFFFFFC0;
                sel_width = sel->width;
                alignment_padding = alignment_x - sel_x;
                aligned_remainder_width = sel_width - alignment_padding;
                if (aligned_remainder_width > 0)
                {
                    CreateFreeVramRect(alignment_x, sel->y, aligned_remainder_width, sel->height);
                    sel->width -= aligned_remainder_width;
                    width = requested_width;
                }
                result = AllocateModelTexRect(owner, width, height, x, y, depth, 0, resource_key);
                if (!result)
                {
                    GameDiagnostic(("Recursive texture VRAM allocation failed\n"));
                    abort();
                }
                result->depth = depth;
                result->flags = flags;
                result->owner = owner;
                goto StoreSourceDimensions;
            }
            right_split_x = sel->x;
            split_y = sel->y;
            right_split_width = sel->width;
            right_split_height = sel->height;
            RemoveVramRec(free_rects, sel);
            right_allocation_x = (uint32)right_split_x + right_split_width - width_words;
            allocated = CreateUsedVramRect(right_allocation_x, split_y, width_words, height);
            if (right_split_width == width_words)
            {
                right_remainder_y = (uint32)split_y + height;
                if (right_split_height == height)
                {
                StoreRightAlignedX:
                    *x = right_allocation_x;

                StoreYAndMetadata:
                    *y = split_y;

                    allocated->depth = depth;
                    result = allocated;
                    allocated->flags = flags;
                    allocated->owner = owner;
                StoreSourceDimensions:
                    result->src_width = requested_width;
                    result->mode = allocation_mode;
                    result->src_height = requested_height;
                    return result;
                }
                right_remainder_x = right_split_x;
                right_remainder_width = right_split_width;
            }
            else
            {
                right_remainder_y = split_y;
                if (right_split_height == height)
                {
                    right_remainder_x = right_split_x;
                    right_remainder_width = (uint32)right_split_width - width_words;
                    right_remainder_height = right_split_height;
                CoalesceRightRemainder:
                    right_split_remainder = CreateFreeVramRect(right_remainder_x, right_remainder_y, right_remainder_width, right_remainder_height);

                    CoalesceFreeVramRects(right_split_remainder);
                    goto StoreRightAlignedX;
                }
                left_of_allocation = CreateFreeVramRect(right_split_x, split_y, (uint32)right_split_width - width_words, right_split_height);
                CoalesceFreeVramRects(left_of_allocation);
                right_remainder_x = right_allocation_x;
                right_remainder_y = (uint32)split_y + height;
                right_remainder_width = width_words;
            }
            right_remainder_height = (uint32)right_split_height - height;
            goto CoalesceRightRemainder;
        }
        left_split_x = sel->x;
        split_y = sel->y;
        left_split_width = sel->width;
        left_split_height = sel->height;
        RemoveVramRec(free_rects, sel);
        allocated = CreateUsedVramRect(left_split_x, split_y, width_words, height);
        if (left_split_width == width_words)
        {
            left_remainder_y = (uint32)split_y + height;
            if (left_split_height == height)
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
            if (left_split_height == height)
            {
                left_remainder_x = (uint32)left_split_x + width_words;
                left_remainder_width = (uint32)left_split_width - width_words;
                left_remainder_height = left_split_height;
            CoalesceLeftRemainder:
                left_split_remainder = CreateFreeVramRect(left_remainder_x, left_remainder_y, left_remainder_width, left_remainder_height);

                CoalesceFreeVramRects(left_split_remainder);
                goto StoreLeftAlignedX;
            }
            right_of_allocation = CreateFreeVramRect((uint32)left_split_x + width_words, split_y, (uint32)left_split_width - width_words, left_split_height);
            CoalesceFreeVramRects(right_of_allocation);
            left_remainder_x = left_split_x;
            left_remainder_y = (uint32)split_y + height;
            left_remainder_width = width_words;
        }
        left_remainder_height = (uint32)left_split_height - height;
        goto CoalesceLeftRemainder;
    }
    if (width == 256 || height == 256)
    {
        aligned_rec = AllocateModelTexRect(owner, 256, 256, &aligned_pos[0], &aligned_pos[1], depth, 0, 0);
        if (!aligned_rec)
        {
            GameDiagnostic(("Aligned texture VRAM allocation failed\n"));
            abort();
        }
        remaining_width = 128;
        remaining_height = 256;
        if (depth != 16)
            width_words = (sint32)((uint32)width_words + 1) >> 1;
        if (depth == 4)
        {
            width_words = (sint32)((uint32)width_words + 1) >> 1;
            remaining_width = 64;
        }
        reserve_x = aligned_rec->x;
        reserve_y = aligned_rec->y;
    }
    else
    {
        aligned_rec = AllocateModelTexRect(owner, 2 * width, 2 * height, &aligned_pos[0], &aligned_pos[1], depth, 0, resource_key);
        if (!aligned_rec)
        {
            GameDiagnostic(("Aligned texture VRAM allocation failed\n"));
            abort();
        }
        if (depth != 16)
            width_words = (sint32)((uint32)width_words + 1) / 2;
        remaining_width = 2 * (uint32)width_words;
        if (depth == 4)
        {
            width_words = (sint32)((uint32)width_words + 1) / 2;
            remaining_width = 2 * (uint32)width_words;
        }
        remaining_height = 2 * height;
        reserve_x = aligned_rec->x;
        reserve_y = aligned_rec->y;
        aligned_pos[0] = (aligned_pos[0] + width_words - 1) & -(uint32)width_words;
        aligned_pos[1] = (aligned_pos[1] + height - 1) & -height;
    }
    aligned_rec->width = width_words;
    aligned_rec->height = height;
    aligned_rec->x = aligned_pos[0];
    aligned_rec->y = aligned_pos[1];
    *x = aligned_pos[0];
    *y = aligned_pos[1];
    if (reserve_x < (sint32)aligned_pos[0])
    {
        left_padding = CreateFreeVramRect(reserve_x, reserve_y, aligned_pos[0] - reserve_x, remaining_height);
        CoalesceFreeVramRects(left_padding);
        remaining_width = (uint32)remaining_width - (aligned_pos[0] - reserve_x);
    }
    if (width_words < remaining_width)
    {
        right_padding = CreateFreeVramRect(aligned_pos[0] + width_words, reserve_y, (uint32)remaining_width - width_words, remaining_height);
        CoalesceFreeVramRects(right_padding);
    }
    if (reserve_y < (sint32)aligned_pos[1])
    {
        top_padding = CreateFreeVramRect(aligned_pos[0], reserve_y, width_words, aligned_pos[1] - reserve_y);
        CoalesceFreeVramRects(top_padding);
        remaining_height = (uint32)remaining_height - (aligned_pos[1] - reserve_y);
    }
    if ((sint32)height < remaining_height)
    {
        bottom_padding = CreateFreeVramRect(aligned_pos[0], aligned_pos[1] + height, width_words, (uint32)remaining_height - height);
        CoalesceFreeVramRects(bottom_padding);
    }
    aligned_rec->depth = depth;
    result = aligned_rec;
    aligned_rec->flags = flags;
    aligned_rec->owner = owner;
    aligned_rec->src_width = requested_width;
    aligned_rec->mode = allocation_mode;
    aligned_rec->src_height = requested_height;
    return result;
}

void RebuildModelVramAllocations()
{
    FUNCTION_MARKER(0x800691B8, "SLUS_003.73");
    while (free_rects)
        RemoveVramRec(free_rects, free_rects);
    CreateFreeVramRect(512, 256, 512, 256);
    CreateFreeVramRect(512, 0, 512, 256);
    VramRecord *copies = 0;
    for (VramRecord *node = used_rects; node; node = node->next)
    {
        VramRecord *copy = (VramRecord *)malloc(sizeof(*copy));
        if (!copy)
        {
            GameDiagnostic(("Cannot rebuild texture VRAM allocations\n"));
            abort();
        }
        *copy = *node;
        copy->next = copies;
        copies = copy;
    }
    while (used_rects)
        RemoveVramRec(used_rects, used_rects);
    uint32 coords[2];
    for (VramRecord *node = copies; node; node = node->next)
    {
        allocation_mode = node->mode;
        AllocateModelTexRect(node->owner, node->src_width, node->src_height, &coords[0], &coords[1], node->depth, node->flags, 0);
    }
    while (copies)
    {
        VramRecord *node = copies;
        copies = node->next;
        free(node);
    }
}
