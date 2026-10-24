#include <stdlib.h>
#include <string.h>

#include "animated_hud_text.h"
#include "hud_text.h"
#include "global.h"
#include "game_diagnostics.h"

static AnimatedHudText *animated_hud_text;

AnimatedHudText *AnimatedHudText::Allocate()
{
    FUNCTION_MARKER(0x8001B654, "SLUS_003.73");
    AnimatedHudText *node = (AnimatedHudText *)malloc(sizeof(*node));
    if (!node)
    {
        GameDiagnostic(("Cannot allocate animated HUD text\n"));
        abort();
    }
    AnimatedHudText *prev = animated_hud_text;
    memset(&node->hd_text, 0, sizeof(node->hd_text));
    node->prev = 0;
    animated_hud_text = node;
    node->next = prev;
    if (prev)
        prev->prev = node;
    return node;
}

void AnimatedHudText::Remove()
{
    FUNCTION_MARKER(0x8001B694, "SLUS_003.73");
    AnimatedHudText *next = this->next;
    if (next)
        next->prev = this->prev;
    AnimatedHudText *prev = this->prev;
    if (prev)
        prev->next = this->next;
    if (this == animated_hud_text)
        animated_hud_text = this->next;
    free(this);
}

void AnimatedHudText::ClearAll()
{
    FUNCTION_MARKER(0x8001B708, "SLUS_003.73");
    AnimatedHudText *node = animated_hud_text;
    while (node)
    {
        AnimatedHudText *next = node->next;
        free(node);
        node = next;
    }
    animated_hud_text = 0;
}

void AnimatedHudText::Add(const char *text, const HudTextAnimation *animation)
{
    FUNCTION_MARKER(0x8001B788, "SLUS_003.73");
    AnimatedHudText *node = AnimatedHudText::Allocate();
    node->text = text;
    node->animation = animation;
    node->age = 0;
    node->hd_previous_scale = GetHdScale(*animation, 0);
}

uint32 AnimatedHudText::UpdateAll()
{
    FUNCTION_MARKER(0x8001B7C4, "SLUS_003.73");
    AnimatedHudText *node = animated_hud_text;
    while (node)
    {
        AnimatedHudText *next = node->next;
        const HudTextAnimation *animation = node->animation;
        uint16 age = node->age;
        node->hd_previous_scale = GetHdScale(*animation, age);
        if (age < animation->duration)
            node->age = age + 1;
        else
        {
            sint8 step = animation->step;
            if (!step)
                node->Remove();
            else if (step != 127)
            {
                node->animation = animation + step;
                node->age = 0;
            }
        }
        node = next;
    }
    uint32 timer = hud_text_glow.timer;
    if (timer)
    {
        --timer;
        hud_text_glow.timer = timer;
    }
    else
    {
        timer = hud_text_glow.alternate ? RandomRange(16) + 16 : RandomRange(24);
        hud_text_glow.timer = timer;
        hud_text_glow.alternate = 0;
    }
    return timer;
}

uint32 AnimatedHudText::Interpolate(sint32 first, sint32 last, uint16 age, uint16 duration)
{
    // Original interpolation divides the signed view of the low-word product
    uint32 product = (uint32)(last - first) * age;
    return (uint32)first + DivideSignedWord(product, duration);
}

float AnimatedHudText::GetHdScale(const HudTextAnimation &animation, uint16 age)
{
    return animation.duration ? animation.scale[0] + ((float)animation.scale[1] - animation.scale[0]) * age / animation.duration : animation.scale[0];
}

void AnimatedHudText::DrawAll()
{
    FUNCTION_MARKER(0x8001B8BC, "SLUS_003.73");
    AnimatedHudText *node = animated_hud_text;
    while (node)
    {
        const HudTextAnimation *animation = node->animation;
        uint8 period = animation->blink;
        if (period && !((node->age / period) & 1))
        {
            node = node->next;
            continue;
        }
        uint16 duration = animation->duration;
        hud_text_style.align = animation->align;
        hud_text_style.scale = AnimatedHudText::Interpolate(animation->scale[0], animation->scale[1], node->age, duration);
        hud_text_style.color.r = AnimatedHudText::Interpolate(animation->rgb[0][0], animation->rgb[1][0], node->age, duration);
        hud_text_style.color.g = AnimatedHudText::Interpolate(animation->rgb[0][1], animation->rgb[1][1], node->age, duration);
        hud_text_style.color.b = AnimatedHudText::Interpolate(animation->rgb[0][2], animation->rgb[1][2], node->age, duration);
        hud_text_style.color.code = animation->semi ? 0x2E : 0x2C;
        uint32 x = AnimatedHudText::Interpolate(animation->scr_pos[0][0], animation->scr_pos[1][0], node->age, duration);
        uint32 y = AnimatedHudText::Interpolate(animation->scr_pos[0][1], animation->scr_pos[1][1], node->age, duration);
        uint32 filter = AnimatedHudText::Interpolate(animation->filter[0], animation->filter[1], node->age, duration);
        float text_scale = duration ? GetHdScale(*animation, node->age) : hud_text_style.scale;
        if (node->hd_text.initialized)
            node->hd_text.style.scale = node->hd_previous_scale;
        DrawStyledHudTextHistory(node->hd_text, x, y, node->text, filter, 0, 256, text_scale);
        node = node->next;
    }
}
