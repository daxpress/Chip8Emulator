#include "sdl/emulator_random_generator.h"
#include <random>

namespace chipotto
{
    uint8_t EmulatorRandomGenerator::GetRandomByte()
    {
        return (std::rand() % 256);
    }
}