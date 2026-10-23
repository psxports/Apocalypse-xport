#include <string.h>
#include <stdlib.h>

#include "model_instance.h"
#include "psx.h"
#include "global.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "resource_script.h"
#include "model_render_view.h"
#include "character.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_matrix.h"
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_clipping.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "texture_resources.h"

static const ModelViewport initial_viewport = {512, 240, 0, 0, 10, 30000, 256, 120, 256};
ModelViewport model_viewport = {512, 240, 0, 0, 10, 30000, 256, 120, 256};
ModelRenderState model_render_state = {0};

void ResetModelRenderState()
{
    ModelRenderState empty = {0};
    model_render_state = empty;
    model_viewport = initial_viewport;
    ResetModelLightingVals();
    ResetScratchProjVertices();
    ResetModelProjVertices();
}

uint32 AdvanceModelRenderCounter()
{
    FUNCTION_MARKER(0x8007EBC4, "SLUS_003.73");
    return ++model_render_state.pass_count;
}

// Emit the selected primitive without resetting its existing command bits
