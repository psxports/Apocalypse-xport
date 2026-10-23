#ifndef GLOBAL_H
#define GLOBAL_H

#include "psx.h"
#include "vector3.h"

bool NamesEqualIgnoringCase(const char *left, const char *right);
uint32 CopyGameString(const char *src, char *dst);
sint32 DivideSignedWord(sint32 numerator, sint32 denominator);
uint32 MultiplyDivideWord(uint32 factor0, uint32 factor1, uint32 divisor);
sint32 RemainderSignedWord(sint32 numerator, sint32 denominator);
uint32 InverseCosineAngle(sint32 val);
sint32 CalculateLaunchAngle(sint32 input);
sint32 ScaleFixedPoint(sint32 val, uint32 factor);
uint32 LookupDistanceRoot(uint32 val);
void FormatNumber(sint32 val, char *text);
void ResetRandomGenerators(void);
uint32 SetRandomSeed(uint32 seed);
uint32 RandomRange(uint32 range);
uint32 SetAuxiliaryRandomSeed(uint32 seed);
void ShuffleSpawnPoints(uint16 *points, sint32 count);
uint32 UpdateAngularAcceleration(ShortVector3 current, ShortVector3 &velocity, ShortVector3 &acceleration, ShortVector3 target, uint32 scale);
uint32 FadePackedColor(uint32 &color, sint32 amount);
uint32 NormalizeShortVector(const Vector3 &vector, Vector3 &normal);

char *AllocateStringCopy(const char *name);

#endif
