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
    uint32 double_buffer;
    uint32 at_end;
    uint32 active;
    uint32 refresh_pending;
    // Updated by the MDEC output callback
    volatile uint32 slice_index;
    uint32 decode_index;
    uint32 frame_ready;
    uint32 paused;
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
    uint32 position_count;
    uint8 rgb24;
    uint8 selected;
    volatile uint16 slice_x;
    volatile uint16 slice_y;
    uint16 slice_width;
    uint16 slice_height;
};

extern MoviePlaybackState movie_playback;
void ResetMoviePlaybackState();

uint32 PlayMovie(uint32 movie);
uint32 InitializeMovieResourceTable();
uint32 SelectMovieRecord(uint32 movie);
uint32 PrepareMovieTexture(uint32 resource);
uint32 QueueMoviePosition(const uint32 *position);
struct TextureResource;

uint32 UpdateMoviePlayback(void);
uint32 PrepareMoviePlaybackBuffers(void);
uint32 RequestCdStreamRefresh();
uint32 ResetMoviePlayback();

#endif
