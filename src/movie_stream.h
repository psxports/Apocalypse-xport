#ifndef MOVIE_STREAM_H
#define MOVIE_STREAM_H

#include "psx_stream.h"

void BindMovieRing(uint32 *ring, uint32 sectors);
uint32 StartMovieRead(uint32 mode);
uint32 PauseMovieRead();
uint32 ResumeMovieRead(uint32 mode);
uint32 PumpMovieRead();
uint32 ReadMovieFrame(uint32 **frame, uint32 **header);
void StopMovieRead();

#endif
