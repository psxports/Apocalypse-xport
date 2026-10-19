#include "object_force.h"
#include "visual_object.h"
#include "player.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "weapon_nodes.h"
#include "game_diagnostics.h"

static ObjectForce *force_head;

ObjectForce::ObjectForce(uint8 mode, VisualObject *owner, ModelInstance **list0, ModelInstance **list1) : Force(), next(force_head), mode(mode), range_shift(0), strength(0), radius_sq(0), pos_owner(), owner(owner)
{
    FUNCTION_MARKER(0x8003A154, "SLUS_003.73");
    dir.x = dir.y = dir.z = 0;
    lists[0] = list0;
    lists[1] = list1;
    force_head = this;
}

void ObjectForce::Unlink()
{
    ObjectForce **link = &force_head;
    while (*link && *link != this)
        link = &(*link)->next;
    if (*link)
        *link = next;
    // Retain next until borrowed traversal has ended
}

ObjectForce::~ObjectForce()
{
    Unlink();
}

uint32 ObjectForce::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003A188, "SLUS_003.73");
    Unlink();
    MarkDestroyed();
    return reason & 1 ? ReleaseWeaponStorage(this) : 0;
}

void ObjectForce::SetPosition(VisualObject &src)
{
    FUNCTION_MARKER(0x8003A214, "SLUS_003.73");
    // The recovered producer borrows its model position rather than copying it
    pos_owner = &src;
}

void ObjectForce::SetStrength(sint16 strength, uint8 range_shift)
{
    FUNCTION_MARKER(0x8003A21C, "SLUS_003.73");
    this->strength = strength;
    this->range_shift = range_shift;
}

void ObjectForce::SetDirection(const uint16 *angles)
{
    FUNCTION_MARKER(0x8003A228, "SLUS_003.73");
    Vector3 vector;
    BuildDirectionVector(vector.words, 1, angles);
    for (uint32 axis = 0; axis < 3; ++axis)
        dir[axis] = vector.words[axis];
}

void ObjectForce::SetRadius(uint16 radius)
{
    FUNCTION_MARKER(0x8003A274, "SLUS_003.73");
    radius_sq = (uint32)radius * radius;
}

void ObjectForce::ApplyDirectional(ModelInstance *list)
{
    FUNCTION_MARKER(0x8003A288, "SLUS_003.73");
    ModelBorrow borrow;
    if (!strength || !pos_owner)
        return;
    while (list)
    {
        VisualObject *object = static_cast<VisualObject *>(list->owner);
        if (object != owner.Get())
        {
            Vector3 delta, shifted;
            uint32 shift = 12;
            SubtractVectors(delta.words, list->pos.words, pos_owner->model.pos.words);
            ShiftVectorRight(shifted.words, delta.words, shift);
            uint32 dot = shifted.words[0] * (uint32)dir.x;
            dot += shifted.words[1] * (uint32)dir.y;
            dot += shifted.words[2] * (uint32)dir.z;
            uint32 proj = (sint32)dot >> 12;
            for (uint32 axis = 0; axis < 3; ++axis)
                delta.words[axis] = (shifted.words[axis] << 12) - (uint32)dir[axis] * proj;
            if ((sint32)proj > 0 && MeasureSquaredVectorLength(delta.words) < radius_sq)
            {
                uint32 range = (uint32)1 << (range_shift & 31);
                if ((sint32)proj < (sint32)range)
                {
                    if (list == visual_lists.players)
                        static_cast<Player *>(object)->active_force = this;
                    uint32 amount = (range - proj) * (uint32)strength;
                    bool large_product = (sint32)amount > 262143;
                    if (mode != 3 && large_product)
                        amount = (sint32)amount >> 8;
                    for (uint32 axis = 0; axis < 3; ++axis)
                    {
                        uint32 product;
                        if (mode == 3)
                            product = (uint32)dir[axis] * (uint32)strength;
                        else
                        {
                            product = (uint32)dir[axis] * amount;
                            // The large-product branch removes eight bits before multiplication
                            if (large_product)
                            {
                                if (range_shift < 8)
                                    product <<= (8 - range_shift) & 31;
                                else
                                    product = (sint32)product >> ((range_shift - 8) & 31);
                            }
                            else
                                product = (sint32)product >> (range_shift & 31);
                        }
                        object->motion.velocity.words[axis] += product;
                    }
                }
            }
        }
        list = list->next;
    }
}

void ObjectForce::ApplyRadial(ModelInstance *list, bool directional)
{
    ModelBorrow borrow;
    if (!strength || !pos_owner)
        return;
    while (list)
    {
        VisualObject *object = static_cast<VisualObject *>(list->owner);
        if (object != owner.Get())
        {
            Vector3 delta, n;
            uint32 shift = 12;
            SubtractVectors(delta.words, list->pos.words, pos_owner->model.pos.words);
            uint32 mag = MeasureVectorLength(delta.words);
            if (mag)
                DivideVector(n.words, delta.words, mag);
            uint32 dot = 1;
            if (directional)
            {
                ShiftVectorRight(delta.words, shift);
                dot = delta.words[0] * (uint32)dir.x;
                dot += delta.words[1] * (uint32)dir.y;
                dot += delta.words[2] * (uint32)dir.z;
            }
            if ((sint32)dot > 0)
            {
                uint32 range = (uint32)1 << (range_shift & 31);
                if ((sint32)mag < (sint32)range)
                {
                    if (!mag)
                    {
                        // Human-approved zero impulse at the force center
                        n.words[0] = n.words[1] = n.words[2] = 0;
                    }
                    uint32 amount = (range - mag) * (uint32)strength;
                    ModelInstance *player = 0;
                    for (uint32 axis = 0; axis < 3; ++axis)
                    {
                        uint32 product = n.words[axis] * amount;
                        uint32 velocity = object->motion.velocity.words[axis];
                        if (axis == 2)
                            player = visual_lists.players;
                        object->motion.velocity.words[axis] = velocity + (uint32)((sint32)product >> (range_shift & 31));
                    }
                    if (list == player)
                        static_cast<Player *>(object)->active_force = this;
                }
            }
        }
        list = list->next;
    }
}

void ObjectForce::ApplyDirectionalRadial(ModelInstance *list)
{
    FUNCTION_MARKER(0x8003A5CC, "SLUS_003.73");
    ApplyRadial(list, true);
}

void ObjectForce::ApplyOmnidirectional(ModelInstance *list)
{
    FUNCTION_MARKER(0x8003A76C, "SLUS_003.73");
    ApplyRadial(list, false);
}

void ObjectForce::Update()
{
    FUNCTION_MARKER(0x8003A8B0, "SLUS_003.73");
    if (mode > 3)
        return;
    for (uint32 i = 0; i < 2; ++i)
    {
        if (lists[i])
        {
            if (mode == 1)
                ApplyDirectionalRadial(*lists[i]);
            else if (mode == 2)
                ApplyOmnidirectional(*lists[i]);
            else
                ApplyDirectional(*lists[i]);
        }
    }
}

void UpdateObjectForces()
{
    FUNCTION_MARKER(0x8003A9AC, "SLUS_003.73");
    ObjectBorrow borrow;
    ObjectForce *force = force_head;
    while (force)
    {
        if (force->IsAlive())
            force->Update();
        force = force->next;
    }
}

uint32 Force::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003A9EC, "SLUS_003.73");
    MarkDestroyed();
    return reason & 1 ? ReleaseWeaponStorage(this) : 0;
}
