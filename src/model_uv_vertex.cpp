#include "model_uv_vertex.h"

static const sint16 model_uv_waves[16][64] = {
#include "model_uv_waves.inc"

};

static sint16 ReadUvWave(uint8 parameter, uint32 phase)
{
    const uint32 amplitude = parameter >> 4;
    if (!amplitude)
        return 0;
    phase = (phase + ((parameter & 15) << 2)) & 63;
    return model_uv_waves[amplitude][phase];
}

sint16 ModelUvVertex::ReadHorizontalOffset(uint32 phase) const
{
    FUNCTION_MARKER(0x8007FED0, "SLUS_003.73");
    return ReadUvWave(wave_x, phase);
}

sint16 ModelUvVertex::ReadVerticalOffset(uint32 phase) const
{
    FUNCTION_MARKER(0x8007FF20, "SLUS_003.73");
    return ReadUvWave(wave_y, phase);
}
