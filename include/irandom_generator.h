#pragma once
#include "export.h"

#include <cstdint>

namespace chipotto
{
	class CHIP8_API IRandomGenerator
	{
	public:
		virtual uint8_t GetRandomByte() = 0;
		virtual ~IRandomGenerator() {};
	};
}