#ifndef MOVIE_LIFECYCLE_H
#define MOVIE_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

#include "vector3.h"

enum
{
    MovieRecordCount = 34,
    MoviePositionCount = 8
};

struct MoviePlaybackState
{
    Vector3 positions[MoviePositionCount];
    bool double_buffer;
    bool at_end;
    bool active;
    bool refresh_pending;
    // Updated by the MDEC output callback
    volatile uint32 slice_idx;
    uint32 decode_idx;
    bool frame_ready;
    bool paused;
    uint32 balance_ticks;
    uint32 frame_number;
    uint32 start_sector;
    uint32 last_frame;
    uint32 decode_bytes;
    uint32 ring_sectors;
    uint8 slice_count;
    uint16 origin_x;
    uint16 origin_y;
    uint16 end_x;
    uint16 width;
    uint16 height;
    sint32 pos_count;
    bool rgb24;
    uint8 sel;
    volatile sint16 slice_x;
    volatile sint16 slice_y;
    sint16 slice_width;
    sint16 slice_height;
};

extern MoviePlaybackState movie_playback;
void ResetMoviePlaybackState();

uint32 PrepareMovieTex(uint32 resource);
uint32 QueueMoviePos(const Vector3 *pos);
uint32 InitializeMovieResourceTable();
uint32 PlayMovie(uint32 movie);
uint32 SelectMovieRec(uint32 movie);
uint32 RequestCdStreamRefresh();
uint32 PrepareMoviePlaybackBuffers();
uint32 ResetMoviePlayback();
uint32 UpdateMoviePlayback();

#endif
