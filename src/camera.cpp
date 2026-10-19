#include <string.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "geometry_commands.h"
#include "scene_lifecycle.h"
#include "game_pad_poll.h"

#include "game_tick.h"

#include "model_render_view.h"
#include "visual_object.h"
#include "player.h"
#include "game_diagnostics.h"
#include "quaternion_math.h"

#include "camera.h"
#include "vector_distance.h"
#include "game_heap.h"
#include "game_object_allocation.h"

#include "angle_math.h"
#include "vector_arithmetic.h"
#include "collision.h"

struct CameraTransition
{
    uint32 val;
    uint32 count;
    uint32 delta;
};

struct CameraTrackingHistory
{
    uint32 angles_initialized;
    uint32 distance_initialized;
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
    CameraTransition orbit_distance;
    CameraTransition orbit_height;
    CameraTransition target_lateral;
    CameraTransition target_height;
    CameraTransition target_forward;
    uint32 restore_quat;
    uint32 manual_mode;
    CameraManualSettings saved;
    uint32 tracking_button_pressed;
    uint32 tracking_button_released;
    uint32 vertical_offset;
    uint32 vertical_ticks;
    CameraTrackingHistory tracking[2];
    uint16 shake_frequency[3];
    uint8 shake_decay[3];
    uint16 shake_amplitude[3][3];
    uint32 yaw_delta;
    uint32 yaw_count;
    uint32 yaw_target;
    Vector3 prev_pos;
};

ModelReference camera_subject;

static CameraTuning camera_tuning = {{{{0, 0, 0}}}, 300, 1200, {720, 0, 0}, {(uint32)-212, 0, 0}, {0, 0, 0}, {(uint32)-32, 0, 0}, {0, 0, 0}, 0, 0, {256}};

static uint32 DivideCameraWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void TurnTrackingCamera(Camera &camera, sint32 delta, sint32 distance, sint32 fine_turn)
{
    uint32 adjustment, maximum, value;
    if (distance >= 1001)
    {
        if (delta < (fine_turn ? -8 : -256))
        {
            if (delta < -256)
            {
                if (delta < -512)
                {
                    adjustment = (0 - (uint32)delta) - 512;
                    maximum = camera.turn_step << 1;
                    if ((sint32)maximum < (sint32)adjustment)
                        adjustment = maximum;
                    camera.angles.words[1] = camera.angles.words[1] - adjustment;
                }
                value = camera.angles.words[1];
                adjustment = (uint16)camera.turn_step;
                camera.angles.words[1] = value - adjustment;
            }
            else
                camera.angles.words[1] = camera.angles.words[1] - 9;
        }
        if (delta >= (fine_turn ? 9 : 257))
        {
            if (delta >= 257)
            {
                if (delta >= 513)
                {
                    adjustment = (uint32)delta - 512;
                    maximum = camera.turn_step << 1;
                    if ((sint32)maximum < (sint32)adjustment)
                        adjustment = maximum;
                    camera.angles.words[1] = camera.angles.words[1] + adjustment;
                }
                value = camera.angles.words[1];
                adjustment = (uint16)camera.turn_step;
                camera.angles.words[1] = value + adjustment;
            }
            else
                camera.angles.words[1] = camera.angles.words[1] + 9;
        }
        value = (uint32)(sint32)(sint16)camera.angles.words[1];
        if ((sint32)value < 0)
        {
            do
            {
                value += 4096;
                camera.angles.words[1] = value;
            } while ((sint16)value < 0);
        }
    }
    camera.angles.words[1] = camera.angles.words[1] & 4095;
}

static void BuildCameraQuaternionMatrix(const Quaternion &quaternion, MATRIX &matrix)
{
    uint32 w = quaternion[3], x = quaternion[0];
    uint32 wx = w * x, y = quaternion[1];
    uint32 wy = w * y, z = quaternion[2];
    uint32 wz = w * z, xx = x * x, xy = x * y, yy = y * y, zz = z * z;
    uint32 xz = x * z, yz;
    uint32 xx_shift, xy_shift, yy_shift, zz_shift, wx_shift, wy_shift, wz_shift, xz_shift, yz_shift;
    matrix.t[0] = 0;
    matrix.t[1] = 0;
    matrix.t[2] = 0;
    wz_shift = (uint32)((sint32)wz >> 11);
    yy_shift = (uint32)((sint32)yy >> 11);
    zz_shift = (uint32)((sint32)zz >> 11);
    xy_shift = (uint32)((sint32)xy >> 11);
    matrix.m[0][0] = (sint16)(4096 - (yy_shift + zz_shift));
    matrix.m[0][1] = (sint16)(xy_shift + wz_shift);
    matrix.m[1][0] = (sint16)(xy_shift - wz_shift);
    wy_shift = (uint32)((sint32)wy >> 11);
    xz_shift = (uint32)((sint32)xz >> 11);
    xx_shift = (uint32)((sint32)xx >> 11);
    matrix.m[1][1] = (sint16)(4096 - (xx_shift + zz_shift));
    wx_shift = (uint32)((sint32)wx >> 11);
    matrix.m[0][2] = (sint16)(xz_shift - wy_shift);
    matrix.m[2][0] = (sint16)(xz_shift + wy_shift);
    matrix.m[2][2] = (sint16)(4096 - (xx_shift + yy_shift));
    yz = y * z;
    yz_shift = (uint32)((sint32)yz >> 11);
    matrix.m[1][2] = (sint16)(yz_shift + wx_shift);
    matrix.m[2][1] = (sint16)(yz_shift - wx_shift);
}

static void RotateCameraVector(const MATRIX &matrix, const Vector3 &input, Vector3 &output)
{
    Vector3 high, low, high_mac, low_mac;
    uint32 packed[5];
    uint32 axis, value, magnitude;
    for (axis = 0; axis < 4; ++axis)
    {
        uint32 first = axis * 2, second = first + 1;
        packed[axis] = (uint16)matrix.m[first / 3][first % 3] | ((uint32)(uint16)matrix.m[second / 3][second % 3] << 16);
    }
    packed[4] = (uint16)matrix.m[2][2];
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, packed[axis]);
    for (axis = 0; axis < 3; ++axis)
    {
        value = input.words[axis];
        if ((sint32)value < 0)
        {
            magnitude = 0 - value;
            high.words[axis] = 0 - (uint32)((sint32)magnitude >> 15);
            low.words[axis] = 0 - (magnitude & 0x7FFF);
        }
        else
        {
            high.words[axis] = (uint32)((sint32)value >> 15);
            low.words[axis] = value & 0x7FFF;
        }
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, high.words[axis]);
    ExecuteGeometryCommand(0x41E012);
    for (axis = 0; axis < 3; ++axis)
        high_mac.words[axis] = xport_gte_read_data(25 + axis);
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, low.words[axis]);
    ExecuteGeometryCommand(0x49E012);
    for (axis = 0; axis < 3; ++axis)
        low_mac.words[axis] = xport_gte_read_data(25 + axis);
    for (axis = 0; axis < 3; ++axis)
        output.words[axis] = low_mac.words[axis] + (high_mac.words[axis] << 3);
}

static uint32 ReadCameraCollisionAngle(CollisionRay &record, sint16 strength, uint32 reverse)
{
    uint32 numerator, ratio, product;
    sint32 divisor = (sint32)record.len;
    if (!divisor)
        record.len = 1;
    divisor = (sint32)record.len;
    numerator = (record.len - record.distance) << 12;
    if (numerator == 0x80000000 && divisor == -1)
        ratio = numerator;
    else
        ratio = (uint32)((sint32)numerator / divisor);
    product = ratio * ratio;
    product = (uint32)(sint32)strength * (uint32)((sint32)product >> 12);
    if (reverse)
        return (uint32)(sint32)(sint16)(0 - (uint32)((sint32)product >> 12));
    return (uint32)((sint32)(product << 4) >> 16);
}

static void ApplyCameraCollisionQuaternion(Camera &camera, const Quaternion &q)
{
    uint32 w = camera.collision_rotation[3], x = camera.collision_rotation[0];
    uint32 y = camera.collision_rotation[1], z = camera.collision_rotation[2];
    uint32 out_x = w * q[0] + x * q[3] + y * q[2] - z * q[1];
    uint32 out_y = w * q[1] + y * q[3] + z * q[0] - x * q[2];
    uint32 out_z = w * q[2] + z * q[3] + x * q[1] - y * q[0];
    uint32 out_w = w * q[3] - x * q[0] - y * q[1] - z * q[2];
    camera.collision_rotation[0] = (uint32)((sint32)out_x >> 12);
    camera.collision_rotation[1] = (uint32)((sint32)out_y >> 12);
    camera.collision_rotation[2] = (uint32)((sint32)out_z >> 12);
    camera.collision_rotation[3] = (uint32)((sint32)out_w >> 12);
}

static sint16 ReadCameraOscillation(const Camera &camera, uint32 axis)
{
    uint32 frequency = (uint32)(sint32)(sint16)camera.shake.frequency[axis];
    uint32 time = game_clock.tick;
    uint32 phase = frequency * time;
    uint32 amplitude = (uint32)(sint32)(sint16)camera.shake.amplitude[axis];
    uint32 sine = (uint32)rsin(phase & 4095);
    sint32 product = (sint32)(amplitude * sine);
    if (product < 0)
        product = (sint32)((uint32)product + 4095);
    return (sint16)((sint32)((uint32)product << 4) >> 16);
}

static void AdjustCameraDebugValue(const PadButton &button, uint32 &val, uint32 subtract)
{
    if (button.down)
    {
        uint32 step = game_pads[1].buttons[15].down ? 32 : 8;
        uint32 value = val;
        val = subtract ? value - step : value + step;
    }
}

static void AdvanceCameraGlobalTransition(CameraTransition &transition)
{
    uint32 count = transition.count;
    if (count)
    {
        --count;
        uint32 value = transition.val;
        uint32 delta = transition.delta;
        transition.count = count;
        transition.val = value + delta;
    }
}

static uint32 MarkShakeList(ModelInstance *list, uint32 result)
{
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        result = object.activation_flags | 2;
        object.activation_flags = result;
        list = list->next;
    }
    return result;
}

Camera::Camera(VisualObject &target) : VisualObject(), proj_distance(0), proj_timer(0), proj_step(0), clamp_enabled(0), vertical_pitch(0), vertical_offset_gain(0), col_lateral_dist(0), col_rear_dist(0), col_yaw_strength(0), col_pitch_strength(0), mode(0), turn_step(0)
{
    uint32 block, index, x, y, z, flags;
    ModelInstance *source;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        clamp_start.words[axis] = 0;
        clamp_end.words[axis] = 0;
        angles.words[axis] = 0;
        shake.amplitude[axis] = 0;
        shake.decay[axis] = 0;
        shake.frequency[axis] = 0;
    }
    FUNCTION_MARKER(0x800772B8, "SLUS_003.73");
    follow.target.SetObject(&target.model);
    aim.target.SetObject(&target.model);
    this->proj_distance = 256;
    this->clamp_enabled = 0;
    this->vertical_pitch = 0;
    this->vertical_offset_gain = 0;
    Quaternion *rotations[4] = {&rotation, &collision_rotation, &blend_rotation, &saved_rotation};
    for (block = 0; block < 4; ++block)
    {
        for (index = 0; index < 3; ++index)
            (*rotations[block])[index] = 0;
        rotations[block]->w = ONE;
    }
    source = follow.target.GetObject();
    this->col_lateral_dist = (uint32)-1;
    this->col_rear_dist = (uint32)-1;
    this->col_pitch_strength = 784;
    this->col_yaw_strength = 256;
    this->turn_step = 15;
    x = source->pos.words[0];
    y = source->pos.words[1];
    z = source->pos.words[2];
    this->follow.pos.words[0] = x;
    this->follow.pos.words[1] = y;
    this->follow.pos.words[2] = z;
    this->follow.timer = 0;
    source = aim.target.GetObject();
    x = source->pos.words[0];
    y = source->pos.words[1];
    z = source->pos.words[2];
    this->aim.pos.words[0] = x;
    this->aim.pos.words[1] = y;
    this->aim.pos.words[2] = z;
    this->kind = 99;
    this->aim.timer = 0;
    this->model.flags = 1;
    PrepareView();
    AddToList(visual_lists.cameras);
    this->mode = 7;
    flags = this->lifecycle_flags;
    this->collision_radius = 0;
    this->lifecycle_flags = flags & 0xFFFD;
}

uint32 Camera::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8007741C, "SLUS_003.73");
    RemoveFromList(visual_lists.cameras);
    result = VisualObject::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return result;
}

void Camera::SetColDistances(uint32 value)
{
    FUNCTION_MARKER(0x80077490, "SLUS_003.73");
    this->col_lateral_dist = value;
    this->col_rear_dist = value;
}

void Camera::SetLateralColDistance(uint32 value)
{
    FUNCTION_MARKER(0x8007749C, "SLUS_003.73");
    this->col_lateral_dist = value;
}

void Camera::SetRearColDistance(uint32 value)
{
    FUNCTION_MARKER(0x800774A4, "SLUS_003.73");
    this->col_rear_dist = value;
}

void Camera::SetColYawStrength(uint32 value)
{
    FUNCTION_MARKER(0x800774AC, "SLUS_003.73");
    this->col_yaw_strength = value;
}

void Camera::SetColPitchStrength(uint32 value)
{
    FUNCTION_MARKER(0x800774B4, "SLUS_003.73");
    this->col_pitch_strength = value;
}

uint32 Camera::ApplyExplosionShake(uint32 kind)
{
    uint32 result = kind == 1 ? 1 : 2;
    const uint16 *preset;
    uint32 byte0, byte1, byte2;
    FUNCTION_MARKER(0x800774EC, "SLUS_003.73");
    if (kind <= 2)
    {
        preset = camera_tuning.shake_amplitude[kind == 0 ? 2 : kind == 1 ? 1 : 0];
        CopyAngleVector((uint16 *)this->shake.amplitude, preset);
        byte0 = ((uint32)(uint8)camera_tuning.shake_decay[0]);
        byte1 = ((uint32)(uint8)camera_tuning.shake_decay[1]);
        byte2 = ((uint32)(uint8)camera_tuning.shake_decay[2]);
        this->shake.decay[0] = byte0;
        this->shake.decay[1] = byte1;
        this->shake.decay[2] = byte2;
        CopyAngleVector((uint16 *)this->shake.frequency, camera_tuning.shake_frequency);
    }
    result = MarkShakeList(visual_lists.actors, result);
    return MarkShakeList(visual_lists.solids, result);
}

uint32 Camera::SetModePreset(uint32 mode)
{
    uint32 result = 20;
    FUNCTION_MARKER(0x80077624, "SLUS_003.73");
    mode &= 65535;
    if (!mode)
    {
        this->mode = 20;
        this->turn_step = 0;
    }
    else if (mode == 1 || mode == 2)
    {
        this->mode = mode == 1 ? 18 : 20;
        result = 16;
        this->turn_step = result;
    }
    return result;
}

MATRIX *Camera::PrepareView()
{
    uint32 x, y, z, offset_x, offset_y, offset_z;
    FUNCTION_MARKER(0x80077680, "SLUS_003.73");
    x = this->model.pos.words[0];
    offset_x = camera_tuning.view_offset.words[0];
    GetModelRenderView().pos.words[0] = (uint32)((sint32)(x + offset_x) >> 12);
    y = this->model.pos.words[1];
    offset_y = camera_tuning.view_offset.words[1];
    GetModelRenderView().pos.words[1] = (uint32)((sint32)(y + offset_y) >> 12);
    z = this->model.pos.words[2];
    offset_z = camera_tuning.view_offset.words[2];
    GetModelRenderView().pos.words[2] = (uint32)((sint32)(z + offset_z) >> 12);
    ConvertQuaternionToMatrix(this->rotation, GetModelRenderView().rotation);
    return SetModelRenderViewTranslation(GetModelRenderView());
}

void Camera::ResolveCollisions()
{
    FUNCTION_MARKER(0x80077748, "SLUS_003.73");
    MATRIX matrix;
    Vector3 previous, input, transformed, lateral, opposite, forward;
    Vector3 target, difference;
    CollisionRay collision;
    Quaternion q, rotated, pitch_rotation, yaw_rotation, combined, old_q, desired_q;
    ShortVector3 follow_angles, subject_angles;
    sint32 distance;
    uint32 result, angle, index, radius;
    ModelInstance *target_object;
    uint32 paused = scene_frame.paused;
    uint32 target_initialized = 0;
    previous = this->model.pos;
    if (!paused)
    {
        radius = camera_tuning.radius;
        for (index = 0; index < 4; ++index)
            q[index] = this->rotation[index];
        for (index = 0; index < 4; ++index)
            this->blend_rotation[index] = q[index];
        input.words[0] = 0;
        input.words[1] = 0;
        input.words[2] = 0 - (radius << 12);
        BuildCameraQuaternionMatrix(collision_rotation, matrix);
        RotateCameraVector(matrix, input, transformed);
        distance = (sint32)this->col_rear_dist;
        // Authorized original-bug fix uses the default distance for invalid negatives
        if (distance < -1)
            distance = -1;
        if (distance == -1)
        {
            forward.words[0] = transformed.words[0];
            forward.words[1] = 0;
            forward.words[2] = transformed.words[2];
        }
        else if (distance > 0)
        {
            input.words[0] = 0;
            input.words[1] = 0;
            input.words[2] = 0 - (this->col_rear_dist << 12);
            RotateCameraVector(matrix, input, forward);
        }
        distance = (sint32)this->col_lateral_dist;
        // Authorized original-bug fix uses the default distance for invalid negatives
        if (distance < -1)
            distance = -1;
        if (distance == -1 || distance > 0)
        {
            input.words[0] = 0 - ((distance == -1 ? camera_tuning.radius : this->col_lateral_dist) << 12);
            input.words[1] = 0;
            input.words[2] = 0;
            RotateCameraVector(matrix, input, lateral);
            opposite.words[0] = 0 - lateral.words[0];
            opposite.words[1] = 0 - lateral.words[1];
            opposite.words[2] = 0 - lateral.words[2];
        }
        target.words[0] = this->follow.pos.words[0] + transformed.words[0];
        target.words[1] = this->follow.pos.words[1] + transformed.words[1];
        target.words[2] = this->follow.pos.words[2] + transformed.words[2];
        target_initialized = 1;
        camera_tuning.prev_pos = target;
        SubtractVectors(difference.words, target.words, previous.words);
        if (this->col_lateral_dist)
        {
            for (index = 0; index < 2; ++index)
            {
                uint32 *offset = index ? opposite.words : lateral.words;
                collision.endpoints[0].words[0] = this->follow.pos.words[0];
                collision.endpoints[0].words[1] = this->follow.pos.words[1];
                collision.endpoints[0].words[2] = this->follow.pos.words[2];
                collision.endpoints[1].words[0] = this->follow.pos.words[0] + offset[0];
                collision.endpoints[1].words[1] = this->follow.pos.words[1];
                collision.endpoints[1].words[2] = this->follow.pos.words[2] + offset[2];
                InitializeCollisionRay(collision);
                collision.triggers = 0;
                collision_query.require_surface10 = true;
                QueryCollisionRay(collision, 1);
                collision_query.require_surface10 = false;
                if (collision.hit.GetObject())
                {
                    angle = ReadCameraCollisionAngle(collision, (sint16)this->col_yaw_strength, index);
                    BuildYawQuaternion(q, angle);
                    ApplyCameraCollisionQuaternion(*this, q);
                }
            }
        }
        if (this->col_rear_dist)
        {
            collision.endpoints[0].words[0] = this->follow.pos.words[0];
            collision.endpoints[0].words[1] = this->follow.pos.words[1];
            collision.endpoints[0].words[2] = this->follow.pos.words[2];
            collision.endpoints[1].words[0] = this->follow.pos.words[0] + (forward.words[0] << 1);
            collision.endpoints[1].words[1] = this->follow.pos.words[1];
            collision.endpoints[1].words[2] = this->follow.pos.words[2] + (forward.words[2] << 1);
            InitializeCollisionRay(collision);
            collision.triggers = 0;
            collision_query.require_surface10 = true;
            QueryCollisionRay(collision, 1);
            collision_query.require_surface10 = false;
            if (collision.hit.GetObject())
            {
                angle = ReadCameraCollisionAngle(collision, (sint16)this->col_pitch_strength, 0);
                BuildPitchQuaternion(q, angle);
                MultiplyQuaternions(rotated, this->collision_rotation, q);
                for (index = 0; index < 4; ++index)
                    this->collision_rotation[index] = rotated[index];
            }
        }
    }
    InterpolateQuaternion(this->blend_rotation, this->collision_rotation, 1023, this->rotation);
    input.words[0] = 0;
    input.words[1] = 0;
    input.words[2] = 0 - (camera_tuning.radius << 12);
    BuildCameraQuaternionMatrix(rotation, matrix);
    RotateCameraVector(matrix, input, transformed);
    this->model.pos.words[0] = this->follow.pos.words[0] + transformed.words[0];
    this->model.pos.words[1] = this->follow.pos.words[1] + transformed.words[1];
    this->model.pos.words[2] = this->follow.pos.words[2] + transformed.words[2];
    SubtractVectors(difference.words, this->model.pos.words, previous.words);
    result = scene_frame.paused;
    motion.velocity.words[0] = difference.words[0];
    motion.velocity.words[1] = difference.words[1];
    motion.velocity.words[2] = difference.words[2];
    if (!result)
    {
        target_object = follow.target.GetObject();
        ModelInstance *aim_object = aim.target.GetObject();
        if (target_object != aim_object)
        {
            if (!target_initialized)
            {
                GameDiagnostic("TODO Camera target is uninitialized after a live pause-state change\n");
                abort();
            }
            CalculateDirectionAngles(follow_angles.words, this->follow.pos.words, target.words);
            ModelInstance *subject = camera_subject.GetObject();
            if (!subject)
            {
                GameDiagnostic("Camera collision rotation has no subject\n");
                abort();
            }
            CalculateDirectionAngles(subject_angles.words, subject->pos.words, target.words);
            angle = ShortestCameraAngleDelta(follow_angles.components[1], subject_angles.components[1]);
            desired_q[0] = desired_q[1] = desired_q[2] = 0;
            desired_q[3] = 4096;
            for (index = 0; index < 4; ++index)
                q[index] = this->rotation[index];
            for (index = 0; index < 4; ++index)
                this->saved_rotation[index] = q[index];
            CalculateDirectionAngles(follow_angles.words, this->model.pos.words, this->follow.pos.words);
            CalculateDirectionAngles(subject_angles.words, this->model.pos.words, camera_subject.GetObject()->pos.words);
            BuildYawQuaternion(yaw_rotation, (uint32)(sint32)(sint16)(4096 - angle));
            MultiplyQuaternions(combined, yaw_rotation, this->rotation);
            BuildPitchQuaternion(pitch_rotation, ((uint32)(uint16)subject_angles.components[0] - (uint32)(uint16)follow_angles.components[0]) & 0xFFF);
            MultiplyQuaternions(rotated, combined, pitch_rotation);
            for (index = 0; index < 4; ++index)
                desired_q[index] = rotated[index];
            for (index = 0; index < 4; ++index)
                old_q[index] = this->rotation[index];
            InterpolateQuaternion(old_q, desired_q, 1023, this->rotation);
            return;
        }
    }
    return;
}

void Camera::SetTargetTransition(const uint32 *position, uint32 frames)
{
    Vector3 delta;
    uint32 x, y, z, axis, divisor;
    ModelInstance *object;
    FUNCTION_MARKER(0x80078178, "SLUS_003.73");
    if (this->follow.timer)
    {
        object = follow.target.GetObject();
        if (object && !static_cast<VisualObject *>(object->owner)->IsFinished())
        {
            object = follow.target.GetObject();
            x = object->pos.words[0];
            y = object->pos.words[1];
            z = object->pos.words[2];
            this->follow.pos.words[0] = x;
            this->follow.pos.words[1] = y;
            this->follow.pos.words[2] = z;
        }
    }
    this->follow.timer = frames;
    for (axis = 0; axis < 3; ++axis)
    {
        x = position[axis];
        y = this->follow.pos.words[axis];
        delta.words[axis] = x - y;
    }
    for (axis = 0; axis < 3; ++axis)
        this->follow.step.words[axis] = delta.words[axis];
    divisor = this->follow.timer;
    for (axis = 0; axis < 3; ++axis)
    {
        x = this->follow.step.words[axis];
        this->follow.step.words[axis] = DivideCameraWord(x, divisor);
    }
    return;
}

void Camera::ClearTargetTransition()
{
    FUNCTION_MARKER(0x8007823C, "SLUS_003.73");
    this->follow.timer = 0;
}

uint32 Camera::ClampTargetToSegment()
{
    Vector3 segment, delta;
    uint32 axis, x, y, z, partial, dot, squared, denominator, factor, result;
    FUNCTION_MARKER(0x80078310, "SLUS_003.73");
    SubtractVectors(segment.words, clamp_end.words, clamp_start.words);
    SubtractVectors(delta.words, follow.pos.words, this->clamp_start.words);
    for (axis = 0; axis < 3; ++axis)
    {
        segment.words[axis] = (uint32)((sint32)segment.words[axis] >> 6);
        delta.words[axis] = (uint32)((sint32)delta.words[axis] >> 6);
    }
    partial = segment.words[0] * delta.words[0] + segment.words[1] * delta.words[1];
    dot = partial + segment.words[2] * delta.words[2];
    if ((sint32)dot <= 0)
    {
        x = this->clamp_start.words[0];
        y = this->clamp_start.words[1];
        z = this->clamp_start.words[2];
        this->follow.pos.words[0] = x;
        this->follow.pos.words[1] = y;
        this->follow.pos.words[2] = z;
        return partial;
    }
    squared = segment.words[0] * segment.words[0] + segment.words[1] * segment.words[1] + segment.words[2] * segment.words[2];
    if ((sint32)squared < (sint32)dot)
    {
        x = this->clamp_end.words[0];
        y = this->clamp_end.words[1];
        z = this->clamp_end.words[2];
        this->follow.pos.words[0] = x;
        this->follow.pos.words[1] = y;
        this->follow.pos.words[2] = z;
        return 1;
    }
    denominator = (uint32)((sint32)squared >> 12);
    if (denominator)
        dot = DivideCameraWord(dot, denominator);
    factor = (uint32)((sint32)dot >> 6);
    result = this->clamp_start.words[0] + segment.words[0] * factor;
    this->follow.pos.words[0] = result;
    result = this->clamp_start.words[1] + segment.words[1] * factor;
    this->follow.pos.words[1] = result;
    result = this->clamp_start.words[2] + segment.words[2] * factor;
    this->follow.pos.words[2] = result;
    return result;
}

uint32 Camera::SetYawTransition(uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x8007851C, "SLUS_003.73");
    frames &= 65535;
    sint32 target_yaw = target & 4095;
    if (!frames)
    {
        angles.words[1] = target_yaw;
        camera_tuning.yaw_count = 0;
        return target_yaw;
    }

    sint32 current_yaw = (sint16)angles.words[1];
    camera_tuning.yaw_target = target_yaw;
    if (current_yaw == target_yaw)
    {
        camera_tuning.yaw_delta = 0;
        camera_tuning.yaw_count = 0;
        return 0;
    }
    camera_tuning.yaw_count = frames;
    sint32 delta = target_yaw - current_yaw;
    if (delta > 2048)
        delta -= 4096;
    else if (delta < -2048)
        delta += 4096;
    sint32 step = delta / (sint32)frames;
    camera_tuning.yaw_delta = step;
    return step;
}

uint32 SetCameraOrbitDistanceTransition(uint32 target, uint32 frames)
{
    uint32 value = target & 65535;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x800785D8, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - camera_tuning.orbit_distance.val) / (sint32)count);
        camera_tuning.orbit_distance.count = count;
        camera_tuning.orbit_distance.delta = result;
    }
    else
    {
        result = value;
        camera_tuning.orbit_distance.val = value;
        camera_tuning.orbit_distance.count = 0;
    }
    return result;
}

uint32 SetCameraOrbitHeightTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x80078618, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - camera_tuning.orbit_height.val) / (sint32)count);
        camera_tuning.orbit_height.count = count;
        camera_tuning.orbit_height.delta = result;
    }
    else
    {
        result = value;
        camera_tuning.orbit_height.val = value;
        camera_tuning.orbit_height.count = 0;
    }
    return result;
}

uint32 SetCameraTargetLateralOffsetTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x8007865C, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - camera_tuning.target_lateral.val) / (sint32)count);
        camera_tuning.target_lateral.count = count;
        camera_tuning.target_lateral.delta = result;
    }
    else
    {
        result = value;
        camera_tuning.target_lateral.val = value;
        camera_tuning.target_lateral.count = 0;
    }
    return result;
}

uint32 SetCameraTargetHeightOffsetTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x800786A0, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - camera_tuning.target_height.val) / (sint32)count);
        camera_tuning.target_height.count = count;
        camera_tuning.target_height.delta = result;
    }
    else
    {
        result = value;
        camera_tuning.target_height.val = value;
        camera_tuning.target_height.count = 0;
    }
    return result;
}

uint32 SetCameraTargetForwardOffsetTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x800786E4, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - camera_tuning.target_forward.val) / (sint32)count);
        camera_tuning.target_forward.count = count;
        camera_tuning.target_forward.delta = result;
    }
    else
    {
        result = value;
        camera_tuning.target_forward.val = value;
        camera_tuning.target_forward.count = 0;
    }
    return result;
}

uint32 Camera::SetProjDistanceTransition(uint32 target, uint32 frames)
{
    uint32 result;
    FUNCTION_MARKER(0x80078728, "SLUS_003.73");
    this->proj_timer = frames;
    frames &= 65535;
    result = target & 65535;
    if (frames)
    {
        result -= this->proj_distance;
        result = DivideCameraWord(result, this->proj_timer);
        this->proj_step = result;
    }
    else
        this->proj_distance = result;
    return result;
}

uint32 Camera::GetProjDistance()
{
    FUNCTION_MARKER(0x80078760, "SLUS_003.73");
    return this->proj_distance;
}

uint32 Camera::Update()
{
    uint32 trig_angle;
    uint32 result, x, y, z, count, value, yaw, timer, first, second, radius;
    ModelInstance *target;
    uint32 shake_word, shake_tail, magnitude, rate;
    Quaternion shake_rotations[5], shaken_rotation;
    uint32 mode_button, record_button, toggle_button, modifier, basic_button, held;
    sint32 angle, vertical, oscillation_count;
    FUNCTION_MARKER(0x8007876C, "SLUS_003.73");
    result = scene_frame.paused;
    if (result)
        return result;
    x = this->model.pos.words[0];
    y = this->model.pos.words[1];
    z = this->model.pos.words[2];
    camera_tuning.prev_pos.words[0] = x;
    camera_tuning.prev_pos.words[1] = y;
    camera_tuning.prev_pos.words[2] = z;
    count = this->follow.timer;
    if (count)
    {
        AddVectors(follow.pos.words, follow.step.words);
        count = this->follow.timer;
        this->follow.timer = count - 1;
    }
    else
    {
        target = follow.target.GetObject();
        if (target && !static_cast<VisualObject *>(target->owner)->IsFinished())
        {
            target = follow.target.GetObject();
            x = target->pos.words[0];
            y = target->pos.words[1];
            z = target->pos.words[2];
            this->follow.pos.words[0] = x;
            this->follow.pos.words[1] = y;
            this->follow.pos.words[2] = z;
        }
    }
    count = this->aim.timer;
    if (count)
    {
        AddVectors(aim.pos.words, aim.step.words);
        count = this->aim.timer;
        this->aim.timer = count - 1;
    }
    else
    {
        target = aim.target.GetObject();
        x = target->pos.words[0];
        y = target->pos.words[1];
        z = target->pos.words[2];
        this->aim.pos.words[0] = x;
        this->aim.pos.words[1] = y;
        this->aim.pos.words[2] = z;
    }
    if (this->proj_timer)
    {
        value = this->proj_distance;
        x = this->proj_step;
        count = this->proj_timer;
        this->proj_distance = value + x;
        this->proj_timer = count - 1;
    }
    if (this->clamp_enabled)
        ClampTargetToSegment();
    if (game_pads[0].buttons[15].down)
    {
        if (camera_tuning.tracking_button_released)
            camera_tuning.tracking_button_pressed = 1;
        camera_tuning.tracking_button_released = 0;
    }
    else
        camera_tuning.tracking_button_released = 1;
    mode_button = game_pads[1].buttons[6].pressed;
    if (mode_button)
    {
        record_button = game_pads[1].buttons[7].pressed;
        if (record_button)
        {
            toggle_button = game_pads[1].buttons[6].down;
            if (toggle_button)
            {
                modifier = game_pads[1].buttons[7].down;
                basic_button = game_pads[0].buttons[0].down;
                held = game_pads[0].buttons[15].down;
                if (held & modifier & basic_button)
                {
                    value = camera_tuning.manual_mode;
                    game_pads[1].buttons[6].pressed = 0;
                    game_pads[1].buttons[7].pressed = 0;
                    value ^= 1;
                    camera_tuning.manual_mode = value;
                    scene_frame.camera_paused = value;
                    if (value)
                    {
                        camera_tuning.saved.target_lateral = camera_tuning.target_lateral.val;
                        camera_tuning.saved.target_height = camera_tuning.target_height.val;
                        camera_tuning.saved.target_forward = camera_tuning.target_forward.val;
                        camera_tuning.saved.orbit_distance = camera_tuning.orbit_distance.val;
                        camera_tuning.saved.orbit_height = camera_tuning.orbit_height.val;
                        camera_tuning.saved.yaw = this->angles.words[1];
                        camera_tuning.saved.proj_distance = this->proj_distance;
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
            this->angles.words[1] = camera_tuning.saved.yaw;
            this->proj_distance = camera_tuning.saved.proj_distance;
        }
        if (game_pads[0].buttons[1].down)
            this->proj_distance = this->proj_distance - 1;
        if (game_pads[0].buttons[2].down)
            this->proj_distance = this->proj_distance + 1;
        if (game_pads[0].buttons[0].down)
            this->proj_distance = 256;
        AdjustCameraDebugValue(game_pads[1].buttons[8], camera_tuning.target_lateral.val, 1);
        AdjustCameraDebugValue(game_pads[1].buttons[9], camera_tuning.target_lateral.val, 0);
        AdjustCameraDebugValue(game_pads[1].buttons[10], camera_tuning.target_forward.val, 0);
        AdjustCameraDebugValue(game_pads[1].buttons[11], camera_tuning.target_forward.val, 1);
        AdjustCameraDebugValue(game_pads[1].buttons[4], camera_tuning.target_height.val, 1);
        AdjustCameraDebugValue(game_pads[1].buttons[5], camera_tuning.target_height.val, 0);
        AdjustCameraDebugValue(game_pads[1].buttons[6], camera_tuning.orbit_height.val, 1);
        AdjustCameraDebugValue(game_pads[1].buttons[7], camera_tuning.orbit_height.val, 0);
        AdjustCameraDebugValue(game_pads[1].buttons[0], camera_tuning.orbit_distance.val, 1);
        AdjustCameraDebugValue(game_pads[1].buttons[3], camera_tuning.orbit_distance.val, 0);
        if (game_pads[1].buttons[1].down)
        {
            value = game_pads[1].buttons[15].down;
            yaw = (uint32)(sint32)(sint16)this->angles.words[1];
            this->angles.words[1] = (yaw + (value ? 32 : 8)) & 4095;
        }
        if (game_pads[1].buttons[2].down)
        {
            value = game_pads[1].buttons[15].down;
            yaw = (uint32)(sint32)(sint16)this->angles.words[1];
            this->angles.words[1] = (yaw - (value ? 32 : 8)) & 4095;
        }
    }
AdvanceTransitions:
    AdvanceCameraGlobalTransition(camera_tuning.orbit_distance);
    AdvanceCameraGlobalTransition(camera_tuning.orbit_height);
    timer = camera_tuning.yaw_count;
    if (timer)
    {
        --timer;
        camera_tuning.yaw_count = timer;
        if (timer)
        {
            yaw = this->angles.words[1];
            value = ((uint32)(uint16)camera_tuning.yaw_delta);
            yaw = (yaw + value) & 4095;
        }
        else
            yaw = ((uint32)(uint16)camera_tuning.yaw_target);
        this->angles.words[1] = yaw;
    }
    AdvanceCameraGlobalTransition(camera_tuning.target_lateral);
    AdvanceCameraGlobalTransition(camera_tuning.target_height);
    AdvanceCameraGlobalTransition(camera_tuning.target_forward);
    value = camera_tuning.orbit_distance.val;
    first = value * value;
    value = camera_tuning.orbit_height.val;
    second = value * value;
    radius = SquareRoot0(first + second);
    x = camera_tuning.orbit_height.val;
    y = camera_tuning.orbit_distance.val;
    camera_tuning.radius = radius;
    angle = ratan2(x, y);
    value = camera_tuning.target_height.val;
    camera_tuning.view_offset.words[1] = value << 12;
    yaw = this->angles.words[1];
    x = camera_tuning.target_lateral.val;
    trig_angle = yaw & 4095;
    first = x * (uint32)rcos(trig_angle);
    z = camera_tuning.target_forward.val;
    second = z * (uint32)rsin(trig_angle);
    camera_tuning.view_offset.words[0] = 0 - first - second;
    yaw = this->angles.words[1];
    trig_angle = yaw & 4095;
    first = x * (uint32)rsin(trig_angle);
    second = z * (uint32)rcos(trig_angle);
    camera_tuning.pitch = 0 - (uint32)angle;
    camera_tuning.view_offset.words[2] = first - second;
    CalculateDirectionAngles(model.angles.words, model.pos.words, aim.pos.words);
    target = follow.target.GetObject();
    if (target && target == visual_lists.players && static_cast<Player *>(target->owner)->airborne_ticks && (sint16)this->vertical_offset_gain && !this->follow.timer)
    {
        vertical = (sint32) static_cast<VisualObject *>(target->owner)->motion.velocity.words[1];
        value = camera_tuning.vertical_ticks;
        oscillation_count = (sint32)(vertical < 0 ? value + 1 : value - 1);
        camera_tuning.vertical_ticks = (uint32)oscillation_count;
        target = follow.target.GetObject();
        value = static_cast<VisualObject *>(target->owner)->motion.velocity.words[1];
        magnitude = (uint32)(sint32)(sint16)this->vertical_offset_gain;
        first = (uint32)((sint32)(0 - value) >> 12) * magnitude;
        value = camera_tuning.vertical_offset;
        count = camera_tuning.vertical_ticks;
        value += first;
        camera_tuning.vertical_offset = value;
        if ((sint32)count > 0)
            this->follow.pos.words[1] = this->follow.pos.words[1] + value;
    }
    else
    {
        camera_tuning.vertical_offset = 0;
        camera_tuning.vertical_ticks = 0;
    }
    magnitude = (uint32)(sint32)(sint16)this->vertical_pitch;
    if (magnitude)
    {
        target = follow.target.GetObject();
        if ((sint32) static_cast<VisualObject *>(target->owner)->motion.velocity.words[1] < 0)
            camera_tuning.pitch = camera_tuning.pitch - magnitude;
        target = follow.target.GetObject();
        if ((sint32) static_cast<VisualObject *>(target->owner)->motion.velocity.words[1] > 0)
        {
            magnitude = this->vertical_pitch;
            value = camera_tuning.pitch;
            magnitude = (uint32)((sint32)(magnitude << 16) >> 17);
            camera_tuning.pitch = value - magnitude;
        }
        camera_tuning.pitch = camera_tuning.pitch & 4095;
    }
    switch (this->mode)
    {
        case 2:
        case 3:
        case 7:
        case 15:
            camera_tuning.restore_quat = 0;
            BuildView();
            break;
        case 18:
        case 19:
            camera_tuning.restore_quat = 1;
            UpdatePlayerRelativeTracking();
            break;
        case 20:
            camera_tuning.restore_quat = 1;
            UpdateFixedOriginTracking();
            break;
    }
    ResolveCollisions();
    CalculateDirectionAngles(model.angles.words, model.pos.words, aim.pos.words);
    value = ((uint32)this->shake.amplitude[0] | ((uint32)this->shake.amplitude[1] << 16));
    if (value || (sint16)this->shake.amplitude[2])
    {
        BuildYawQuaternion(shake_rotations[0], (uint32)(sint32)ReadCameraOscillation(*this, 1));
        MultiplyQuaternions(shake_rotations[1], shake_rotations[0], this->rotation);
        BuildPitchQuaternion(shake_rotations[2], (uint32)(sint32)ReadCameraOscillation(*this, 0));
        MultiplyQuaternions(shake_rotations[3], shake_rotations[1], shake_rotations[2]);
        BuildRollQuaternion(shake_rotations[4], (uint32)(sint32)ReadCameraOscillation(*this, 2));
        MultiplyQuaternions(shaken_rotation, shake_rotations[3], shake_rotations[4]);
        this->rotation = shaken_rotation;
        // Object storage is aligned for the original paired LWL/LWR reads
        shake_word = ((uint32)this->shake.amplitude[0] | ((uint32)this->shake.amplitude[1] << 16));
        shake_word = ((uint32)this->shake.amplitude[0] | ((uint32)this->shake.amplitude[1] << 16));
        shake_tail = this->shake.amplitude[2];
        vertical = (sint16)this->shake.amplitude[0];
        magnitude = this->shake.amplitude[0];
        rate = this->shake.decay[0];
        this->shake.amplitude[0] = vertical < 0 ? magnitude + rate : magnitude - rate;
        vertical = (sint16)this->shake.amplitude[1];
        magnitude = this->shake.amplitude[1];
        rate = this->shake.decay[1];
        this->shake.amplitude[1] = vertical < 0 ? magnitude + rate : magnitude - rate;
        vertical = (sint16)this->shake.amplitude[2];
        magnitude = this->shake.amplitude[2];
        rate = this->shake.decay[2];
        this->shake.amplitude[2] = vertical < 0 ? magnitude + rate : magnitude - rate;
        if ((shake_word ^ this->shake.amplitude[0]) & 0x8000)
            this->shake.amplitude[0] = 0;
        if (((shake_word >> 16) ^ this->shake.amplitude[1]) & 0x8000)
            this->shake.amplitude[1] = 0;
        if ((shake_tail ^ this->shake.amplitude[2]) & 0x8000)
            this->shake.amplitude[2] = 0;
    }
    PrepareView();
    result = camera_tuning.restore_quat;
    if (result)
    {
        x = this->saved_rotation[0];
        y = this->saved_rotation[1];
        z = this->saved_rotation[2];
        value = this->saved_rotation[3];
        this->rotation[0] = x;
        this->rotation[1] = y;
        this->rotation[2] = z;
        this->rotation[3] = value;
    }
    return result;
}

uint32 Camera::BuildView()
{
    uint32 trig_angle;
    FUNCTION_MARKER(0x800793C8, "SLUS_003.73");
    Vector3 offset;
    offset.words[0] = 0;
    offset.words[1] = 0;
    offset.words[2] = camera_tuning.radius << 12;
    Vector3 up = {{{0, 0x1F4000, 0}}};
    Vector3 translated, forward, normal_up;
    uint32 rotation = ((uint32)this->angles.words[0] | ((uint32)this->angles.words[1] << 16));
    uint32 roll = this->angles.words[2];
    uint32 steps[4], pass, step, i, x, y, z, sine, cosine, sum;
    sint32 divisor;
    MATRIX matrix;
    steps[0] = camera_tuning.pitch & 0xFFF;
    steps[1] = (rotation >> 16) & 0xFFF;
    steps[2] = rotation & 0xFFF;
    steps[3] = roll & 0xFFF;
    for (pass = 0; pass < 2; ++pass)
    {
        Vector3 &vector = pass ? up : offset;
        if (pass)
            steps[0] = camera_tuning.pitch & 4095;
        for (step = 0; step < 4; ++step)
        {
            trig_angle = steps[step];
            sine = (uint32)rsin(trig_angle);
            cosine = (uint32)rcos(trig_angle);
            x = (uint32)((sint32)vector.words[0] >> 12);
            y = (uint32)((sint32)vector.words[1] >> 12);
            z = (uint32)((sint32)vector.words[2] >> 12);
            if (step == 1)
            {
                vector.words[0] = x * cosine + z * sine;
                vector.words[2] = z * cosine - x * sine;
            }
            else if (step == 3)
            {
                vector.words[0] = x * cosine - y * sine;
                vector.words[1] = y * cosine + x * sine;
            }
            else
            {
                vector.words[1] = y * cosine - z * sine;
                vector.words[2] = z * cosine + y * sine;
            }
        }
    }
    AddVectors(translated.words, offset.words, follow.pos.words);
    sum = 0;
    for (i = 0; i < 3; ++i)
    {
        forward.words[i] = (uint32)((sint32)follow.pos.words[i] >> 12) - (uint32)((sint32)translated.words[i] >> 12);
        sum += forward.words[i] * forward.words[i];
    }
    divisor = (sint32)SquareRoot0(sum);
    if (!divisor)
        divisor = 1;
    for (i = 0; i < 3; ++i)
    {
        uint32 value = forward.words[i] << 12;
        forward.words[i] = value == 0x80000000 && divisor == -1 ? value : (uint32)((sint32)value / divisor);
    }
    sum = 0;
    for (i = 0; i < 3; ++i)
    {
        normal_up.words[i] = (uint32)((sint32)up.words[i] >> 12);
        sum += normal_up.words[i] * normal_up.words[i];
    }
    divisor = (sint32)SquareRoot0(sum);
    if (!divisor)
        divisor = 1;
    for (i = 0; i < 3; ++i)
    {
        uint32 value = normal_up.words[i] << 12;
        normal_up.words[i] = value == 0x80000000 && divisor == -1 ? value : (uint32)((sint32)value / divisor);
    }
    xport_gte_write_control(0, normal_up.words[0]);
    xport_gte_write_control(2, normal_up.words[1]);
    xport_gte_write_control(4, normal_up.words[2]);
    xport_gte_write_data(11, forward.words[2]);
    xport_gte_write_data(9, forward.words[0]);
    xport_gte_write_data(10, forward.words[1]);
    ExecuteGeometryCommand(0x170000C);
    x = xport_gte_read_data(25);
    y = xport_gte_read_data(26);
    z = xport_gte_read_data(27);
    matrix.m[0][0] = (sint16)((sint32)x >> 12);
    matrix.m[1][0] = (sint16)((sint32)y >> 12);
    matrix.m[2][0] = (sint16)((sint32)z >> 12);
    matrix.m[0][1] = (sint16)normal_up.words[0];
    matrix.m[1][1] = (sint16)normal_up.words[1];
    matrix.m[2][1] = (sint16)normal_up.words[2];
    matrix.m[0][2] = (sint16)forward.words[0];
    matrix.m[1][2] = (sint16)forward.words[1];
    matrix.m[2][2] = (sint16)forward.words[2];
    return ConvertMatrixToQuaternion(matrix, this->collision_rotation);
}

uint32 Camera::UpdatePlayerRelativeTracking()
{
    FUNCTION_MARKER(0x80079DC8, "SLUS_003.73");
    ModelInstance *target, *original_target;
    uint32 distance = 0, target_distance;
    uint32 angle, table_index, product, magnitude, scale, previous, value;
    const uint32 *origin;
    ShortVector3 *history;
    uint32 *initialized;
    sint32 distance_initialized = 0;
    sint32 delta;
    ShortVector3 direction, target_angles, turn, position_angles, final_target_angles;
    original_target = camera_subject.GetObject();
    if (!original_target)
    {
        aim.target.SetObject(follow.target.GetObject());
        return BuildView();
    }
    target = camera_subject.GetObject();
    aim.target.SetObject(original_target);
    camera_tuning.view_offset.words[0] = 0;
    camera_tuning.view_offset.words[1] = (uint32)-368640;
    camera_tuning.view_offset.words[2] = 0;
    if (target)
    {
        distance = MeasurePositionDistance(follow.pos.words, target->pos.words);
        distance_initialized = 1;
    }
    origin = this->model.pos.words;
    CalculateDirectionAngles(direction.words, this->follow.pos.words, origin);
    CalculateDirectionAngles(target_angles.words, camera_subject.GetObject()->pos.words, origin);
    history = &camera_tuning.tracking[0].angles;
    initialized = &camera_tuning.tracking[0].angles_initialized;
    if (!*initialized)
    {
        *initialized = 1;
        *history = target_angles;
    }
    delta = ShortestCameraAngleDelta(history->words[1], target_angles.components[1]);
    target = camera_subject.GetObject();
    angle = history->words[1];
    target_angles.components[1] = (sint16)((angle + (uint32)(delta >> 1) + 4096) & 4095);
    target_distance = MeasurePositionDistance(target->pos.words, origin);
    target = camera_subject.GetObject();
    if (target)
    {
        CalculateDirectionAngles(turn.words, follow.pos.words, target->pos.words);
        delta = ShortestCameraAngleDelta(this->angles.words[1], turn.components[1]);
        if (!distance_initialized)
        {
            // TODO The original reads an uninitialized distance if its live target changes here
            GameDiagnostic("Uninitialized tracking-camera distance at %08X\n", 0x80079DC8);
            abort();
        }
        TurnTrackingCamera(*this, delta, (sint32)distance, 1);
    }
    CalculateDirectionAngles(position_angles.words, this->follow.pos.words, origin);
    CalculateDirectionAngles(final_target_angles.words, camera_subject.GetObject()->pos.words, origin);
    table_index = ((uint32)(uint16)position_angles.components[1] - (uint32)(uint16)final_target_angles.components[1]) & 4095;
    angle = this->angles.words[1];
    table_index += ((uint32)(uint16)target_angles.components[1] - angle) & 4095;
    table_index &= 4095;
    value = (uint32)rsin(table_index);
    product = value * target_distance;
    magnitude = (uint32)((sint32)product >> 12);
    value = (uint32)((sint32)product >> 31);
    magnitude = (magnitude ^ value) - value;
    scale = (uint32)((sint32)(magnitude * 6553) >> 12);
    if (!camera_tuning.tracking[0].distance_initialized)
    {
        camera_tuning.tracking[0].distance_initialized = 1;
        camera_tuning.tracking[0].distance = scale;
    }
    previous = camera_tuning.tracking[0].distance;
    value = (uint32)((sint32)(scale + previous) >> 1);
    camera_tuning.radius = value;
    if ((sint32)value < 1700)
        camera_tuning.radius = 1700;
    value = camera_tuning.radius;
    if ((sint32)value >= 3001)
        camera_tuning.radius = 3000;
    camera_tuning.pitch = 130;
    camera_tuning.tracking[0].distance = scale;
    *history = target_angles;
    return BuildView();
}

uint32 Camera::UpdateFixedOriginTracking()
{
    FUNCTION_MARKER(0x8007A1A8, "SLUS_003.73");
    ModelInstance *target, *original_target;
    uint32 distance = 0, target_distance;
    uint32 angle, table_index, product, magnitude, scale, previous, value;
    const uint32 *origin;
    ShortVector3 *history;
    uint32 *initialized;
    sint32 distance_initialized = 0;
    sint32 delta;
    ShortVector3 direction, target_angles, turn, position_angles, final_target_angles;
    original_target = camera_subject.GetObject();
    if (!original_target)
    {
        aim.target.SetObject(follow.target.GetObject());
        return BuildView();
    }
    target = camera_subject.GetObject();
    aim.target.SetObject(original_target);
    camera_tuning.view_offset.words[0] = 0;
    camera_tuning.view_offset.words[1] = (uint32)-368640;
    camera_tuning.view_offset.words[2] = 0;
    if (target)
    {
        distance = MeasurePositionDistance(follow.pos.words, target->pos.words);
        distance_initialized = 1;
    }
    origin = camera_tuning.prev_pos.words;
    CalculateDirectionAngles(direction.words, this->follow.pos.words, origin);
    CalculateDirectionAngles(target_angles.words, camera_subject.GetObject()->pos.words, origin);
    history = &camera_tuning.tracking[1].angles;
    initialized = &camera_tuning.tracking[1].angles_initialized;
    if (!*initialized)
    {
        *initialized = 1;
        *history = target_angles;
    }
    delta = ShortestCameraAngleDelta(history->words[1], target_angles.components[1]);
    target = camera_subject.GetObject();
    angle = history->words[1];
    target_angles.components[1] = (sint16)((angle + (uint32)(delta >> 1) + 4096) & 4095);
    target_distance = MeasurePositionDistance(target->pos.words, origin);
    target = camera_subject.GetObject();
    if (target)
    {
        CalculateDirectionAngles(turn.words, follow.pos.words, target->pos.words);
        delta = ShortestCameraAngleDelta(this->angles.words[1], turn.components[1]);
        if (!distance_initialized)
        {
            // TODO The original reads an uninitialized distance if its live target changes here
            GameDiagnostic("Uninitialized tracking-camera distance at %08X\n", 0x8007A1A8);
            abort();
        }
        TurnTrackingCamera(*this, delta, (sint32)distance, 0);
    }
    CalculateDirectionAngles(position_angles.words, this->follow.pos.words, origin);
    CalculateDirectionAngles(final_target_angles.words, camera_subject.GetObject()->pos.words, origin);
    if (!camera_tuning.tracking[1].distance_initialized)
    {
        previous = camera_tuning.radius;
        camera_tuning.tracking[1].distance_initialized = 1;
        camera_tuning.tracking[1].distance = previous;
    }
    previous = camera_tuning.tracking[1].distance;
    camera_tuning.pitch = 100;
    *history = target_angles;
    value = (uint32)((sint32)(previous + 1700) >> 1);
    camera_tuning.radius = value;
    camera_tuning.tracking[1].distance = value;
    return BuildView();
}

void ResetCameraDefaults()
{
    FUNCTION_MARKER(0x8007A5FC, "SLUS_003.73");
    camera_tuning.shake_amplitude[2][2] = 0x32;
    camera_tuning.shake_amplitude[1][2] = 0x1E;
    camera_tuning.shake_decay[0] = 3;
    camera_tuning.shake_decay[2] = 3;
    camera_tuning.shake_frequency[0] = 0x258;
    camera_tuning.shake_frequency[2] = 0x258;
    camera_tuning.shake_amplitude[2][0] = 0x19;
    camera_tuning.shake_amplitude[2][1] = 0;
    camera_tuning.shake_amplitude[1][0] = 0;
    camera_tuning.shake_amplitude[1][1] = 0;
    camera_tuning.shake_amplitude[0][0] = 0;
    camera_tuning.shake_amplitude[0][1] = 0;
    camera_tuning.shake_amplitude[0][2] = 0x19;
    camera_tuning.shake_decay[1] = 0;
    camera_tuning.shake_frequency[1] = 0;
    camera_tuning.view_offset.words[0] = 0x00000000;
    camera_tuning.view_offset.words[1] = 0xFFFA6000;
    camera_tuning.view_offset.words[2] = 0x00000000;
}
