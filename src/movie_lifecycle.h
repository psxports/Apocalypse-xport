#ifndef MOVIE_LIFECYCLE_H
#define MOVIE_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

uint32 PlayMovie(uint32 movie);
uint32 InitializeMovieResourceTable();
uint32 WaitForMovieCdStop();
uint32 SelectMovieRecord(uint32 movie);
uint32 PrepareMovieTexture(uint32 resource);
uint32 QueueMoviePosition(const uint32 *position);
uint32 RemapMovieTexturePolygons(uint32 tpage, const uint8 *texture);

uint32 UpdateMoviePlayback(void);
uint32 PrepareMoviePlaybackBuffers(void);
uint32 InitializeMovieDecoder(void);
uint32 OutputMovieSlice(void);
uint32 NextMovieFrame(void);
uint32 DecodeNextMovieFrame(void);
uint32 StartMovieStream(void);
uint32 ResetMoviePlayback();
void ReleaseMovieFrameBuffers();

#endif
