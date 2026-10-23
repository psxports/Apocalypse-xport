#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "game_diagnostics.h"

static const uint16 distance_root_vals[3072] = {
#include "distance_root_table.inc"
};

bool NamesEqualIgnoringCase(const char *left, const char *right)
{
    FUNCTION_MARKER(0x80067724, "SLUS_003.73");
    if (!left)
        return right == 0;
    if (!right)
        return false;
    uint32 offset = 0;
    for (;;)
    {
        uint8 left_byte = left[offset];
        uint8 right_byte = right[offset];
        if (left_byte >= 'A' && left_byte <= 'Z')
            left_byte += 'a' - 'A';
        if (right_byte >= 'A' && right_byte <= 'Z')
            right_byte += 'a' - 'A';
        if (left_byte != right_byte)
            return false;
        if (!left_byte)
            return true;
        ++offset;
    }
}

uint32 CopyGameString(const char *src, char *dst)
{
    FUNCTION_MARKER(0x80067808, "SLUS_003.73");
    uint32 offset = 0;
    uint8 val;
    do
    {
        dst[offset] = src[offset];
        val = src[offset];
        ++offset;
    } while (val);
    return val;
}

sint32 DivideSignedWord(sint32 numerator, sint32 denominator)
{
    // Raw MIPS DIV has defined zero and overflow results
    if (!denominator)
        return numerator < 0 ? 1 : -1;
    if (numerator == (-2147483647 - 1) && denominator == -1)
        return numerator;
    return numerator / denominator;
}

uint32 MultiplyDivideWord(uint32 factor0, uint32 factor1, uint32 divisor)
{
    FUNCTION_MARKER(0x80085BA4, "SLUS_003.73");
    uint64 product = (uint64)factor0 * factor1;
    uint32 high = product >> 32;
    uint32 low = product;
    // Thirty-two shifts discard all bits of the original incoming V0
    uint32 result = 0;
    for (uint32 bit = 0; bit < 32; ++bit)
    {
        high = (high << 1) | (low >> 31);
        result <<= 1;
        if (high >= divisor)
        {
            high -= divisor;
            result |= 1;
        }
        low <<= 1;
    }
    return result;
}

sint32 RemainderSignedWord(sint32 numerator, sint32 denominator)
{
    // Raw MIPS DIV preserves the dividend on a zero divisor
    if (!denominator)
        return numerator;
    if (numerator == (-2147483647 - 1) && denominator == -1)
        return 0;
    return numerator % denominator;
}

uint32 InverseCosineAngle(sint32 val)
{
    FUNCTION_MARKER(0x80067930, "SLUS_003.73");
    sint32 angle = 512, step = 256;
    bool negative = val < 0;
    // Preserve wrapped negation for the original INT32_MIN case
    if (negative)
        val = -(uint32)val;
    do
    {
        sint32 cosine = rcos(angle);
        if (val < cosine)
            angle += step;
        else if (val == cosine)
            break;
        else
            angle -= step;
        step >>= 1;
    } while (step);
    return negative ? 2048 - angle : angle;
}

sint32 CalculateLaunchAngle(sint32 input)
{
    FUNCTION_MARKER(0x8008702C, "SLUS_003.73");
    return catan(input);
}

sint32 ScaleFixedPoint(sint32 val, uint32 factor)
{
    sint32 product = val * factor;
    return product >> 12;
}

uint32 LookupDistanceRoot(uint32 val)
{
    FUNCTION_MARKER(0x80085B54, "SLUS_003.73");
    xport_gte_write_data(30, val);
    uint32 shift = xport_gte_read_data(31) & 254;
    if (!val)
        return 0;
    bool high_bit = (val & 0x80000000) != 0;
    uint32 idx = high_bit ? val >> 20 : (val << (shift & 31)) >> 20;
    if (idx < 1024 || idx >= 4096)
    {
        GameDiagnostic(("Invalid normalized distance-root index\n"));
        abort();
    }
    uint32 result = distance_root_vals[idx - 1024];
    return high_bit ? result : result >> ((shift >> 1) & 31);
}

void FormatNumber(sint32 val, char *text)
{
    bool emitted = false;
    sint32 divisor = 1000000000, digit;
    FUNCTION_MARKER(0x8006A75C, "SLUS_003.73");
    if (val < 0)
    {
        // Original word negation leaves INT_MIN negative and formats it as -0
        val = -(uint32)val;
        *text++ = '-';
    }
    while (divisor)
    {
        if (val >= divisor)
        {
            digit = val / divisor;
            emitted = true;
            *text++ = digit + '0';
            val -= digit * divisor;
        }
        else if (emitted || divisor == 1)
            *text++ = '0';
        divisor /= 10;
    }
    *text = 0;
}

struct RandomGenerator
{
    uint32 state;
    uint32 multiplier;
    uint32 increment;
};

static RandomGenerator random_generator;
static RandomGenerator auxiliary_random_generator;

void ResetRandomGenerators(void)
{
    RandomGenerator initial = {0, 0, 0};
    random_generator = initial;
    auxiliary_random_generator = initial;
}

uint32 SetRandomSeed(uint32 seed)
{
    FUNCTION_MARKER(0x8006654C, "SLUS_003.73");
    random_generator.state = seed;
    random_generator.multiplier = 314159265;
    random_generator.increment = 178453311;
    return 178453311;
}

uint32 RandomRange(uint32 range)
{
    uint32 state, multiplier, inc;
    sint32 next, product;
    FUNCTION_MARKER(0x80066570, "SLUS_003.73");
    state = random_generator.state;
    multiplier = random_generator.multiplier;
    inc = random_generator.increment;
    next = state * multiplier + inc;
    product = (next & 65535) * range;
    multiplier = (multiplier ^ next) + (next >> 4);
    inc += 0xEFEFEFF0;
    random_generator.state = next;
    inc += (next >> 3);
    random_generator.multiplier = multiplier;
    random_generator.increment = inc;
    // Original result shifts the wrapped product arithmetically
    return product >> 16;
}

uint32 SetAuxiliaryRandomSeed(uint32 seed)
{
    FUNCTION_MARKER(0x800771F4, "SLUS_003.73");
    auxiliary_random_generator.state = seed;
    auxiliary_random_generator.multiplier = 314159265;
    auxiliary_random_generator.increment = 178453311;
    return 178453311;
}

void ShuffleSpawnPoints(uint16 *points, sint32 count)
{
    sint32 idx = 0;
    uint16 val;
    uint32 sel;
    FUNCTION_MARKER(0x80067E18, "SLUS_003.73");
    while (idx < count)
    {
        val = points[idx];
        sel = RandomRange(count);
        points[idx] = points[sel];
        ++idx;
        points[sel] = val;
    }
}

uint32 UpdateAngularAcceleration(ShortVector3 current, ShortVector3 &velocity, ShortVector3 &acceleration, ShortVector3 target, uint32 scale)
{
    FUNCTION_MARKER(0x80066CF0, "SLUS_003.73");
    sint16 dx = target.x - current.x;
    sint16 dy = target.y - current.y;
    if (dx < -2048)
        dx += 4096;
    if (dx > 2048)
        dx -= 4096;
    if (dy < -2048)
        dy += 4096;
    if (dy > 2048)
        dy -= 4096;
    if (!dx && !dy)
    {
        velocity.x = 0;
        velocity.y = 0;
        return 0;
    }
    sint32 product = scale * dx;
    acceleration.x = product >> 8;
    product = scale * dy;
    sint32 result = product >> 8;
    acceleration.y = result;
    return result;
}

uint32 FadePackedColor(uint32 &color, sint32 amount)
{
    FUNCTION_MARKER(0x80032D3C, "SLUS_003.73");
    RGBC val;
    val.rgbc = color;
    sint32 r = val.r, g = val.g, b = val.b;
    // Negative amounts retain the original wrapping brightening rather than an upper clamp
    val.r = r < amount ? 0 : (uint32)r - amount;
    val.g = g < amount ? 0 : (uint32)g - amount;
    val.b = b < amount ? 0 : (uint32)b - amount;
    color = val.rgbc;
    return val.r;
}

uint32 NormalizeShortVector(const Vector3 &vector, Vector3 &normal)
{
    FUNCTION_MARKER(0x800872BC, "SLUS_003.73");
    SVECTOR input, output;
    input.vx = vector.x;
    input.vy = vector.y;
    input.vz = vector.z;
    input.pad = 0;
    uint32 sum = VectorNormalSS(&input, &output);
    // Preserve the original LZCS and LZCR device outputs
    xport_gte_write_data(30, sum);
    normal.x = output.vx;
    normal.y = output.vy;
    normal.z = output.vz;
    return sum;
}

char *AllocateStringCopy(const char *name)
{
    size_t len = strlen(name);
    char *copy = (char *)malloc(len + 1);
    if (!copy)
    {
        GameDiagnostic(("Cannot allocate record name\n"));
        abort();
    }
    memcpy(copy, name, len + 1);
    return copy;
}
