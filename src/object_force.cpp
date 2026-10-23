#include "object_force.h"
#include "visual_object.h"
#include "player.h"

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
    return reason & 1 ? ReleaseStorage() : 0;
}

void ObjectForce::SetPos(VisualObject &src)
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

void ObjectForce::SetDir(const ShortVector3 &angles)
{
    FUNCTION_MARKER(0x8003A228, "SLUS_003.73");
    Vector3 vector;
    vector.SetDir(1, angles);
    for (uint32 axis = 0; axis < 3; ++axis)
        dir[axis] = vector.comp[axis];
}

void ObjectForce::SetRadius(uint16 radius)
{
    FUNCTION_MARKER(0x8003A274, "SLUS_003.73");
    radius_sq = (uint32)radius * radius;
}

void ObjectForce::ApplyDirectional(ModelInstance *const *head)
{
    FUNCTION_MARKER(0x8003A288, "SLUS_003.73");
    ModelBorrow borrow;
    if (!strength || !pos_owner)
        return;
    ModelInstance *list = *head;
    while (list)
    {
        VisualObject *object = static_cast<VisualObject *>(list->owner);
        if (object != owner.Get())
        {
            Vector3 delta, shifted;
            uint32 shift = 12;
            delta.Subtract(list->pos, pos_owner->model.pos);
            shifted.ShiftRight(delta, shift);
            sint32 dot = (uint32)shifted.x * dir.x;
            dot += (uint32)shifted.y * dir.y;
            dot += (uint32)shifted.z * dir.z;
            uint32 proj = dot >> 12;
            for (uint32 axis = 0; axis < 3; ++axis)
                delta.comp[axis] = ((uint32)shifted.comp[axis] << 12) - dir[axis] * proj;
            if ((sint32)proj > 0 && delta.LenSq() < radius_sq)
            {
                uint32 range = (uint32)1 << (range_shift & 31);
                if ((sint32)proj < (sint32)range)
                {
                    if (list == visual_lists.players)
                        static_cast<Player *>(object)->active_force = this;
                    uint32 amount = (range - proj) * strength;
                    bool large_product = (sint32)amount > 262143;
                    if (mode != 3 && large_product)
                        amount = (sint32)amount >> 8;
                    for (uint32 axis = 0; axis < 3; ++axis)
                    {
                        sint32 product;
                        if (mode == 3)
                            product = dir[axis] * strength;
                        else
                        {
                            product = dir[axis] * amount;
                            // The large-product branch removes eight bits before multiplication
                            if (large_product)
                            {
                                if (range_shift < 8)
                                    product = (uint32)product << ((8 - range_shift) & 31);
                                else
                                    product >>= (range_shift - 8) & 31;
                            }
                            else
                                product >>= range_shift & 31;
                        }
                        object->motion.velocity.comp[axis] = (uint32)object->motion.velocity.comp[axis] + product;
                    }
                }
            }
        }
        list = list->next;
    }
}

void ObjectForce::ApplyRadial(ModelInstance *const *head, bool directional)
{
    ModelBorrow borrow;
    if (!strength || !pos_owner)
        return;
    ModelInstance *list = *head;
    while (list)
    {
        VisualObject *object = static_cast<VisualObject *>(list->owner);
        if (object != owner.Get())
        {
            Vector3 delta, n;
            uint32 shift = 12;
            delta.Subtract(list->pos, pos_owner->model.pos);
            uint32 mag = delta.Len();
            if (mag)
                n.Divide(delta, mag);
            sint32 dot = 1;
            if (directional)
            {
                delta.ShiftRight(shift);
                dot = (uint32)delta.x * dir.x;
                dot += (uint32)delta.y * dir.y;
                dot += (uint32)delta.z * dir.z;
            }
            if (dot > 0)
            {
                uint32 range = (uint32)1 << (range_shift & 31);
                if ((sint32)mag < (sint32)range)
                {
                    if (!mag)
                    {
                        // Human-approved zero impulse at the force center
                        n.x = n.y = n.z = 0;
                    }
                    uint32 amount = (range - mag) * strength;
                    ModelInstance *player = 0;
                    for (uint32 axis = 0; axis < 3; ++axis)
                    {
                        sint32 product = (uint32)n.comp[axis] * amount;
                        uint32 velocity = object->motion.velocity.comp[axis];
                        if (axis == 2)
                            player = visual_lists.players;
                        object->motion.velocity.comp[axis] = velocity + (product >> (range_shift & 31));
                    }
                    if (list == player)
                        static_cast<Player *>(object)->active_force = this;
                }
            }
        }
        list = list->next;
    }
}

void ObjectForce::ApplyDirectionalRadial(ModelInstance *const *head)
{
    FUNCTION_MARKER(0x8003A5CC, "SLUS_003.73");
    ApplyRadial(head, true);
}

void ObjectForce::ApplyOmnidirectional(ModelInstance *const *head)
{
    FUNCTION_MARKER(0x8003A76C, "SLUS_003.73");
    ApplyRadial(head, false);
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
                ApplyDirectionalRadial(lists[i]);
            else if (mode == 2)
                ApplyOmnidirectional(lists[i]);
            else
                ApplyDirectional(lists[i]);
        }
    }
}

void ObjectForce::UpdateAll()
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
