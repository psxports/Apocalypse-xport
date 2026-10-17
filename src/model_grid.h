#ifndef MODEL_GRID_H
#define MODEL_GRID_H

#include "model_instance.h"

extern "C"
{
#include "psx.h"
}

struct ModelGridCell
{
    ModelInstance **objects;
    uint32 count;
    bool present;
};

struct ModelBankGrid
{
    bool active;
    sint32 min_x;
    sint32 min_z;
    sint32 max_x;
    sint32 max_z;
    sint32 cell_size;
    sint16 columns;
    sint16 rows;
    ModelGridCell cells[20][20];
};

const ModelBankGrid &GetModelBankGrid(uint32 slot);
uint32 ClearModelBankGrid(sint32 slot);
uint32 ClearModelBankGrids();
void InitializeModelBankGrid(uint32 slot, const uint8 *data, uint32 bytes, ModelInstance *objects, uint32 object_count);

#endif
