#include "game_mdec.h"
#include "draft_first_signatures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Project-local MDEC command, coefficient, transform and output state */
static struct {
  uint8 quant[2][64];
  sint16 basis[8][8];
  uint32 input, remaining, command, callback;
  uint8 pixels[768];
  uint32 pixel_count, pixel_cursor;
  uint32 output, output_words;
  int requested, pumping, initialized;
} mdec;

static void mdec_error(const char *message)
{
  fprintf(stderr, "MDEC: %s\n", message);
  abort();
}

static sint32 mdec_clamp(sint32 value, sint32 low, sint32 high)
{
  return value < low ? low : value > high ? high : value;
}

static sint32 mdec_signed(uint32 value, unsigned bits)
{
  uint32 sign = 1u << (bits - 1);
  return (sint32)((value & (2u * sign - 1u)) ^ sign) - (sint32)sign;
}

static uint32 mdec_halfword(void)
{
  uint32 value;
  if (!mdec.remaining) mdec_error("Incomplete compressed block");
  value = r_u16(mdec.input);
  mdec.input += 2;
  --mdec.remaining;
  return value;
}

/* Construct the diagonal coefficient ordering rather than store a host table */
static unsigned mdec_coefficient_index(unsigned ordinal)
{
  unsigned diagonal, position = 0;
  for (diagonal = 0; diagonal <= 14; ++diagonal) {
    unsigned first = diagonal < 8 ? 0 : diagonal - 7;
    unsigned last = diagonal < 8 ? diagonal : 7;
    unsigned k;
    for (k = first; k <= last; ++k) {
      unsigned row = diagonal & 1 ? k : diagonal - k;
      unsigned column = diagonal - row;
      if (position++ == ordinal) return row * 8 + column;
    }
  }
  mdec_error("Invalid coefficient index");
  return 0;
}

static void mdec_block(sint16 samples[64], unsigned quantizer)
{
  sint32 coefficient[64] = {0};
  long long intermediate[64];
  uint32 code, scale;
  unsigned ordinal = 0, row, column, frequency;
  do { code = mdec_halfword(); } while (code == 0xFE00u);
  scale = code >> 10;
  for (;;) {
    sint32 value = mdec_signed(code, 10);
    if (!scale) value *= 2;
    else if (!ordinal) value *= mdec.quant[quantizer][0];
    else value = (value * mdec.quant[quantizer][ordinal] * (sint32)scale + 4) / 8;
    coefficient[scale ? mdec_coefficient_index(ordinal) : ordinal] = mdec_clamp(value, -1024, 1023);
    if (ordinal == 63) break;
    code = mdec_halfword();
    ordinal += (code >> 10) + 1;
    if (ordinal >= 64) break;
  }
  for (row = 0; row < 8; ++row) {
    for (column = 0; column < 8; ++column) {
      long long sum = 0;
      for (frequency = 0; frequency < 8; ++frequency)
        sum += (long long)coefficient[frequency * 8 + column] * mdec.basis[row][frequency];
      intermediate[row * 8 + column] = sum;
    }
  }
  for (row = 0; row < 8; ++row) {
    for (column = 0; column < 8; ++column) {
      long long sum = 0;
      sint32 value;
      for (frequency = 0; frequency < 8; ++frequency)
        sum += intermediate[row * 8 + frequency] * mdec.basis[column][frequency];
      value = mdec_signed((uint32)((sum + 0x80000000LL) >> 32), 9);
      samples[row * 8 + column] = (sint16)mdec_clamp(value, -128, 127);
    }
  }
}

static void mdec_macroblock(void)
{
  sint16 block[6][64];
  unsigned depth = (mdec.command >> 27) & 3u;
  unsigned signed_output = (mdec.command >> 26) & 1u;
  unsigned bit15 = (mdec.command >> 25) & 1u;
  unsigned i, x, y;
  mdec.pixel_cursor = 0;
  if (depth < 2) {
    mdec_block(block[0], 0);
    for (i = 0; i < 64; ++i) {
      uint8 luminance = (uint8)(block[0][i] + (signed_output ? 0 : 128));
      if (depth == 1) mdec.pixels[i] = luminance;
      else if (!(i & 1)) mdec.pixels[i / 2] = luminance >> 4;
      else mdec.pixels[i / 2] |= luminance & 0xF0u;
    }
    mdec.pixel_count = depth == 1 ? 64 : 32;
    return;
  }
  for (i = 0; i < 6; ++i) mdec_block(block[i], i < 2 ? 1 : 0);
  for (y = 0; y < 16; ++y) {
    for (x = 0; x < 16; ++x) {
      unsigned chroma = (y / 2) * 8 + x / 2;
      unsigned quadrant = 2 + (y / 8) * 2 + x / 8;
      sint32 cr = block[0][chroma], cb = block[1][chroma];
      sint32 luminance = block[quadrant][(y & 7) * 8 + (x & 7)];
      sint32 red = mdec_signed((uint32)(luminance + ((359 * cr + 128) >> 8)), 9);
      sint32 green = mdec_signed((uint32)(luminance + ((((-88 * cb) & ~31) + ((-183 * cr) & ~7) + 128) >> 8)), 9);
      sint32 blue = mdec_signed((uint32)(luminance + ((454 * cb + 128) >> 8)), 9);
      uint8 r = (uint8)(mdec_clamp(red, -128, 127) + (signed_output ? 0 : 128));
      uint8 g = (uint8)(mdec_clamp(green, -128, 127) + (signed_output ? 0 : 128));
      uint8 b = (uint8)(mdec_clamp(blue, -128, 127) + (signed_output ? 0 : 128));
      unsigned pixel = y * 16 + x;
      if (depth == 2) {
        mdec.pixels[pixel * 3] = r;
        mdec.pixels[pixel * 3 + 1] = g;
        mdec.pixels[pixel * 3 + 2] = b;
      } else {
        uint16 rgb = (uint16)((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10) | (bit15 << 15));
        mdec.pixels[pixel * 2] = (uint8)rgb;
        mdec.pixels[pixel * 2 + 1] = (uint8)(rgb >> 8);
      }
    }
  }
  mdec.pixel_count = depth == 2 ? 768 : 512;
}

void sub_8009BC3C(uint32 mode)
{
  unsigned i, x, y;
  if (mode > 1) mdec_error("Unsupported reset mode");
  mdec.input = mdec.remaining = mdec.pixel_count = mdec.pixel_cursor = 0;
  mdec.requested = 0;
  if (!mode) {
    for (i = 0; i < 128; ++i) mdec.quant[i / 64][i % 64] = r_u8(0x800FDE60u + i);
    for (y = 0; y < 8; ++y)
      for (x = 0; x < 8; ++x)
        mdec.basis[y][x] = (sint16)r_u16(0x800FDEE4u + (x * 8 + y) * 2);
    mdec.initialized = 1;
  }
}

uint32 sub_8009BED8(uint32 callback)
{
  uint32 previous = mdec.callback;
  mdec.callback = callback;
  return previous;
}

void sub_8009BDA0(uint32 input, uint32 mode)
{
  uint32 command = r_u32(input);
  if (!mdec.initialized) mdec_error("Decode before table initialization");
  command = mode & 1 ? command & 0xF7FFFFFFu : command | 0x08000000u;
  command = mode & 2 ? command | 0x02000000u : command & 0xFDFFFFFFu;
  w_u32(input, command);
  if ((command >> 29) != 1) mdec_error("Unsupported input command");
  mdec.command = command;
  mdec.input = input + 4;
  mdec.remaining = (command & 0xFFFFu) * 2;
  mdec.pixel_count = mdec.pixel_cursor = 0;
}

void sub_8009BE1C(uint32 output, uint32 words)
{
  if (mdec.requested) mdec_error("Output DMA request already queued");
  mdec.output = output;
  /* The SDK DMA block size is thirty-two words */
  mdec.output_words = words & ~31u;
  mdec.requested = 1;
  if (mdec.pumping) return;
  mdec.pumping = 1;
  while (mdec.requested) {
    uint32 destination = mdec.output;
    uint32 bytes = mdec.output_words * 4;
    uint32 i;
    mdec.requested = 0;
    for (i = 0; i < bytes; ++i) {
      if (mdec.pixel_cursor == mdec.pixel_count) mdec_macroblock();
      w_u8(destination + i, mdec.pixels[mdec.pixel_cursor++]);
    }
    if (mdec.callback == 0x8002F84Cu) sub_8002F84C();
    else if (mdec.callback) mdec_error("Unknown output callback");
  }
  mdec.pumping = 0;
}