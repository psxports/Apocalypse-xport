#include "movie_stream.h"
#include "game_data_io.h"
#include "psx_spu.h"

static uint32 ring_sectors;
static uint32 reading;
static bool ring_bound;

void BindMovieRing(uint32 *ring, uint32 sectors)
{
    ring_sectors = sectors;
    StSetRing(ring, sectors);
    ring_bound = true;
}

uint32 StartMovieRead(uint32 mode)
{
    uint8 value = mode;
    if (!ControlDataCd(14, &value, 0))
        return 0;
    stream_set_read_mode(mode);
    reading = ControlDataCd(27, 0, 0);
    return reading;
}

uint32 PauseMovieRead()
{
    // Preserve complete queued frames, XA samples and decoder history
    cd_set_reading_native(0);
    reading = 0;
    return 1;
}

uint32 ResumeMovieRead(uint32 mode)
{
    uint8 value = mode;
    if (!ControlDataCd(14, &value, 0))
        return 0;
    stream_set_read_mode(mode);
    cd_set_reading_native(1);
    reading = 1;
    return 1;
}

uint32 PumpMovieRead()
{
    uint32 consumed = 0;
    if (!reading)
        return 0;
    // Service the native CD clock while the caller waits for a complete frame
    psx_native_poll_events();
    for (uint32 remaining = ring_sectors; remaining; --remaining)
    {
        uint16 status = StNextStatus(0, 0);
        if (status == StCOMPLETE || status == StLOCK || stream_pump() <= 0)
            break;
        ++consumed;
    }
    return consumed;
}

uint32 ReadMovieFrame(uint32 **frame, uint32 **header)
{
    PumpMovieRead();
    return StGetNext(frame, header);
}

void StopMovieRead()
{
    reading = 0;
    if (ring_bound)
        StUnSetRing();
    ring_bound = false;
    ring_sectors = 0;
    CdReadyCallback(0);
}
