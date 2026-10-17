#include "game_sdk_bindings.h"
#include "model_grid.h"
#include <stdlib.h>
#include <string.h>

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

const ModelBankGrid &GetModelBankGrid(uint32 slot)
{
    if (slot >= 2)
    {
        GameDiagnostic("Invalid model grid %u\n", slot);
        abort();
    }
    return model_grids[slot];
}

uint32 ClearModelBankGrid(sint32 slot)
{
    FUNCTION_MARKER(0x8007DC5C, "SLUS_003.73");
    if ((uint32)slot < 2)
        ReleaseModelGrid(model_grids[slot]);
    return 0;
}

uint32 ClearModelBankGrids()
{
    FUNCTION_MARKER(0x8007DCDC, "SLUS_003.73");
    ClearModelBankGrid(0);
    return ClearModelBankGrid(1);
}

static bool DecodeModelGrid(ModelBankGrid &grid, const uint8 *data, uint32 bytes, ModelInstance *objects, uint32 object_count)
{
    if (!data || bytes < 20 || (object_count && !objects))
        return false;
    grid.min_x = (sint32)xport_load_le32(data);
    grid.min_z = (sint32)xport_load_le32(data + 4);
    grid.max_x = (sint32)xport_load_le32(data + 8);
    grid.max_z = (sint32)xport_load_le32(data + 12);
    grid.columns = (sint16)xport_load_le16(data + 16);
    grid.rows = (sint16)xport_load_le16(data + 18);
    if (grid.columns <= 0 || grid.columns > 20 || grid.rows <= 0 || grid.rows > 20)
        return false;
    sint32 extent = (sint32)((uint32)grid.max_x - (uint32)grid.min_x);
    grid.cell_size = extent / grid.columns;
    uint32 cursor = 20;
    for (sint32 z = 0; z < grid.rows; ++z)
        for (sint32 x = 0; x < grid.columns; ++x)
        {
            if (cursor > bytes || bytes - cursor < 12)
                return false;
            sint32 count = (sint32)xport_load_le32(data + cursor + 8);
            cursor += 12;
            if (count < 0 || (uint32)count > (bytes - cursor) / 4)
                return false;
            uint32 list_bytes = (uint32)count * 4;
            if (bytes - cursor - list_bytes < 4 || xport_load_le32(data + cursor + list_bytes))
                return false;
            ModelGridCell &cell = grid.cells[x][z];
            if ((uint32)count > 0xFFFFFFFF / sizeof(*cell.objects))
                return false;
            if (count)
            {
                cell.objects = (ModelInstance **)malloc((uint32)count * sizeof(*cell.objects));
                if (!cell.objects)
                {
                    GameDiagnostic("Cannot allocate model grid cell\n");
                    abort();
                }
            }
            cell.count = (uint32)count;
            cell.present = true;
            for (uint32 i = 0; i < cell.count; ++i)
            {
                uint32 index = xport_load_le32(data + cursor + i * 4);
                if (index >= object_count)
                    return false;
                cell.objects[i] = objects + index;
            }
            cursor += list_bytes + 4;
        }
    grid.active = cursor == bytes;
    return grid.active;
}

void InitializeModelBankGrid(uint32 slot, const uint8 *data, uint32 bytes, ModelInstance *objects, uint32 object_count)
{
    FUNCTION_MARKER(0x8007E41C, "SLUS_003.73");
    if (slot >= 2)
    {
        GameDiagnostic("Invalid model grid %u\n", slot);
        abort();
    }
    ModelBankGrid decoded;
    memset(&decoded, 0, sizeof(decoded));
    if (!DecodeModelGrid(decoded, data, bytes, objects, object_count))
    {
        ReleaseModelGrid(decoded);
        GameDiagnostic("Invalid model grid block\n");
        abort();
    }
    ReleaseModelGrid(model_grids[slot]);
    model_grids[slot] = decoded;
}
