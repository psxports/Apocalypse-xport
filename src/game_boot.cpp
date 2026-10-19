#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "world_gravity.h"
#include "game_diagnostics.h"
extern "C"
{
#include "xport.h"
#include "psx_gpu.h"
#include "game_gpu_present.h"
#include "psx_press.h"
}
#include "object_initializers.h"
#include "scene_lifecycle.h"
#include "sequence_transport.h"
#include "camera.h"

extern "C"
{
    extern const uint32 xport_gpu_graph_type_address = 0;
}

static bool game_constructors_initialized;

void ResetGameConstructors()
{
    game_constructors_initialized = false;
}

void InitializeGameConstructors()
{
    if (game_constructors_initialized)
        return;
    game_constructors_initialized = true;
    /* Empty constructors and finite delay loops have no guest data effects */
    ResetGravityVector();
    ResetCameraDefaults();
}

void SceneVBlankCallback()
{
    mdec_psyq_pump((uint32)33868800 / FIELD_RATE);
    TickMusicSequences();
    AdvanceSceneVBlank();
}
