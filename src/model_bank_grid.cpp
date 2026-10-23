#include <stdlib.h>

#include "game_diagnostics.h"
#include "model_bank_grid.h"

struct ModelGridFile
{
    sint32 min_x;
    sint32 min_z;
    sint32 max_x;
    sint32 max_z;
    uint16 columns;
    uint16 rows;
};

typedef char ModelGridFileSize[sizeof(ModelGridFile) == 20 ? 1 : -1];

struct ModelGridCellFile
{
    uint32 reserved[2];
    uint32 count;
};

typedef char ModelGridCellFileSize[sizeof(ModelGridCellFile) == 12 ? 1 : -1];

static ModelBankGrid model_grids[2];

static void ReleaseModelGrid(ModelBankGrid &grid)
{
    for (uint32 x = 0; x < 20; ++x)
        for (uint32 z = 0; z < 20; ++z)
        {
            ModelGridCell &cell = grid.cells[x][z];
            RetireModelMemory(cell.objects);
            cell.objects = 0;
            cell.count = 0;
            cell.present = false;
        }
    grid.active = false;
}

const ModelBankGrid &ModelBankGrid::Get(uint32 slot)
{
    if (slot >= 2)
    {
        GameDiagnostic(("Invalid model grid %u\n", slot));
        abort();
    }
    return model_grids[slot];
}

void ModelBankGrid::Clear(sint32 slot)
{
    FUNCTION_MARKER(0x8007DC5C, "SLUS_003.73");
    if (slot >= 0 && slot < 2)
        ReleaseModelGrid(model_grids[slot]);
}

void ModelBankGrid::ClearAll()
{
    FUNCTION_MARKER(0x8007DCDC, "SLUS_003.73");
    ModelBankGrid::Clear(0);
    ModelBankGrid::Clear(1);
}

static bool DecodeModelGrid(ModelBankGrid &grid, const uint8 *data, uint32 bytes, ModelInstance *objects, uint32 object_count)
{
    if (!data || bytes < 20 || (object_count && !objects))
        return false;
    const ModelGridFile &file = *(const ModelGridFile *)data;
    grid.min_x = file.min_x;
    grid.min_z = file.min_z;
    grid.max_x = file.max_x;
    grid.max_z = file.max_z;
    grid.columns = file.columns;
    grid.rows = file.rows;
    if (grid.columns <= 0 || grid.columns > 20 || grid.rows <= 0 || grid.rows > 20)
        return false;
    sint32 extent = (uint32)grid.max_x - grid.min_x;
    grid.cell_size = extent / grid.columns;
    uint32 cursor = 20;
    for (sint32 z = 0; z < grid.rows; ++z)
        for (sint32 x = 0; x < grid.columns; ++x)
        {
            if (cursor > bytes || bytes - cursor < 12)
                return false;
            const ModelGridCellFile &cell_file = *(const ModelGridCellFile *)(data + cursor);
            uint32 count = cell_file.count;
            cursor += 12;
            if (count > (bytes - cursor) / 4)
                return false;
            uint32 list_bytes = count * 4;
            const uint32 *indices = (const uint32 *)(data + cursor);
            if (bytes - cursor - list_bytes < 4 || indices[count])
                return false;
            ModelGridCell &cell = grid.cells[x][z];
            if (count > 0xFFFFFFFF / sizeof(*cell.objects))
                return false;
            if (count)
            {
                cell.objects = (ModelInstance **)malloc(count * sizeof(*cell.objects));
                if (!cell.objects)
                {
                    GameDiagnostic(("Cannot allocate model grid cell\n"));
                    abort();
                }
            }
            cell.count = count;
            cell.present = true;
            for (uint32 i = 0; i < cell.count; ++i)
            {
                uint32 idx = indices[i];
                if (idx >= object_count)
                    return false;
                cell.objects[i] = objects + idx;
            }
            cursor += list_bytes + 4;
        }
    grid.active = cursor == bytes;
    return grid.active;
}

void ModelBankGrid::Initialize(uint32 slot, const uint8 *data, uint32 bytes, ModelInstance *objects, uint32 object_count)
{
    FUNCTION_MARKER(0x8007E41C, "SLUS_003.73");
    if (slot >= 2)
    {
        GameDiagnostic(("Invalid model grid %u\n", slot));
        abort();
    }
    ModelBankGrid decoded = {0};
    if (!DecodeModelGrid(decoded, data, bytes, objects, object_count))
    {
        ReleaseModelGrid(decoded);
        GameDiagnostic(("Invalid model grid block\n"));
        abort();
    }
    ReleaseModelGrid(model_grids[slot]);
    model_grids[slot] = decoded;
}
