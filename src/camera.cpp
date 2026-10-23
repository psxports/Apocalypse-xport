#include <string.h>
#include <new>
#include <stdlib.h>

#include "psx.h"
#include "scene_lifecycle.h"
#include "game_pad_input.h"
#include "game_tick.h"
#include "model_render_view.h"
#include "visual_object.h"
#include "player.h"
#include "game_diagnostics.h"
#include "quaternion.h"
#include "camera.h"
#include "global.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "angle_math.h"
#include "collision.h"

struct CameraTransition
{
    uint32 val;
    uint16 count;
    uint32 delta;
};

struct CameraTrackingHistory
{
    bool angles_initialized;
    bool distance_initialized;
    ShortVector3 angles;
    uint32 distance;
};

struct CameraManualSettings
{
    uint32 proj_distance;
    uint32 target_lateral;
    uint32 target_height;
    uint32 target_forward;
    uint32 orbit_distance;
    uint32 orbit_height;
    sint16 yaw;
};

struct CameraTuning
{
    Vector3 view_offset;
    uint32 pitch;
    uint32 radius;
    uint32 instance_count;
    CameraTransition orbit_distance;
    CameraTransition orbit_height;
    CameraTransition target_lateral;
    CameraTransition target_height;
    CameraTransition target_forward;
    bool restore_quat;
    bool manual_mode;
    CameraManualSettings saved;
    bool tracking_button_pressed;
    bool tracking_button_released;
    uint32 vertical_offset;
    sint32 vertical_ticks;
    CameraTrackingHistory tracking[2];
    ShortVector3 shake_frequency;
    uint8 shake_decay[3];
    ShortVector3 shake_amplitude[3];
    sint16 yaw_delta;
    uint16 yaw_count;
    uint16 yaw_target;
    Vector3 prev_pos;
    Vector3 reset_vectors[15];
};

ModelReference camera_subject;

static CameraTuning camera_tuning = {{{{0, 0, 0}}}, 300, 1200, 0, {720, 0, 0}, {(uint32)-212, 0, 0}, {0, 0, 0}, {(uint32)-32, 0, 0}, {0, 0, 0}, false, false, {256}};

void Camera::TurnTracking(sint16 delta, sint32 distance, bool fine_turn)
{
    sint32 adjust, maximum, val;
    if (distance >= 1001)
    {
        if (delta < (fine_turn ? -8 : -256))
        {
            if (delta < -256)
            {
                if (delta < -512)
                {
                    adjust = -delta - 512;
                    maximum = turn_step << 1;
                    if (maximum < adjust)
                        adjust = maximum;
                    angles.y = (uint16)angles.y - adjust;
                }
                val = (uint16)angles.y;
                adjust = turn_step;
                angles.y = val - adjust;
            }
            else
                angles.y = (uint16)angles.y - 9;
        }
        if (delta >= (fine_turn ? 9 : 257))
        {
            if (delta >= 257)
            {
                if (delta >= 513)
                {
                    adjust = delta - 512;
                    maximum = turn_step << 1;
                    if (maximum < adjust)
                        adjust = maximum;
                    angles.y = (uint16)angles.y + adjust;
                }
                val = (uint16)angles.y;
                adjust = turn_step;
                angles.y = val + adjust;
            }
            else
                angles.y = (uint16)angles.y + 9;
        }
        val = angles.y;
        if (val < 0)
        {
            do
            {
                val += 4096;
                angles.y = val;
            } while (val < 0);
        }
    }
    angles.y = (uint16)angles.y & 4095;
}

void Camera::RotateVector(const MATRIX &matrix, const Vector3 &input, Vector3 &output)
{
    uint32 packed[5];
    for (uint32 axis = 0; axis < 5; ++axis)
        packed[axis] = PackModelRotationWord(matrix, axis);
    for (uint32 axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, packed[axis]);
    VECTOR src = {input.x, input.y, input.z, 0};
    VECTOR dst;
    ApplyRotMatrixLV(&src, &dst);
    output.x = dst.vx;
    output.y = dst.vy;
    output.z = dst.vz;
}

sint16 Camera::ReadColAngle(CollisionRay &rec, sint16 strength, bool reverse)
{
    uint32 numerator, ratio, product;
    if (!rec.len)
        rec.len = 1;
    numerator = (rec.len - rec.distance) << 12;
    ratio = DivideSignedWord(numerator, rec.len);
    product = ratio * ratio;
    product = strength * (uint32)((sint32)product >> 12);
    if (reverse)
        return -((sint32)product >> 12);
    return (sint32)(product << 4) >> 16;
}

sint16 Camera::ReadOscillation(uint32 axis) const
{
    uint32 phase = (uint32)shake.frequency[axis] * game_clock.tick;
    sint32 s = rsin(phase & 4095);
    sint32 product = shake.amplitude[axis] * s;
    // Q12 scaling rounds toward zero before the original signed16 narrowing
    return product / ONE;
}

void Camera::AdjustDebugVal(const PadButton &button, uint32 &val, bool subtract)
{
    if (button.down)
    {
        uint32 step = game_pads[1].buttons[15].down ? 32 : 8;
        val = subtract ? val - step : val + step;
    }
}

void Camera::AdvanceGlobalTransition(CameraTransition &transition)
{
    if (transition.count)
    {
        --transition.count;
        transition.val += transition.delta;
    }
}

sint16 Camera::ShortestAngleDelta(sint16 src_angle, sint16 target_angle)
{
    sint32 delta;
    FUNCTION_MARKER(0x80077274, "SLUS_003.73");
    delta = (target_angle & 4095) - (src_angle & 4095);
    if (delta > 2048)
        delta -= 4096;
    else if (delta < -2048)
        delta += 4096;
    return delta;
}

Camera::Camera(VisualObject &target) : VisualObject(), proj_distance(0), proj_timer(0), proj_step(0), clamp_enabled(0), vertical_pitch(0), vertical_offset_gain(0), col_lateral_dist(0), col_rear_dist(0), col_yaw_strength(0), col_pitch_strength(0), mode(0), turn_step(0), update_ticks(0)
{
    uint32 block, idx, flags;
    ModelInstance *src;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        clamp_start.comp[axis] = 0;
        clamp_end.comp[axis] = 0;
        angles.comp[axis] = 0;
        shake.amplitude[axis] = 0;
        shake.decay[axis] = 0;
        shake.frequency[axis] = 0;
    }
    FUNCTION_MARKER(0x800772B8, "SLUS_003.73");
    follow.target.SetObject(&target.model);
    aim.target.SetObject(&target.model);
    proj_distance = 256;
    Quaternion *rotations[4] = {&rotation, &col_rotation, &blend_rotation, &saved_rotation};
    for (block = 0; block < 4; ++block)
    {
        for (idx = 0; idx < 3; ++idx)
            (*rotations[block])[idx] = 0;
        rotations[block]->w = ONE;
    }
    src = follow.target.GetObject();
    col_lateral_dist = -1;
    col_rear_dist = -1;
    col_pitch_strength = 784;
    col_yaw_strength = 256;
    turn_step = 15;
    follow.pos = src->pos;
    src = aim.target.GetObject();
    aim.pos = src->pos;
    kind = 99;
    model.flags = 1;
    PrepareView();
    AddToList(visual_lists.cameras);
    mode = 7;
    flags = lifecycle_flags;
    col_radius = 0;
    lifecycle_flags = flags & 0xFFFD;
    ++camera_tuning.instance_count;
}

uint32 Camera::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8007741C, "SLUS_003.73");
    RemoveFromList(visual_lists.cameras);
    --camera_tuning.instance_count;
    result = VisualObject::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

void Camera::SetColDistances(sint32 val)
{
    FUNCTION_MARKER(0x80077490, "SLUS_003.73");
    col_lateral_dist = val;
    col_rear_dist = val;
}

void Camera::SetLateralColDistance(sint32 val)
{
    FUNCTION_MARKER(0x8007749C, "SLUS_003.73");
    col_lateral_dist = val;
}

void Camera::SetRearColDistance(sint32 val)
{
    FUNCTION_MARKER(0x800774A4, "SLUS_003.73");
    col_rear_dist = val;
}

void Camera::SetColYawStrength(sint16 val)
{
    FUNCTION_MARKER(0x800774AC, "SLUS_003.73");
    col_yaw_strength = val;
}

void Camera::SetColPitchStrength(sint16 val)
{
    FUNCTION_MARKER(0x800774B4, "SLUS_003.73");
    col_pitch_strength = val;
}

void Camera::MarkShakeList(ModelInstance *list)
{
    FUNCTION_MARKER(0x800774BC, "SLUS_003.73");
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        object.activation_flags |= 2;
        list = list->next;
    }
}

void Camera::ApplyExplosionShake(uint32 kind)
{
    FUNCTION_MARKER(0x800774EC, "SLUS_003.73");
    if (kind <= 2)
    {
        const ShortVector3 &preset = camera_tuning.shake_amplitude[2 - kind];
        shake.amplitude = preset;
        shake.decay[0] = camera_tuning.shake_decay[0];
        shake.decay[1] = camera_tuning.shake_decay[1];
        shake.decay[2] = camera_tuning.shake_decay[2];
        shake.frequency = camera_tuning.shake_frequency;
    }
    Camera::MarkShakeList(visual_lists.actors);
    Camera::MarkShakeList(visual_lists.solids);
}

uint32 Camera::SetModePreset(uint16 mode)
{
    uint32 result = 20;
    FUNCTION_MARKER(0x80077624, "SLUS_003.73");
    if (!mode)
    {
        this->mode = 20;
        turn_step = 0;
    }
    else if (mode == 1 || mode == 2)
    {
        this->mode = mode == 1 ? 18 : 20;
        result = 16;
        turn_step = result;
    }
    return result;
}

MATRIX *Camera::PrepareView()
{
    FUNCTION_MARKER(0x80077680, "SLUS_003.73");
    ModelRenderView &view = GetModelRenderView();
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint32 coord = (uint32)model.pos.comp[axis] + camera_tuning.view_offset.comp[axis];
        view.pos.comp[axis] = coord >> 12;
    }
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint32 coord = (uint32)aim.pos.comp[axis] + camera_tuning.view_offset.comp[axis];
        view.target_pos.comp[axis] = coord >> 12;
    }
    rotation.ToMatrix(view.rotation);
    return view.SetTranslation();
}

void Camera::ResolveCollisions()
{
    FUNCTION_MARKER(0x80077748, "SLUS_003.73");
    MATRIX matrix;
    Vector3 prev, input, t, lateral, opposite, forward;
    Vector3 target, difference;
    CollisionRay col;
    Quaternion q, rotated, pitch_rotation, yaw_rotation, combined, old_q, desired_q;
    ShortVector3 follow_angles, subject_angles;
    sint32 distance;
    uint32 result, angle, idx, radius;
    ModelInstance *target_object;
    uint32 paused = scene_frame.paused;
    bool target_initialized = false;
    prev = model.pos;
    if (!paused)
    {
        radius = camera_tuning.radius;
        blend_rotation = rotation;
        input.x = 0;
        input.y = 0;
        input.z = -(radius << 12);
        col_rotation.ToMatrix(matrix);
        Camera::RotateVector(matrix, input, t);
        distance = col_rear_dist;
        // Authorized original-bug fix uses the default distance for invalid negatives
        if (distance < -1)
            distance = -1;
        if (distance == -1)
        {
            forward.x = t.x;
            forward.y = 0;
            forward.z = t.z;
        }
        else if (distance > 0)
        {
            input.x = 0;
            input.y = 0;
            input.z = -((uint32)col_rear_dist << 12);
            Camera::RotateVector(matrix, input, forward);
        }
        distance = col_lateral_dist;
        // Authorized original-bug fix uses the default distance for invalid negatives
        if (distance < -1)
            distance = -1;
        if (distance == -1 || distance > 0)
        {
            input.x = -((distance == -1 ? camera_tuning.radius : (uint32)col_lateral_dist) << 12);
            input.y = 0;
            input.z = 0;
            Camera::RotateVector(matrix, input, lateral);
            opposite.x = -(uint32)lateral.x;
            opposite.y = -(uint32)lateral.y;
            opposite.z = -(uint32)lateral.z;
        }
        target.x = (uint32)follow.pos.x + t.x;
        target.y = (uint32)follow.pos.y + t.y;
        target.z = (uint32)follow.pos.z + t.z;
        target_initialized = true;
        camera_tuning.prev_pos = target;
        difference.Subtract(target, prev);
        if (col_lateral_dist)
        {
            for (idx = 0; idx < 2; ++idx)
            {
                const Vector3 &offset = idx ? opposite : lateral;
                col.endpoints[0] = follow.pos;
                col.endpoints[1].x = (uint32)follow.pos.x + offset.x;
                col.endpoints[1].y = follow.pos.y;
                col.endpoints[1].z = (uint32)follow.pos.z + offset.z;
                col.Initialize();
                col.triggers = false;
                col_query.require_surface10 = true;
                col.Query(1);
                col_query.require_surface10 = false;
                if (col.hit.GetObject())
                {
                    angle = Camera::ReadColAngle(col, col_yaw_strength, idx);
                    q.SetYaw(angle);
                    col_rotation.Multiply(q, col_rotation);
                }
            }
        }
        if (col_rear_dist)
        {
            col.endpoints[0] = follow.pos;
            col.endpoints[1].x = (uint32)follow.pos.x + ((uint32)forward.x << 1);
            col.endpoints[1].y = follow.pos.y;
            col.endpoints[1].z = (uint32)follow.pos.z + ((uint32)forward.z << 1);
            col.Initialize();
            col.triggers = false;
            col_query.require_surface10 = true;
            col.Query(1);
            col_query.require_surface10 = false;
            if (col.hit.GetObject())
            {
                angle = Camera::ReadColAngle(col, col_pitch_strength, 0);
                q.SetPitch(angle);
                rotated.Multiply(col_rotation, q);
                col_rotation = rotated;
            }
        }
    }
    blend_rotation.Interpolate(col_rotation, 1023, rotation);
    input.x = 0;
    input.y = 0;
    input.z = -(camera_tuning.radius << 12);
    rotation.ToMatrix(matrix);
    Camera::RotateVector(matrix, input, t);
    model.pos.x = (uint32)follow.pos.x + t.x;
    model.pos.y = (uint32)follow.pos.y + t.y;
    model.pos.z = (uint32)follow.pos.z + t.z;
    difference.Subtract(model.pos, prev);
    result = scene_frame.paused;
    motion.velocity = difference;
    if (!result)
    {
        target_object = follow.target.GetObject();
        ModelInstance *aim_object = aim.target.GetObject();
        if (target_object != aim_object)
        {
            if (!target_initialized)
            {
                GameDiagnostic(("TODO Camera target is uninitialized after a live pause-state change\n"));
                abort();
            }
            follow_angles.SetDir(follow.pos, target);
            ModelInstance *subject = camera_subject.GetObject();
            if (!subject)
            {
                GameDiagnostic(("Camera collision rotation has no subject\n"));
                abort();
            }
            subject_angles.SetDir(subject->pos, target);
            angle = Camera::ShortestAngleDelta(follow_angles.comp[1], subject_angles.comp[1]);
            desired_q[0] = desired_q[1] = desired_q[2] = 0;
            desired_q[3] = ONE;
            saved_rotation = rotation;
            follow_angles.SetDir(model.pos, follow.pos);
            subject_angles.SetDir(model.pos, camera_subject.GetObject()->pos);
            yaw_rotation.SetYaw((uint32)(sint16)(4096 - angle));
            combined.Multiply(yaw_rotation, rotation);
            pitch_rotation.SetPitch((subject_angles.x - follow_angles.x) & 4095);
            rotated.Multiply(combined, pitch_rotation);
            desired_q = rotated;
            old_q = rotation;
            old_q.Interpolate(desired_q, 1023, rotation);
            return;
        }
    }
    return;
}

void Camera::SetTargetTransition(const Vector3 &pos, uint32 frames)
{
    FUNCTION_MARKER(0x80078178, "SLUS_003.73");
    if (follow.timer)
    {
        ModelInstance *object = follow.target.GetObject();
        if (object && !static_cast<VisualObject *>(object->owner)->IsFinished())
        {
            object = follow.target.GetObject();
            follow.pos = object->pos;
        }
    }
    follow.timer = frames;
    follow.step.Subtract(pos, follow.pos);
    uint32 divisor = follow.timer;
    follow.step.Divide(divisor);
}

void Camera::ClearTargetTransition()
{
    FUNCTION_MARKER(0x8007823C, "SLUS_003.73");
    follow.timer = 0;
}

void Camera::ClampTargetToSeg()
{
    FUNCTION_MARKER(0x80078310, "SLUS_003.73");
    Vector3 seg, delta;
    seg.Subtract(clamp_end, clamp_start);
    delta.Subtract(follow.pos, clamp_start);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        seg.comp[axis] >>= 6;
        delta.comp[axis] >>= 6;
    }
    uint32 dot = (uint32)seg.x * delta.x + (uint32)seg.y * delta.y + (uint32)seg.z * delta.z;
    if ((sint32)dot <= 0)
    {
        follow.pos = clamp_start;
        return;
    }
    uint32 sq = (uint32)seg.x * seg.x + (uint32)seg.y * seg.y + (uint32)seg.z * seg.z;
    if ((sint32)sq < (sint32)dot)
    {
        follow.pos = clamp_end;
        return;
    }
    uint32 denominator = (sint32)sq >> 12;
    if (denominator)
        dot = DivideSignedWord(dot, denominator);
    uint32 factor = (sint32)dot >> 6;
    for (uint32 axis = 0; axis < 3; ++axis)
        follow.pos.comp[axis] = (uint32)clamp_start.comp[axis] + (uint32)seg.comp[axis] * factor;
}

void Camera::SetYawTransition(uint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x8007851C, "SLUS_003.73");
    sint32 target_yaw = target & 4095;
    if (!frames)
    {
        angles.y = target_yaw;
        camera_tuning.yaw_count = 0;
        return;
    }

    sint32 cur_yaw = angles.y;
    camera_tuning.yaw_target = target_yaw;
    if (cur_yaw == target_yaw)
    {
        camera_tuning.yaw_delta = 0;
        camera_tuning.yaw_count = 0;
        return;
    }
    camera_tuning.yaw_count = frames;
    sint32 delta = target_yaw - cur_yaw;
    if (delta > 2048)
        delta -= 4096;
    else if (delta < -2048)
        delta += 4096;
    sint32 step = delta / frames;
    camera_tuning.yaw_delta = step;
}

void Camera::SetOrbitDistanceTransition(uint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x800785D8, "SLUS_003.73");
    if (frames)
    {
        sint32 difference = target - camera_tuning.orbit_distance.val;
        sint32 step = difference / frames;
        camera_tuning.orbit_distance.count = frames;
        camera_tuning.orbit_distance.delta = step;
    }
    else
    {
        camera_tuning.orbit_distance.val = target;
        camera_tuning.orbit_distance.count = 0;
    }
}

void Camera::SetOrbitHeightTransition(sint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x80078618, "SLUS_003.73");
    if (frames)
    {
        sint32 difference = target - camera_tuning.orbit_height.val;
        sint32 step = difference / frames;
        camera_tuning.orbit_height.count = frames;
        camera_tuning.orbit_height.delta = step;
    }
    else
    {
        camera_tuning.orbit_height.val = target;
        camera_tuning.orbit_height.count = 0;
    }
}

void Camera::SetTargetLateralOffsetTransition(sint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x8007865C, "SLUS_003.73");
    if (frames)
    {
        sint32 difference = target - camera_tuning.target_lateral.val;
        sint32 step = difference / frames;
        camera_tuning.target_lateral.count = frames;
        camera_tuning.target_lateral.delta = step;
    }
    else
    {
        camera_tuning.target_lateral.val = target;
        camera_tuning.target_lateral.count = 0;
    }
}

void Camera::SetTargetHeightOffsetTransition(sint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x800786A0, "SLUS_003.73");
    if (frames)
    {
        sint32 difference = target - camera_tuning.target_height.val;
        sint32 step = difference / frames;
        camera_tuning.target_height.count = frames;
        camera_tuning.target_height.delta = step;
    }
    else
    {
        camera_tuning.target_height.val = target;
        camera_tuning.target_height.count = 0;
    }
}

void Camera::SetTargetForwardOffsetTransition(sint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x800786E4, "SLUS_003.73");
    if (frames)
    {
        sint32 difference = target - camera_tuning.target_forward.val;
        sint32 step = difference / frames;
        camera_tuning.target_forward.count = frames;
        camera_tuning.target_forward.delta = step;
    }
    else
    {
        camera_tuning.target_forward.val = target;
        camera_tuning.target_forward.count = 0;
    }
}

void Camera::SetProjDistanceTransition(uint16 target, uint16 frames)
{
    FUNCTION_MARKER(0x80078728, "SLUS_003.73");
    proj_timer = frames;
    if (frames)
    {
        uint32 difference = target - proj_distance;
        proj_step = DivideSignedWord(difference, proj_timer);
    }
    else
        proj_distance = target;
}

uint32 Camera::GetProjDistance()
{
    FUNCTION_MARKER(0x80078760, "SLUS_003.73");
    return proj_distance;
}

uint32 Camera::Update()
{
    uint32 trig_angle;
    uint32 result, x, y, z, val, yaw, term0, term1, radius;
    ModelInstance *target;
    uint32 mag;
    Quaternion shake_rotations[5], shaken_rotation;
    bool mode_button, rec_button, toggle_button, modifier, basic_button, held;
    sint32 angle, vertical;
    FUNCTION_MARKER(0x8007876C, "SLUS_003.73");
    result = scene_frame.paused;
    if (result)
        return result;
    camera_tuning.prev_pos = model.pos;
    ++update_ticks;
    if (follow.timer)
    {
        follow.pos.Add(follow.step);
        --follow.timer;
    }
    else
    {
        target = follow.target.GetObject();
        if (target && !static_cast<VisualObject *>(target->owner)->IsFinished())
        {
            target = follow.target.GetObject();
            follow.pos = target->pos;
        }
    }
    if (aim.timer)
    {
        aim.pos.Add(aim.step);
        --aim.timer;
    }
    else
    {
        target = aim.target.GetObject();
        if (!target)
        {
            GameDiagnostic(("Camera update has no aim target\n"));
            abort();
        }
        aim.pos = target->pos;
    }
    if (proj_timer)
    {
        proj_distance += proj_step;
        --proj_timer;
    }
    if (clamp_enabled)
        ClampTargetToSeg();
    if (game_pads[0].buttons[15].down)
    {
        if (camera_tuning.tracking_button_released)
            camera_tuning.tracking_button_pressed = true;
        camera_tuning.tracking_button_released = false;
    }
    else
        camera_tuning.tracking_button_released = true;
    mode_button = game_pads[1].buttons[6].pressed;
    if (mode_button)
    {
        rec_button = game_pads[1].buttons[7].pressed;
        if (rec_button)
        {
            toggle_button = game_pads[1].buttons[6].down;
            if (toggle_button)
            {
                modifier = game_pads[1].buttons[7].down;
                basic_button = game_pads[0].buttons[0].down;
                held = game_pads[0].buttons[15].down;
                if (held && modifier && basic_button)
                {
                    bool manual_mode = !camera_tuning.manual_mode;
                    game_pads[1].buttons[6].pressed = 0;
                    game_pads[1].buttons[7].pressed = 0;
                    camera_tuning.manual_mode = manual_mode;
                    scene_frame.camera_paused = manual_mode;
                    if (manual_mode)
                    {
                        camera_tuning.saved.target_lateral = camera_tuning.target_lateral.val;
                        camera_tuning.saved.target_height = camera_tuning.target_height.val;
                        camera_tuning.saved.target_forward = camera_tuning.target_forward.val;
                        camera_tuning.saved.orbit_distance = camera_tuning.orbit_distance.val;
                        camera_tuning.saved.orbit_height = camera_tuning.orbit_height.val;
                        camera_tuning.saved.yaw = angles.y;
                        camera_tuning.saved.proj_distance = proj_distance;
                    }
                    else
                        goto AdvanceTransitions;
                }
            }
        }
    }
    if (camera_tuning.manual_mode)
    {
        if (game_pads[1].buttons[14].down)
        {
            camera_tuning.target_lateral.val = camera_tuning.saved.target_lateral;
            camera_tuning.target_height.val = camera_tuning.saved.target_height;
            camera_tuning.target_forward.val = camera_tuning.saved.target_forward;
            camera_tuning.orbit_distance.val = camera_tuning.saved.orbit_distance;
            camera_tuning.orbit_height.val = camera_tuning.saved.orbit_height;
            angles.y = camera_tuning.saved.yaw;
            proj_distance = camera_tuning.saved.proj_distance;
        }
        if (game_pads[0].buttons[1].down)
            --proj_distance;
        if (game_pads[0].buttons[2].down)
            ++proj_distance;
        if (game_pads[0].buttons[0].down)
            proj_distance = 256;
        Camera::AdjustDebugVal(game_pads[1].buttons[8], camera_tuning.target_lateral.val, 1);
        Camera::AdjustDebugVal(game_pads[1].buttons[9], camera_tuning.target_lateral.val, 0);
        Camera::AdjustDebugVal(game_pads[1].buttons[10], camera_tuning.target_forward.val, 0);
        Camera::AdjustDebugVal(game_pads[1].buttons[11], camera_tuning.target_forward.val, 1);
        Camera::AdjustDebugVal(game_pads[1].buttons[4], camera_tuning.target_height.val, 1);
        Camera::AdjustDebugVal(game_pads[1].buttons[5], camera_tuning.target_height.val, 0);
        Camera::AdjustDebugVal(game_pads[1].buttons[6], camera_tuning.orbit_height.val, 1);
        Camera::AdjustDebugVal(game_pads[1].buttons[7], camera_tuning.orbit_height.val, 0);
        Camera::AdjustDebugVal(game_pads[1].buttons[0], camera_tuning.orbit_distance.val, 1);
        Camera::AdjustDebugVal(game_pads[1].buttons[3], camera_tuning.orbit_distance.val, 0);
        if (game_pads[1].buttons[1].down)
        {
            val = game_pads[1].buttons[15].down;
            yaw = angles.y;
            angles.y = (yaw + (val ? 32 : 8)) & 4095;
        }
        if (game_pads[1].buttons[2].down)
        {
            val = game_pads[1].buttons[15].down;
            yaw = angles.y;
            angles.y = (yaw - (val ? 32 : 8)) & 4095;
        }
    }
AdvanceTransitions:
    Camera::AdvanceGlobalTransition(camera_tuning.orbit_distance);
    Camera::AdvanceGlobalTransition(camera_tuning.orbit_height);
    if (camera_tuning.yaw_count)
    {
        --camera_tuning.yaw_count;
        if (camera_tuning.yaw_count)
            angles.y = ((uint16)angles.y + camera_tuning.yaw_delta) & 4095;
        else
            angles.y = camera_tuning.yaw_target;
    }
    Camera::AdvanceGlobalTransition(camera_tuning.target_lateral);
    Camera::AdvanceGlobalTransition(camera_tuning.target_height);
    Camera::AdvanceGlobalTransition(camera_tuning.target_forward);
    val = camera_tuning.orbit_distance.val;
    term0 = val * val;
    val = camera_tuning.orbit_height.val;
    term1 = val * val;
    radius = SquareRoot0(term0 + term1);
    x = camera_tuning.orbit_height.val;
    y = camera_tuning.orbit_distance.val;
    camera_tuning.radius = radius;
    angle = ratan2(x, y);
    val = camera_tuning.target_height.val;
    camera_tuning.view_offset.y = val << 12;
    yaw = (uint16)angles.y;
    x = camera_tuning.target_lateral.val;
    trig_angle = yaw & 4095;
    term0 = x * rcos(trig_angle);
    z = camera_tuning.target_forward.val;
    term1 = z * rsin(trig_angle);
    camera_tuning.view_offset.x = -term0 - term1;
    yaw = (uint16)angles.y;
    trig_angle = yaw & 4095;
    term0 = x * rsin(trig_angle);
    term1 = z * rcos(trig_angle);
    camera_tuning.pitch = -(uint32)angle;
    camera_tuning.view_offset.z = term0 - term1;
    model.angles.SetDir(model.pos, aim.pos);
    target = follow.target.GetObject();
    // Only the player has the airborne state used for jump compensation
    if (target && target == visual_lists.players && static_cast<Player *>(target->owner)->airborne_ticks && vertical_offset_gain && !follow.timer)
    {
        vertical = static_cast<VisualObject *>(target->owner)->motion.velocity.y;
        val = camera_tuning.vertical_ticks;
        camera_tuning.vertical_ticks = vertical < 0 ? val + 1 : val - 1;
        target = follow.target.GetObject();
        val = (uint32) static_cast<VisualObject *>(target->owner)->motion.velocity.y;
        mag = vertical_offset_gain;
        vertical = -val;
        term0 = (vertical >> 12) * mag;
        val = camera_tuning.vertical_offset;
        val += term0;
        camera_tuning.vertical_offset = val;
        if (camera_tuning.vertical_ticks > 0)
            follow.pos.y = (uint32)follow.pos.y + val;
    }
    else
    {
        camera_tuning.vertical_offset = 0;
        camera_tuning.vertical_ticks = 0;
    }
    mag = vertical_pitch;
    if (mag)
    {
        target = follow.target.GetObject();
        if (!target)
        {
            GameDiagnostic(("Camera vertical pitch has no follow target\n"));
            abort();
        }
        sint32 velocity = static_cast<VisualObject *>(target->owner)->motion.velocity.y;
        if (velocity < 0)
            camera_tuning.pitch = camera_tuning.pitch - mag;
        if (velocity > 0)
        {
            mag = vertical_pitch >> 1;
            val = camera_tuning.pitch;
            camera_tuning.pitch = val - mag;
        }
        camera_tuning.pitch = camera_tuning.pitch & 4095;
    }
    switch (mode)
    {
        case 2:
        case 3:
        case 7:
        case 15:
            camera_tuning.restore_quat = false;
            BuildView();
            break;
        case 18:
        case 19:
            camera_tuning.restore_quat = true;
            UpdatePlayerRelativeTracking();
            break;
        case 20:
            camera_tuning.restore_quat = true;
            UpdateFixedOriginTracking();
            break;
    }
    ResolveCollisions();
    model.angles.SetDir(model.pos, aim.pos);
    if (shake.amplitude.x || shake.amplitude.y || shake.amplitude.z)
    {
        shake_rotations[0].SetYaw(ReadOscillation(1));
        shake_rotations[1].Multiply(shake_rotations[0], rotation);
        shake_rotations[2].SetPitch(ReadOscillation(0));
        shake_rotations[3].Multiply(shake_rotations[1], shake_rotations[2]);
        shake_rotations[4].SetRoll(ReadOscillation(2));
        shaken_rotation.Multiply(shake_rotations[3], shake_rotations[4]);
        rotation = shaken_rotation;
        ShortVector3 prev_amplitude = shake.amplitude;
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            sint32 amplitude = shake.amplitude[axis];
            shake.amplitude[axis] = amplitude < 0 ? amplitude + shake.decay[axis] : amplitude - shake.decay[axis];
        }
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            if ((prev_amplitude[axis] < 0) != (shake.amplitude[axis] < 0))
                shake.amplitude[axis] = 0;
        }
    }
    PrepareView();
    result = camera_tuning.restore_quat;
    if (result)
        rotation = saved_rotation;
    return result;
}

uint32 Camera::BuildView()
{
    uint32 trig_angle;
    FUNCTION_MARKER(0x800793C8, "SLUS_003.73");
    Vector3 offset;
    offset.x = 0;
    offset.y = 0;
    offset.z = camera_tuning.radius << 12;
    Vector3 forward, n_up;
    ShortVector3 view_angles = angles;
    uint32 steps[4], pass, step, i, x, y, z, s, c, sum;
    sint32 divisor;
    MATRIX matrix;
    steps[0] = camera_tuning.pitch & 0xFFF;
    steps[1] = (uint16)view_angles.y & 4095;
    steps[2] = (uint16)view_angles.x & 4095;
    steps[3] = (uint16)view_angles.z & 4095;
    for (pass = 0; pass < 2; ++pass)
    {
        if (pass)
        {
            trig_angle = camera_tuning.pitch & 4095;
            s = rsin(trig_angle);
            c = rcos(trig_angle);
            view_up.x = 0;
            view_up.y = 500 * c;
            view_up.z = 500 * s;
        }
        Vector3 &vector = pass ? view_up : offset;
        for (step = pass ? 1 : 0; step < 4; ++step)
        {
            trig_angle = steps[step];
            s = rsin(trig_angle);
            c = rcos(trig_angle);
            x = vector.x >> 12;
            y = vector.y >> 12;
            z = vector.z >> 12;
            if (step == 1)
            {
                vector.x = x * c + z * s;
                vector.z = z * c - x * s;
            }
            else if (step == 3)
            {
                vector.x = x * c - y * s;
                vector.y = y * c + x * s;
            }
            else
            {
                vector.y = y * c - z * s;
                vector.z = z * c + y * s;
            }
        }
    }
    view_from.Add(offset, follow.pos);
    view_to = follow.pos;
    sum = 0;
    for (i = 0; i < 3; ++i)
    {
        forward.comp[i] = (view_to.comp[i] >> 12) - (view_from.comp[i] >> 12);
        sum += (uint32)forward.comp[i] * forward.comp[i];
    }
    divisor = SquareRoot0(sum);
    if (!divisor)
        divisor = 1;
    for (i = 0; i < 3; ++i)
    {
        uint32 val = (uint32)forward.comp[i] << 12;
        forward.comp[i] = DivideSignedWord(val, divisor);
    }
    sum = 0;
    for (i = 0; i < 3; ++i)
    {
        n_up.comp[i] = view_up.comp[i] >> 12;
        sum += (uint32)n_up.comp[i] * n_up.comp[i];
    }
    divisor = SquareRoot0(sum);
    if (!divisor)
        divisor = 1;
    for (i = 0; i < 3; ++i)
    {
        uint32 val = (uint32)n_up.comp[i] << 12;
        n_up.comp[i] = DivideSignedWord(val, divisor);
    }
    xport_gte_write_control(0, (uint32)n_up.x);
    xport_gte_write_control(2, (uint32)n_up.y);
    xport_gte_write_control(4, (uint32)n_up.z);
    xport_gte_write_data(11, (uint32)forward.z);
    xport_gte_write_data(9, (uint32)forward.x);
    xport_gte_write_data(10, (uint32)forward.y);
    xport_gte_execute(0x170000C);
    sint32 mac_x = xport_gte_read_data(25);
    sint32 mac_y = xport_gte_read_data(26);
    sint32 mac_z = xport_gte_read_data(27);
    matrix.m[0][0] = mac_x >> 12;
    matrix.m[1][0] = mac_y >> 12;
    matrix.m[2][0] = mac_z >> 12;
    matrix.m[0][1] = (uint32)n_up.x;
    matrix.m[1][1] = (uint32)n_up.y;
    matrix.m[2][1] = (uint32)n_up.z;
    matrix.m[0][2] = (uint32)forward.x;
    matrix.m[1][2] = (uint32)forward.y;
    matrix.m[2][2] = (uint32)forward.z;
    return Quaternion::FromMatrix(matrix, col_rotation);
}

uint32 Camera::UpdatePlayerRelativeTracking()
{
    FUNCTION_MARKER(0x80079DC8, "SLUS_003.73");
    ModelInstance *target, *original_target;
    sint32 distance = 0;
    uint32 target_distance;
    uint32 angle, table_idx, product, mag, scale, prev, val;
    const Vector3 *origin;
    bool distance_known = false;
    sint32 delta;
    ShortVector3 dir, target_angles, turn, pos_angles, final_target_angles;
    original_target = camera_subject.GetObject();
    if (!original_target)
    {
        aim.target.SetObject(follow.target.GetObject());
        return BuildView();
    }
    target = camera_subject.GetObject();
    aim.target.SetObject(original_target);
    camera_tuning.view_offset.x = 0;
    camera_tuning.view_offset.y = -368640;
    camera_tuning.view_offset.z = 0;
    if (target)
    {
        distance = follow.pos.Distance(target->pos);
        distance_known = true;
    }
    origin = &model.pos;
    dir.SetDir(follow.pos, *origin);
    target_angles.SetDir(camera_subject.GetObject()->pos, *origin);
    CameraTrackingHistory &tracking = camera_tuning.tracking[0];
    if (!tracking.angles_initialized)
    {
        tracking.angles_initialized = true;
        tracking.angles = target_angles;
    }
    delta = Camera::ShortestAngleDelta((uint16)tracking.angles.y, target_angles.comp[1]);
    target = camera_subject.GetObject();
    angle = (uint16)tracking.angles.y;
    target_angles.y = (angle + (delta >> 1) + 4096) & 4095;
    target_distance = target->pos.Distance(*origin);
    target = camera_subject.GetObject();
    if (target)
    {
        turn.SetDir(follow.pos, target->pos);
        delta = Camera::ShortestAngleDelta((uint16)angles.y, turn.comp[1]);
        if (!distance_known)
        {
            // TODO The original reads an uninitialized distance if its live target changes here
            GameDiagnostic(("Uninitialized tracking-camera distance at %08X\n", 0x80079DC8));
            abort();
        }
        TurnTracking(delta, distance, true);
    }
    pos_angles.SetDir(follow.pos, *origin);
    final_target_angles.SetDir(camera_subject.GetObject()->pos, *origin);
    table_idx = (pos_angles.y - final_target_angles.y) & 4095;
    angle = (uint16)angles.y;
    table_idx += (target_angles.y - angle) & 4095;
    table_idx &= 4095;
    val = rsin(table_idx);
    product = val * target_distance;
    mag = (sint32)product >> 12;
    val = (sint32)product >> 31;
    mag = (mag ^ val) - val;
    scale = (sint32)(mag * 6553) >> 12;
    if (!tracking.distance_initialized)
    {
        tracking.distance_initialized = true;
        tracking.distance = scale;
    }
    prev = tracking.distance;
    val = (sint32)(scale + prev) >> 1;
    camera_tuning.radius = val;
    if ((sint32)val < 1700)
        camera_tuning.radius = 1700;
    val = camera_tuning.radius;
    if ((sint32)val >= 3001)
        camera_tuning.radius = 3000;
    camera_tuning.pitch = 130;
    tracking.distance = scale;
    tracking.angles = target_angles;
    return BuildView();
}

uint32 Camera::UpdateFixedOriginTracking()
{
    FUNCTION_MARKER(0x8007A1A8, "SLUS_003.73");
    ModelInstance *target, *original_target;
    sint32 distance = 0;
    uint32 angle, prev, val;
    const Vector3 *origin;
    bool distance_known = false;
    sint32 delta;
    ShortVector3 dir, target_angles, turn, pos_angles, final_target_angles;
    original_target = camera_subject.GetObject();
    if (!original_target)
    {
        aim.target.SetObject(follow.target.GetObject());
        return BuildView();
    }
    target = camera_subject.GetObject();
    aim.target.SetObject(original_target);
    camera_tuning.view_offset.x = 0;
    camera_tuning.view_offset.y = -368640;
    camera_tuning.view_offset.z = 0;
    if (target)
    {
        distance = follow.pos.Distance(target->pos);
        distance_known = true;
    }
    origin = &camera_tuning.prev_pos;
    dir.SetDir(follow.pos, *origin);
    target_angles.SetDir(camera_subject.GetObject()->pos, *origin);
    CameraTrackingHistory &tracking = camera_tuning.tracking[1];
    if (!tracking.angles_initialized)
    {
        tracking.angles_initialized = true;
        tracking.angles = target_angles;
    }
    delta = Camera::ShortestAngleDelta((uint16)tracking.angles.y, target_angles.comp[1]);
    target = camera_subject.GetObject();
    angle = (uint16)tracking.angles.y;
    target_angles.y = (angle + (delta >> 1) + 4096) & 4095;
    target->pos.Distance(*origin);
    target = camera_subject.GetObject();
    if (target)
    {
        turn.SetDir(follow.pos, target->pos);
        delta = Camera::ShortestAngleDelta((uint16)angles.y, turn.comp[1]);
        if (!distance_known)
        {
            // TODO The original reads an uninitialized distance if its live target changes here
            GameDiagnostic(("Uninitialized tracking-camera distance at %08X\n", 0x8007A1A8));
            abort();
        }
        TurnTracking(delta, distance, false);
    }
    pos_angles.SetDir(follow.pos, *origin);
    final_target_angles.SetDir(camera_subject.GetObject()->pos, *origin);
    if (!tracking.distance_initialized)
    {
        prev = camera_tuning.radius;
        tracking.distance_initialized = true;
        tracking.distance = prev;
    }
    prev = tracking.distance;
    camera_tuning.pitch = 100;
    tracking.angles = target_angles;
    val = (sint32)(prev + 1700) >> 1;
    camera_tuning.radius = val;
    tracking.distance = val;
    return BuildView();
}

void Camera::ResetDefaults()
{
    FUNCTION_MARKER(0x8007A5FC, "SLUS_003.73");
    camera_tuning.reset_vectors[14].x = 0;
    camera_tuning.reset_vectors[14].y = -1638400;
    camera_tuning.reset_vectors[14].z = 0;
    camera_tuning.reset_vectors[13].x = 0;
    camera_tuning.reset_vectors[13].y = -409600;
    camera_tuning.reset_vectors[13].z = 0;
    camera_tuning.reset_vectors[12].x = 0;
    camera_tuning.reset_vectors[12].y = -655360;
    camera_tuning.reset_vectors[12].z = -3891200;
    camera_tuning.reset_vectors[11].x = 0;
    camera_tuning.reset_vectors[11].y = -409600;
    camera_tuning.reset_vectors[11].z = 0;
    camera_tuning.reset_vectors[10].x = 0;
    camera_tuning.reset_vectors[10].y = -614400;
    camera_tuning.reset_vectors[10].z = 2662400;
    camera_tuning.reset_vectors[9].x = 0;
    camera_tuning.reset_vectors[9].y = -409600;
    camera_tuning.reset_vectors[9].z = 0;
    camera_tuning.reset_vectors[8].x = 0;
    camera_tuning.reset_vectors[8].y = -4505600;
    camera_tuning.reset_vectors[8].z = 5324800;
    camera_tuning.reset_vectors[7].x = 0;
    camera_tuning.reset_vectors[7].y = -409600;
    camera_tuning.reset_vectors[7].z = -3276800;
    camera_tuning.reset_vectors[6].x = 0;
    camera_tuning.reset_vectors[6].y = -9216000;
    camera_tuning.reset_vectors[6].z = 1638400;
    camera_tuning.reset_vectors[5].x = 0;
    camera_tuning.reset_vectors[5].y = -409600;
    camera_tuning.reset_vectors[5].z = -1228800;
    camera_tuning.reset_vectors[4].x = 0;
    camera_tuning.reset_vectors[4].y = -921600;
    camera_tuning.reset_vectors[4].z = 3276800;
    camera_tuning.reset_vectors[3].x = 0;
    camera_tuning.reset_vectors[3].y = -1597440;
    camera_tuning.reset_vectors[3].z = 3276800;
    camera_tuning.reset_vectors[2].x = 0;
    camera_tuning.reset_vectors[2].y = 409600;
    camera_tuning.reset_vectors[2].z = -114688000;
    camera_tuning.reset_vectors[1].x = 0;
    camera_tuning.reset_vectors[1].y = -409600;
    camera_tuning.reset_vectors[1].z = 0;
    camera_tuning.shake_amplitude[2].z = 50;
    camera_tuning.shake_amplitude[1].z = 30;
    camera_tuning.shake_decay[0] = 3;
    camera_tuning.shake_decay[2] = 3;
    camera_tuning.shake_frequency.x = 600;
    camera_tuning.shake_frequency.z = 600;
    camera_tuning.reset_vectors[0].x = 0;
    camera_tuning.shake_amplitude[2].x = 25;
    camera_tuning.shake_amplitude[2].y = 0;
    camera_tuning.shake_amplitude[1].x = 0;
    camera_tuning.shake_amplitude[1].y = 0;
    camera_tuning.shake_amplitude[0].x = 0;
    camera_tuning.shake_amplitude[0].y = 0;
    camera_tuning.shake_amplitude[0].z = 25;
    camera_tuning.shake_decay[1] = 0;
    camera_tuning.shake_frequency.y = 0;
    camera_tuning.reset_vectors[0].y = -450560;
    camera_tuning.reset_vectors[0].z = 0;
    camera_tuning.view_offset.x = 0;
    camera_tuning.view_offset.y = -368640;
    camera_tuning.view_offset.z = 0;
}
