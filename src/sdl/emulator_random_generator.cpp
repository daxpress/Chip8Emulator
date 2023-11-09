#include "sdl/emulator_random_generator.h"
#include <random>
#include <ctime>

namespace chipotto
{
    EmulatorRandomGenerator::EmulatorRandomGenerator()
    {
        srand(time(nullptr));
    }

    uint8_t EmulatorRandomGenerator::GetRandomByte()
    {
        return (std::rand() % 256);
    }
}