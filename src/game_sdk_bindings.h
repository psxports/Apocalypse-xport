#ifndef GAME_SDK_BINDINGS_H
#define GAME_SDK_BINDINGS_H

#include "psx.h"
#ifdef __cplusplus
extern "C"
{
#endif

sint32 GameDiagnostic(const char *format, ...);

void sub_800878DC(uint32 matrix);
void sub_8008798C(uint32 red, uint32 green, uint32 blue);
void sub_800879AC(uint32 red, uint32 green, uint32 blue);
uint32 sub_8008632C(uint32 mode);
uint32 sub_8008655C(void);
uint32 sub_800865EC(uint32 callback);
void sub_8008793C(uint32 matrix);
uint32 sub_80087E5C(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height);
uint32 sub_80087F10(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height);
uint32 sub_80087F7C(uint32 mode);
uint32 sub_800880F0(uint32 level);
uint32 sub_800882F8(uint32 mode);
uint32 sub_80088B28(uint32 environment);
uint32 sub_8008895C(uint32 environment);
uint32 sub_800888EC(uint32 ordering_table);
uint32 sub_8008BF9C(uint32 format, uint32 value);
uint32 sub_8008E01C(void);
uint32 sub_8008E93C(uint32 serial, uint32 attribute, uint32 value);
uint32 sub_8008F05C(uint32 serial, uint32 left, uint32 right);
uint32 sub_800900AC(uint32 left, uint32 right);
uint32 sub_800945BC(uint32 mode);
uint32 sub_800968DC(uint32 mode);
uint32 sub_800936AC(uint32 count);
uint32 sub_8008F16C(uint32 table, uint32 sequences, uint32 tracks);

uint32 sub_80097DD0(uint32 callback);
uint32 sub_80097DE4(uint32 callback);
uint32 sub_80098214(uint32 callback);
uint32 sub_80098238(uint32 mode);
uint32 sub_8009C788(uint32 callback);

uint32 sub_8008BF34(uint32 text);

uint32 sub_80094F40(uint32 data, uint32 bytes);
uint32 sub_8009986C(uint32 destination, uint32 words);
uint32 sub_8009A7E8(uint32 reset_position);

uint32 sub_8009F92C(uint32 duration);
uint32 sub_8009F94C(void);
uint32 sub_8009E108(uint32 controller, uint32 mode);
uint32 sub_8009EA80(uint32 reason);
uint32 sub_8009EB90(uint32 controller);

uint32 sub_8009D878(uint32 controller);
uint32 sub_8009DB54(uint32 controller);
uint32 sub_8009DED8(uint32 controller);
uint32 sub_8009E054(uint32 controller);
uint32 sub_8009EBD4(uint32 controller, uint32 mode);
uint32 sub_8009ECE4(uint32 controller);

uint32 sub_800981F4(uint32 destination, uint32 words);
uint32 sub_8009C760(uint32 callback);

#ifdef __cplusplus
}
#endif

#endif
