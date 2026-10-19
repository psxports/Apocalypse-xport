#include <stddef.h>
#include <stdlib.h>

#include "model_attachment_data.h"
#include "game_diagnostics.h"

// Compact attachment points cross the GTE input boundary
typedef char ModelAttachmentSizeCheck[sizeof(ModelAttachmentPoint) == 8 ? 1 : -1];
typedef char ModelAttachmentBoneCheck[offsetof(ModelAttachmentPoint, bone) == 6 ? 1 : -1];

// Original attachment table 800A6868
static const ModelAttachmentPoint war_attachments_points[] = {
    {0, 32, 0, 4}, {0, 32, -128, 4}, {0, 0, 0, 18}, {0, 0, -128, 19}, {0, 0, 0, 17}, {0, 0, 0, 1}, {0, 32, 0, 1}, {0, 0, 0, 6}, {3, -64, -2, 6}, {27, 0, 0, 8}, {27, -64, 3, 8}, {0, 0, 0, 4}, {7, 32, 3, 4}, {-16, 0, 0, 9}, {-16, -16, -2, 9},
};

const ModelAttachmentTable WarAttachments = {15, war_attachments_points};

// Original attachment table 800A6A70
static const ModelAttachmentPoint the_beast_attachments_points[] = {
    {0, 32, 0, 3}, {-16, 32, -64, 8}, {16, 32, -64, 8}, {0, 32, 512, 8}, {0, 96, -32, 8}, {0, 0, 0, 13}, {0, 0, 0, 16},
};

const ModelAttachmentTable TheBeastAttachments = {7, the_beast_attachments_points};

// Original attachment table 800A6BD0
static const ModelAttachmentPoint plague_attachments_points[] = {
    {0, 0, 0, 11},
    {0, 0, 0, 14},
};

const ModelAttachmentTable PlagueAttachments = {2, plague_attachments_points};

// Original attachment table 800A6BF4
static const ModelAttachmentPoint death_attachments_points[] = {
    {0, 0, 0, 10},
    {100, 100, 230, 13},
    {25, 40, 40, 13},
    {0, 0, 0, 15},
};

const ModelAttachmentTable DeathAttachments = {4, death_attachments_points};

// Original attachment table 800A6C5C
static const ModelAttachmentPoint swat_mode1_attachments_points[] = {
    {0, 64, 0, 10},
    {0, 0, 0, 12},
};

const ModelAttachmentTable SwatMode1Attachments = {2, swat_mode1_attachments_points};

// Original attachment table 800A6C70
static const ModelAttachmentPoint swat_mode2_attachments_points[] = {
    {0, 0, 0, 9},
    {0, 0, 0, 15},
};

const ModelAttachmentTable SwatMode2Attachments = {2, swat_mode2_attachments_points};

// Original attachment table 800A6C84
static const ModelAttachmentPoint swat_mode4_attachments_points[] = {
    {0, 0, 0, 4},
    {0, 0, 0, 2},
};

const ModelAttachmentTable SwatMode4Attachments = {2, swat_mode4_attachments_points};

// Original attachment table 800A6C98
static const ModelAttachmentPoint swat_mode256_attachments_points[] = {
    {-64, 0, 0, 5},
    {64, 0, 0, 5},
};

const ModelAttachmentTable SwatMode256Attachments = {2, swat_mode256_attachments_points};

// Original attachment table 800A6CAC
static const ModelAttachmentPoint swat_mode64_attachments_points[] = {
    {0, 0, 0, 10},
    {0, 0, 0, 8},
};

const ModelAttachmentTable SwatMode64Attachments = {2, swat_mode64_attachments_points};

// Original attachment table 800A6CC0
static const ModelAttachmentPoint swat_mode16_attachments_points[] = {
    {0, 0, 0, 10},
    {0, 0, 0, 13},
};

const ModelAttachmentTable SwatMode16Attachments = {2, swat_mode16_attachments_points};

// Original attachment table 800A6CD4
static const ModelAttachmentPoint swat_mode8_attachments_points[] = {
    {0, 0, 0, 15},
    {0, 0, 0, 16},
};

const ModelAttachmentTable SwatMode8Attachments = {2, swat_mode8_attachments_points};

// Original attachment table 800A6D5C
static const ModelAttachmentPoint tank_attachments_points[] = {
    {0, 0, 0, 3}, {0, 0, 0, 1}, {0, 0, -1024, 2}, {32, -64, -256, 1}, {-32, -64, -256, 1},
};

const ModelAttachmentTable TankAttachments = {5, tank_attachments_points};

// Original attachment table 800A6D88
static const ModelAttachmentPoint turret_default_attachments_points[] = {
    {0, -128, -32, 0},
    {0, -64, 128, 0},
    {0, 0, -264, 3},
};

const ModelAttachmentTable TurretDefaultAttachments = {3, turret_default_attachments_points};

// Original attachment table 800A6DE4
static const ModelAttachmentPoint turret_raised_attachments_points[] = {
    {0, -128, -32, 0},
    {0, -64, 128, 0},
    {0, 0, -264, 2},
};

const ModelAttachmentTable TurretRaisedAttachments = {3, turret_raised_attachments_points};

// Original attachment table 800A6E34
static const ModelAttachmentPoint turret_level4_attachments_points[] = {
    {0, -128, -32, 1},
    {0, -64, 200, 1},
    {0, 0, -340, 2},
};

const ModelAttachmentTable TurretLevel4Attachments = {3, turret_level4_attachments_points};

// Original attachment table 800A6E78
static const ModelAttachmentPoint turret_level10_attachments_points[] = {
    {0, -128, -32, 1},
    {0, -64, 200, 1},
    {0, 0, -340, 2},
};

const ModelAttachmentTable TurretLevel10Attachments = {3, turret_level10_attachments_points};

// Original attachment table 800A6ECC
static const ModelAttachmentPoint player_attachments_points[] = {
    {0, -12, -64, 13}, {0, 0, 0, 10}, {0, 0, 0, 0}, {0, 0, 0, 8}, {0, 0, 0, 11}, {0, 0, 0, 6}, {0, 0, 0, 3}, {0, 0, 0, 13}, {0, 0, 0, 10}, {0, 0, 0, 14},
};

const ModelAttachmentTable PlayerAttachments = {10, player_attachments_points};

// Original attachment table 800ED474
static const ModelAttachmentPoint croc_attachments_points[] = {
    {0, 0, 0, 9},
};

const ModelAttachmentTable CrocAttachments = {1, croc_attachments_points};

// Original attachment table 800ED480
static const ModelAttachmentPoint mini_tank_attachments_points[] = {
    {0, 0, -200, 2},
    {0, 0, 0, 3},
};

const ModelAttachmentTable MiniTankAttachments = {2, mini_tank_attachments_points};

const ModelAttachmentPoint &GetModelAttachmentPoint(const ModelAttachmentTable *table, uint32 index)
{
    if (!table || index >= table->count)
    {
        GameDiagnostic("Invalid model attachment point\n");
        abort();
    }
    return table->points[index];
}
