#include "game_random.h"

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
    uint32 state, multiplier, increment, next, product;
    FUNCTION_MARKER(0x80066570, "SLUS_003.73");
    state = random_generator.state;
    multiplier = random_generator.multiplier;
    increment = random_generator.increment;
    next = state * multiplier + increment;
    product = (next & 65535) * range;
    multiplier = (multiplier ^ next) + (uint32)((sint32)next >> 4);
    increment += 0xEFEFEFF0;
    random_generator.state = next;
    increment += (uint32)((sint32)next >> 3);
    random_generator.multiplier = multiplier;
    random_generator.increment = increment;
    // Original result shifts the wrapped product arithmetically
    return (uint32)((sint32)product >> 16);
}

uint32 SetAuxiliaryRandomSeed(uint32 seed)
{
    FUNCTION_MARKER(0x800771F4, "SLUS_003.73");
    auxiliary_random_generator.state = seed;
    auxiliary_random_generator.multiplier = 314159265;
    auxiliary_random_generator.increment = 178453311;
    return 178453311;
}

