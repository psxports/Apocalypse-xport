#ifndef GAME_MOVIE_STARTUP_H
#define GAME_MOVIE_STARTUP_H
#include "psx.h"
uint32 apocalypse_play_movie(uint32 movie);
uint32 apocalypse_str_pump(void);
uint32 apocalypse_str_start(uint32 mode);
uint32 apocalypse_str_next(uint32 *frame,uint32 *header);
uint32 apocalypse_str_free(uint32 frame);
void apocalypse_str_stop(void);
uint32 sub_80097B1C(uint32 ring,uint32 count);
#endif

